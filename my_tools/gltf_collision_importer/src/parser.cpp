/**
* @copyright 2023 - Max Bebök
* @license MIT
*/

#define CGLTF_IMPLEMENTATION

#include <filesystem>

#include <string>
#include "parser.h"
#include "hash.h"

#include "math/vec2.h"
#include "math/vec3.h"

#include "lib/meshopt/meshoptimizer.h"
#include "math/mat4.h"
#include "parser/parser.h"
#include "converter/converter.h"

#include "binaryFile.h"

typedef struct OctreeFloat{
	    Vec3 AABB_Min;
	    Vec3 AABB_Max;
	    uint16_t numVerts;// (16 bit should be enough)
	    uint16_t id;
	    std::vector<Vec3> verts;
	    std::vector<OctreeFloat> children;// (actual child nodes!)
} OctreeFloat;

namespace {
  const std::vector<std::string> BAD_VERSIONS{
    "Blender I/O v4.1",
    "Blender I/O v4.2",
    "Blender I/O v4.3",
    "Blender I/O v4.4",
  };
}

void printBoneTree(const Bone &bone, int depth)
{
  for(int i=0; i<depth; ++i)printf("  ");
  printf("%s\n", bone.name.c_str());
  bone.parentMatrix.print(depth * 2);
  for(auto &child : bone.children) {
    printBoneTree(*child, depth+1);
  }
}

void WriteOctree(struct OctreeFloat* node, BinaryFile* file);
void SplitOctree(int depth, struct OctreeFloat* node);
void AABB_Extents_to_MinMax(Vec3* min, Vec3* max, const Vec3 c, const Vec3 e);
bool SAT_AABB_Tri(const struct OctreeFloat* node, const Vec3* verts);

std::vector<ModelCustom> parseGLTFCustom(const char *gltfPath, const char *t3dmPath, float modelScale)
{
  std::vector<ModelCustom> allModels{};
  T3DMData t3dm{};
  fs::path gltfBasePath{gltfPath};
  gltfBasePath = gltfBasePath.parent_path();

  cgltf_options options{};
  cgltf_data* data = nullptr;
  cgltf_result result = cgltf_parse_file(&options, gltfPath, &data);

  if(result == cgltf_result_file_not_found) {
    fprintf(stderr, "Error: File not found! (%s)\n", gltfPath);
    throw std::runtime_error("File not found!");
  }

  if(cgltf_validate(data) != cgltf_result_success) {
    fprintf(stderr, "Invalid glTF data!\n");
    throw std::runtime_error("Invalid glTF data!");
  }

  cgltf_load_buffers(&options, data, gltfPath);


  // Meshes
  for(int i=0; i<data->nodes_count; ++i)
  {
    auto node = &data->nodes[i];
    //printf("- Node %d: %s\n", i, node->name);

    auto mesh = node->mesh;
    if(!mesh)continue;

    // printf(" - Mesh %d: %s\n", i, mesh->name);

    for(int j = 0; j < mesh->primitives_count; j++)
    {
      t3dm.models.push_back({});
      auto &model = t3dm.models.back();
      if(node->name)model.name = node->name;

      auto prim = &mesh->primitives[j];
      //printf("   - Primitive %d:\n", j);

      /*if(prim->material) {
        parseMaterial(gltfBasePath, i, j, model, prim);
      }*/

      // find vertex count
      int vertexCount = 0;
      for(int k = 0; k < prim->attributes_count; k++) {
        if(prim->attributes[k].type == cgltf_attribute_type_position) {
          vertexCount = prim->attributes[k].data->count;
          break;
        }
      }

      std::vector<VertexNorm> vertices{};
      vertices.resize(vertexCount, {.color = {1.0f, 1.0f, 1.0f, 1.0f}, .boneIndex = -1});
      std::vector<uint16_t> indices{};

      // Read indices
      if(prim->indices != nullptr)
      {
        auto acc = prim->indices;
        auto basePtr = ((uint8_t*)acc->buffer_view->buffer->data) + acc->buffer_view->offset + acc->offset;
        auto elemSize = Gltf::getDataSize(acc->component_type);

        for(int k = 0; k < acc->count; k++) {
          indices.push_back(Gltf::readAsU32(basePtr, acc->component_type));
          basePtr += elemSize;
        }
      }

      // Read vertices
      for(int k = 0; k < prim->attributes_count; k++)
      {
        auto attr = &prim->attributes[k];
        auto acc = attr->data;
        auto basePtr = ((uint8_t*)acc->buffer_view->buffer->data) + acc->buffer_view->offset + acc->offset;
        auto elemSize = Gltf::getDataSize(acc->component_type);

        // printf("     - Attribute %d: %s\n", k, attr->name);
        if(attr->type == cgltf_attribute_type_position)
        {
          assert(attr->data->type == cgltf_type_vec3);

          for(int l = 0; l < acc->count; l++)
          {
            auto &v = vertices[l];
            v.pos[0] = Gltf::readAsFloat(basePtr, acc->component_type); basePtr += elemSize;
            v.pos[1] = Gltf::readAsFloat(basePtr, acc->component_type); basePtr += elemSize;
            v.pos[2] = Gltf::readAsFloat(basePtr, acc->component_type); basePtr += elemSize;
          }
        }

        /*if(attr->type == cgltf_attribute_type_color)
        {
          assert(attr->data->type == cgltf_type_vec4);

          for(int l = 0; l < acc->count; l++)
          {
            auto &v = vertices[l];
            v.color[0] = Gltf::readAsFloat(basePtr, acc->component_type); basePtr += elemSize;
            v.color[1] = Gltf::readAsFloat(basePtr, acc->component_type); basePtr += elemSize;
            v.color[2] = Gltf::readAsFloat(basePtr, acc->component_type); basePtr += elemSize;
            v.color[3] = Gltf::readAsFloat(basePtr, acc->component_type); basePtr += elemSize;

            // linear to gamma
            for(int c=0; c<3; ++c) {
              v.color[c] = powf(v.color[c], 0.4545f);
            }
          }
        }*/

        if(attr->type == cgltf_attribute_type_normal)
        {
          assert(attr->data->type == cgltf_type_vec3);

          for(int l = 0; l < acc->count; l++)
          {
            auto &v = vertices[l];
            v.norm[0] = Gltf::readAsFloat(basePtr, acc->component_type); basePtr += elemSize;
            v.norm[1] = Gltf::readAsFloat(basePtr, acc->component_type); basePtr += elemSize;
            v.norm[2] = Gltf::readAsFloat(basePtr, acc->component_type); basePtr += elemSize;
          }
        }

        /*if(attr->type == cgltf_attribute_type_texcoord)
        {
          assert(attr->data->type == cgltf_type_vec2);

          for(int l = 0; l < acc->count; l++)
          {
            auto &v = vertices[l];
            v.uv[0] = Gltf::readAsFloat(basePtr, acc->component_type); basePtr += elemSize;
            v.uv[1] = Gltf::readAsFloat(basePtr, acc->component_type); basePtr += elemSize;
          }
        }

        if(attr->type == cgltf_attribute_type_joints)
        {
          assert(attr->data->type == cgltf_type_vec4);
          for(int l = 0; l < acc->count; l++)
          {
            auto &v = vertices[l];
            u32 joins[4];
            for(int c=0; c<4; ++c) {
              joins[c] = Gltf::readAsU32(basePtr, acc->component_type); basePtr += elemSize;
            }
            //printf("  - %d %d %d %d\n", joins[0], joins[1], joins[2], joins[3]);
            v.boneIndex = joins[0];
            if(v.boneIndex >= boneCount || v.boneIndex < 0)v.boneIndex = -1;
          }
        }

        if(attr->type == cgltf_attribute_type_weights)
        {
          assert(attr->data->type == cgltf_type_vec4);

          for(int l = 0; l < acc->count; l++)
          {
            auto &v = vertices[l];
            float weights[4];
            for(int c=0; c<4; ++c) {
              weights[c] = Gltf::readAsFloat(basePtr, acc->component_type); basePtr += elemSize;
            }
            //printf("  - %f %f %f %f\n", weights[0], weights[1], weights[2], weights[3]);
          }
        }*/
      }

      /*std::vector<VertexT3D> verticesT3D{};
      verticesT3D.resize(vertices.size());

      float texSizeX = model.material.texA.texWidth;
      float texSizeY = model.material.texA.texHeight;

      if(texSizeX == 0)texSizeX = 32;
      if(texSizeY == 0)texSizeY = 32;

      // convert vertices
      for(int k = 0; k < vertices.size(); k++) {
        Mat4 mat = parseNodeMatrix(node);
        convertVertex(
          modelScale, texSizeX, texSizeY, vertices[k], verticesT3D[k],
          mat, matrixStack, model.material.uvFilterAdjust
        );
      }*/

      // optimizations
      // meshopt_optimizeVertexCache(indices.data(), indices.data(), indices.size(), vertices.size());
//meshopt_optimizeOverdraw(indices.data(), indices.data(), indices.size(), &vertices[0].pos.data[0], vertices.size(), sizeof(VertexNorm), 1.05f);

      // expand into triangles, this is used to split up and dedupe data

      // Resting pose matrix stack, used to pre-transform vertices
      std::vector<Mat4> matrixStack{};
      std::unordered_map<std::string, const Bone*> boneMap{};
      if(!t3dm.skeletons.empty()) {
        auto addBoneMax = [&](auto&& addBoneMax, const Bone &bone) -> void {
          matrixStack.push_back(bone.inverseBindPose);
          boneMap[bone.name] = &bone;
          for(auto &child : bone.children) {
            addBoneMax(addBoneMax, *child);
          }
        };
        for(const auto &skel : t3dm.skeletons) {
          addBoneMax(addBoneMax, skel);
        }
      }

      std::vector<VertexT3D> verticesT3D{};
      verticesT3D.resize(vertices.size());

      float texSizeX = model.material.texA.texWidth;
      float texSizeY = model.material.texA.texHeight;

      if(texSizeX == 0)texSizeX = 32;
      if(texSizeY == 0)texSizeY = 32;

      // convert vertices
      for(int k = 0; k < vertices.size(); k++) {
        Mat4 mat = parseNodeMatrix(node);
        convertVertex(
          modelScale, texSizeX, texSizeY, vertices[k], verticesT3D[k],
          mat, matrixStack, model.material.uvFilterAdjust
        );
      }

      //std::vector<TriangleCustom> triangles{};
      std::vector<TriangleT3D> triangles{};


      ModelCustom modelTemp;


      triangles.reserve(indices.size() / 3);

      for(int k = 0; k < indices.size(); k += 3) {
        /*triangles.push_back({
          vertices[indices[k + 0]],
          vertices[indices[k + 1]],
          vertices[indices[k + 2]],
        });*/
        triangles.push_back({
          verticesT3D[indices[k + 0]],
          verticesT3D[indices[k + 1]],
          verticesT3D[indices[k + 2]],
        });
      }

      modelTemp.triangles = triangles;

      allModels.push_back(modelTemp);

      


      std::vector<Vec3> floatVerts;
      std::vector<Vec3> firstVerts;
      firstVerts.resize(vertices.size());
      floatVerts.resize(indices.size());

      for(int k = 0; k < vertices.size(); k++) {
        Mat4 mat = parseNodeMatrix(node);
        firstVerts[k] = mat * vertices[k].pos * modelScale;
      }
      for(int k = 0; k < indices.size(); k += 3) {
        floatVerts[k] = firstVerts[indices[k + 0]];
        floatVerts[k+1] = firstVerts[indices[k + 1]];
        floatVerts[k+2] = firstVerts[indices[k + 2]];
      }
      /*floatVerts.resize(triangles.size()*3);
      for(int k = 0; k < triangles.size()*3; k+=3) {
        floatVerts[k] = (Vec3){triangles[k/3].vert[0].pos[0], triangles[k/3].vert[0].pos[1], triangles[k/3].vert[0].pos[2]};
        floatVerts[k+1] = (Vec3){triangles[k/3].vert[1].pos[0], triangles[k/3].vert[1].pos[1], triangles[k/3].vert[1].pos[2]};
        floatVerts[k+2] = (Vec3){triangles[k/3].vert[2].pos[0], triangles[k/3].vert[2].pos[1], triangles[k/3].vert[2].pos[2]};
      }*/

      uint16_t totalTriCount = 0;
      uint16_t totalVertexCount = 0;
      uint16_t id = 0;
      Vec3 AABB_Max = (Vec3){0.f, 0.f, 0.f};
      Vec3 AABB_Min = (Vec3){0.f, 0.f, 0.f};

      BinaryFile file{};

  /*    file.write<float>(1);//AABB_Min, T3DVec3 (set later)
      file.write<float>(2);//AABB_Min, T3DVec3 (set later)
      file.write<float>(3);//AABB_Min, T3DVec3 (set later)
      file.write<float>(1);//AABB_Max, T3DVec3 (set later)
      file.write<float>(2);//AABB_Max, T3DVec3 (set later)
      file.write<float>(3);//AABB_Max, T3DVec3 (set later)
      file.write<uint16_t>(69); // total vertex count (set later)*/
  //file.write<uint16_t>(420);//indices (remove later)
      //file.write<uint16_t>(69); // id (set later)
      //Write Verticies

      //WARNING! this only takes pos fixed point, doesn't include s and t pixel coords. honestly, I don't know if that matters, but it might be why everything is so shakey...
      /*AABB_Max[0] = allModels[0].triangles[0].vert[0].pos[0];//this copies just the pos, do we want full VertexT3D?
      AABB_Max[1] = allModels[0].triangles[0].vert[0].pos[1];
      AABB_Max[2] = allModels[0].triangles[0].vert[0].pos[2];
      AABB_Min[0] = allModels[0].triangles[0].vert[0].pos[0];//lots more data, is it worth it? needed for collision?
      AABB_Min[1] = allModels[0].triangles[0].vert[0].pos[1];
      AABB_Min[2] = allModels[0].triangles[0].vert[0].pos[2];*/
      AABB_Max = floatVerts[0];
      AABB_Min = floatVerts[0];


      for (int i = 0; i < floatVerts.size(); i+=3)//only works if one model...
      {
        //find AABB min/max
            for (int j = 0; j < 3; j++){
                if (floatVerts[i][j] < AABB_Min[j])//first verts (x) comp
                {
                    AABB_Min[j] = floatVerts[i][j];
                }
                if (floatVerts[i+1][j] < AABB_Min[j])//second verts ...
                {
                    AABB_Min[j] = floatVerts[i+1][j];
                }
                if (floatVerts[i+2][j] < AABB_Min[j])//third verts ...
                {
                    AABB_Min[j] = floatVerts[i+2][j];
                }
            }
            for (int j = 0; j < 3; j++){
                if (floatVerts[i][j] > AABB_Max[j])
                {
                    AABB_Max[j] = floatVerts[i][j];
                }
                if (floatVerts[i+1][j] > AABB_Max[j])
                {
                    AABB_Max[j] = floatVerts[i+1][j];
                }
                if (floatVerts[i+2][j] > AABB_Max[j])
                {
                    AABB_Max[j] = floatVerts[i+2][j];
                }
            }
      }

    struct OctreeFloat collisionOctree = {
      .AABB_Min = AABB_Min,
      .AABB_Max = AABB_Max,
      .numVerts = floatVerts.size(),// (16 bit should be enough)
      .id = 0,
      .verts = floatVerts,
      //.children;// 
    };

    //collisionOctree.tris.resize(4);
    //for (int i = 0; i < floatVerts.size(); i+=3)//only works if one model...
    //{
      /*collisionOctree.tris[i/3][0] = floatVerts[i];
      collisionOctree.tris[i/3][1] = floatVerts[i+1];
      collisionOctree.tris[i/3][2] = floatVerts[i+2];*/
    //}

    SplitOctree(3, &collisionOctree);
printf("So, are we getting here?\n");
    //Now, finally write to file

    //*********inital write ***************/

      file.writeChars("COL", 3);
    file.write<uint8_t>(42);//magic                               //4 bytes
    file.write<uint16_t>(collisionOctree.id); // id 2 bytes
    file.write<uint16_t>(collisionOctree.numVerts); // total vertex count 2 bytes
    file.write<float>(collisionOctree.AABB_Min[0]);//AABB_Min 4 bytes
    file.write<float>(collisionOctree.AABB_Min[1]);//AABB_Min 4 bytes
    file.write<float>(collisionOctree.AABB_Min[2]);//AABB_Min 4 bytes
    file.write<float>(collisionOctree.AABB_Max[0]);//AABB_Max 4 bytes
    file.write<float>(collisionOctree.AABB_Max[1]);//AABB_Max 4 bytes
    file.write<float>(collisionOctree.AABB_Max[2]);//AABB_Max 4 bytes
                                                              //total 32 bytes
    if (collisionOctree.children.size() != 0)
    {
      for(int i = 0; i < collisionOctree.children.size(); i++)
      {
        file.write<uint16_t>(collisionOctree.children[i].id);
      }
    }
    else
    {
      for(int i = 0; i < 8; i++)
      {
        file.write<uint16_t>(9999);
      }
    }

    /*for(int i = 0; i < node->verts.size(); i++)
    {
      file->write<float>(node->verts[i][0]);
      file->write<float>(node->verts[i][1]);
      file->write<float>(node->verts[i][2]);
    }*/

    if (collisionOctree.children.size() != 0 && collisionOctree.numVerts != 0)
    {
      for (int i = 0; i < collisionOctree.children.size(); i++)
      {
        WriteOctree(&collisionOctree.children[i], &file);
      }
    }


   
    //WriteOctree(&collisionOctree, &file);

    file.writeToFile(t3dmPath);

    }





    
  }

  cgltf_free(data);

  return allModels;//custom model struct
}

void WriteOctree(struct OctreeFloat* node, BinaryFile* file)
{          
    file->writeChars("COL", 3);
    file->write<uint8_t>(42);//magic                               //4 bytes
    file->write<uint16_t>(node->id); // id 2 bytes
    file->write<uint16_t>(node->numVerts); // total vertex count 2 bytes
    file->write<float>(node->AABB_Min[0]);//AABB_Min 4 bytes
    file->write<float>(node->AABB_Min[1]);//AABB_Min 4 bytes
    file->write<float>(node->AABB_Min[2]);//AABB_Min 4 bytes
    file->write<float>(node->AABB_Max[0]);//AABB_Max 4 bytes
    file->write<float>(node->AABB_Max[1]);//AABB_Max 4 bytes
    file->write<float>(node->AABB_Max[2]);//AABB_Max 4 bytes
                                                              //total 32 bytes
    if (node->children.size() != 0)
    {
      for(int i = 0; i < node->children.size(); i++)
      {
        file->write<uint16_t>(node->children[i].id);
      }
    }
    else
    {
      for(int i = 0; i < 8; i++)
      {
        file->write<uint16_t>(9999);
      }
    }

    for(int i = 0; i < node->verts.size(); i++)
    {
      file->write<float>(node->verts[i][0]);
      file->write<float>(node->verts[i][1]);
      file->write<float>(node->verts[i][2]);
    }

    if (node->children.size() != 0 && node->numVerts != 0)
    {
      for (int i = 0; i < node->children.size(); i++)
      {
        WriteOctree(&node->children[i], file);
      }
    }


}

void SplitOctree(int depth, struct OctreeFloat* node)
{
  if (depth == 0) return;

  //convert to center-extents
  Vec3 center = (node->AABB_Max + node->AABB_Min) / 2.f;
  //center[0] = (node->AABB_Max[0] + node->AABB_Min[0]) / 2;
  //center[1] = (node->AABB_Max[1] + node->AABB_Min[1]) / 2;
  //center[2] = (node->AABB_Max[2] + node->AABB_Min[2]) / 2;
  Vec3 extents = node->AABB_Max - center;
  ///extents[0] = node->AABB_Max[0] - center[0];
  ///extents[1] = node->AABB_Max[1] - center[1];
  ///extents[2] = node->AABB_Max[2] - center[2];
  Vec3 halfExtents = extents / 2.f;
  ///halfExtents[0] = extents[0] / 2;
  ///halfExtents[1] = extents[1] / 2;
  ///halfExtents[2] = extents[2] / 2;

  //create 8 new sub octrees (nodes)
   Vec3 tempSides[8];
   tempSides[0][0] = -halfExtents[0]; tempSides[0][1] = halfExtents[1]; tempSides[0][2] = -halfExtents[2];
   tempSides[1][0] = halfExtents[0]; tempSides[1][1] = halfExtents[1]; tempSides[1][2] = -halfExtents[2];
   tempSides[2][0] = -halfExtents[0]; tempSides[2][1] = halfExtents[1]; tempSides[2][2] = halfExtents[2];
   tempSides[3][0] = halfExtents[0]; tempSides[3][1] = halfExtents[1]; tempSides[3][2] = halfExtents[2];

   tempSides[4][0] = -halfExtents[0]; tempSides[4][1] = -halfExtents[1]; tempSides[4][2] = -halfExtents[2];
   tempSides[5][0] = halfExtents[0]; tempSides[5][1] = -halfExtents[1]; tempSides[5][2] = -halfExtents[2];
   tempSides[6][0] = -halfExtents[0]; tempSides[6][1] = -halfExtents[1]; tempSides[6][2] = halfExtents[2];
   tempSides[7][0] = halfExtents[0]; tempSides[7][1] = -halfExtents[1]; tempSides[7][2] = halfExtents[2];

   Vec3 centers[8];
   for (int i = 0; i < 8; i++)
   {
      centers[i] = center + tempSides[i];
   }

   Vec3 tempMin;
   Vec3 tempMax;
   //node->children = malloc(8 * sizeof(Octree));
   printf("Before extends to min max\n");
   
      if (node->numVerts == 0) return;


   for (int i = 0; i < 8; i++)//Create new children
    {
     // AABB_Extents_to_MinMax(&tempMin, &tempMax, centers[i], halfExtents);
      tempMax = halfExtents + centers[i];
      tempMin = centers[i] * 2 - tempMax;//Min = c * 2 - Max

      struct OctreeFloat childOctree = {
        .AABB_Min = tempMin,
        .AABB_Max = tempMax,
        .numVerts = 0,// temp
        .id = 10*node->id + i + 1
        //.tris = allModels[0].triangles,
        //.children;// 
      };
      /*node->children[i] = (Octree) {
        .AABB_Min = TempMin,
        .AABB_Max = TempMax,
        .numVertices = 0,
        .children = NULL
      };*/
      node->children.push_back(childOctree);
    }
    printf("Now SAT AABB Tri\n");
    for (int i = 0; i < 8; i++)//give children verts
    {
      //bool gotavert = false;
      printf("^^^^^^^^^^^^^^i : %d^^^^^^^^^^^^^\n", i);
      printf("num verts: %d\n", node->numVerts);
      for (int j = 0; j < node->numVerts; j+=3)
      {
        printf("j : %d\n", j);
        if (SAT_AABB_Tri(&node->children[i], &node->verts[j]))//Test tri against AABB (SAT TEST!)
        {
          printf("SAT loopin...\n");
          //gotavert = true;
          //node->children[i].tris[node->children[i].numTris] = node->tris[j];
          node->children[i].verts.push_back(node->verts[j]);
          node->children[i].verts.push_back(node->verts[j+1]);
          node->children[i].verts.push_back(node->verts[j+2]);
          node->children[i].numVerts+=3;
        }
      }
      printf("******loop end BB******\n");
      /*if(gotavert)
      {
          if (node->children[i].vertices == NULL)
      }*/

      if (node->children[i].numVerts != 0)
      {
        //depth--;
        printf("let's split again!\n");
        SplitOctree(depth - 1, &node->children[i]);
      }
      else//no verts, end of splitting... wait, should we delete nodes that are totally empty?
      {
        printf("continue...\n");
        continue;
      }
    }
    printf("and it's ALL over! depth = %d\n", depth);

}

void AABB_Extents_to_MinMax(Vec3* min, Vec3* max, const Vec3 c, const Vec3 e)
{
  printf("extents to min max...\n");
  //t3d_vec3_add(Max, e, c); //Max = e + c
  *max = e + c;
  *min = c * 2 - *max;//Min = c * 2 - Max
  
  //scaleVector(Min, 2.f);
  //t3d_vec3_diff(Min, Min, Max);
}

bool TestAxisTriAABB(Vec3* e, Vec3* axis, Vec3* v0, Vec3* v1, Vec3* v2,
                      Vec3* u0, Vec3* u1, Vec3* u2)
{
  printf("Test Axis tri AABB\n");
  //debugf("bonjour\n");
  //project all 3 verts onto the axis
  float p0 = v0->dot(*axis);
  float p1 = v1->dot(*axis);
  float p2 = v2->dot(*axis);
  printf("After dots\n");
  //project AABB onto the axis (just care about length of half-size of AABB since at origin)
  float r = e->data[0] * fabs(u0->dot(*axis)) +
            e->data[1] * fabs(u1->dot(*axis)) +
            e->data[2] * fabs(u2->dot(*axis));
  printf("after project onto axis\n");
  //do actual test, see if both points of projected triangle are outside the projected half-length AABB
  //  if so we can exit (return false)
  float pMax = p0;
  float pMin = p0;
  if(p1 > pMax) pMax = p1;
  if(p2 > pMax) pMax = p2;
  if(p1 < pMin) pMin = p1;
  if(p2 < pMin) pMin = p2;
  printf("first compare\n");

  float finalMax;
  if (-pMax > pMin)
  {
    finalMax = -pMax;
  }
  else
  {
    finalMax = pMin;
  }

  if (finalMax > r)
  {
    printf("--------------------return FALSE\n");
    return false;
  }
  printf("--------------------return true\n");
  return true;
}

bool SAT_AABB_Tri(const struct OctreeFloat* node, const Vec3* verts)
{
  Vec3 c = (node->AABB_Max + node->AABB_Min) / 2;
  //t3d_vec3_add(&c, &node->AABB_Max, &node->AABB_Min);
  //scaleVector(&c, 0.5f);

  Vec3 e = node->AABB_Max - c;
  //t3d_vec3_diff(&e, &node->AABB_Max, &c);

  //translate tri to origin
  Vec3 v0 = verts[0] - c;
  Vec3 v1 = verts[1] - c;
  Vec3 v2 = verts[2] - c;
  //t3d_vec3_diff(&v0, &TriVerts[0], &c);
  //t3d_vec3_diff(&v1, &TriVerts[1], &c);
  //t3d_vec3_diff(&v2, &TriVerts[2], &c);

  //Compute edge vectors of triangle (B-A, C-B, A-C)
  Vec3 f0 = v1 - v0;
  Vec3 f1 = v2 - v1;
  Vec3 f2 = v0 - v2;
  //t3d_vec3_diff(&f0, &v1, &v0);
  //t3d_vec3_diff(&f1, &v2, &v1);
  //t3d_vec3_diff(&f2, &v0, &v2);

  //Compute Face Normals of AABB (always the same since axis-aligned)
  Vec3 u0 = {1.0f, 0.0f, 0.0f};
  Vec3 u1 = {0.0f, 1.0f, 0.0f};
  Vec3 u2 = {0.0f, 0.0f, 1.0f};

  //Compute 9 axis based on cross products of tri edges and AABB edges
  Vec3 axis_u0_f0 = u0.cross(f0);
  //t3d_vec3_cross(&axis_u0_f0, &u0, &f0);
  Vec3 axis_u0_f1 = u0.cross(f1);
  //t3d_vec3_cross(&axis_u0_f1, &u0, &f1);
  Vec3 axis_u0_f2 = u0.cross(f2);
  //t3d_vec3_cross(&axis_u0_f2, &u0, &f2);

  Vec3 axis_u1_f0 = u1.cross(f0);
  //t3d_vec3_cross(&axis_u1_f0, &u1, &f0);
  Vec3 axis_u1_f1 = u1.cross(f1);
  //t3d_vec3_cross(&axis_u1_f1, &u1, &f1);
  Vec3 axis_u1_f2 = u1.cross(f2);
  //t3d_vec3_cross(&axis_u1_f2, &u1, &f2);

  Vec3 axis_u2_f0 = u2.cross(f0);
  //t3d_vec3_cross(&axis_u2_f0, &u2, &f0);
  Vec3 axis_u2_f1 = u2.cross(f1);
  //t3d_vec3_cross(&axis_u2_f1, &u2, &f1);
  Vec3 axis_u2_f2 = u2.cross(f2);
  //t3d_vec3_cross(&axis_u2_f2, &u2, &f2);

  //test all 9 of these axis:
  printf("begin axis tests...\n");
  if(!TestAxisTriAABB(&e, &axis_u0_f0, &v0, &v1, &v2, &u0, &u1, &u2)) return false;
  if(!TestAxisTriAABB(&e, &axis_u0_f1, &v0, &v1, &v2, &u0, &u1, &u2)) return false;
  if(!TestAxisTriAABB(&e, &axis_u0_f2, &v0, &v1, &v2, &u0, &u1, &u2)) return false;

  if(!TestAxisTriAABB(&e, &axis_u1_f0, &v0, &v1, &v2, &u0, &u1, &u2)) return false;
  if(!TestAxisTriAABB(&e, &axis_u1_f1, &v0, &v1, &v2, &u0, &u1, &u2)) return false;
  if(!TestAxisTriAABB(&e, &axis_u1_f2, &v0, &v1, &v2, &u0, &u1, &u2)) return false;

  if(!TestAxisTriAABB(&e, &axis_u2_f0, &v0, &v1, &v2, &u0, &u1, &u2)) return false;
  if(!TestAxisTriAABB(&e, &axis_u2_f1, &v0, &v1, &v2, &u0, &u1, &u2)) return false;
  if(!TestAxisTriAABB(&e, &axis_u2_f2, &v0, &v1, &v2, &u0, &u1, &u2)) return false;

  //perform 3 tests for the 3 AABB face normals
  if(!TestAxisTriAABB(&e, &u0, &v0, &v1, &v2, &u0, &u1, &u2)) return false;
  if(!TestAxisTriAABB(&e, &u1, &v0, &v1, &v2, &u0, &u1, &u2)) return false;
  if(!TestAxisTriAABB(&e, &u2, &v0, &v1, &v2, &u0, &u1, &u2)) return false;

  //perform last test for face normal of the tri
  Vec3 triangleNormal = f0.cross(f1);
  //t3d_vec3_cross(&triangleNormal, &f0, &f1);
  if(!TestAxisTriAABB(&e, &triangleNormal, &v0, &v1, &v2, &u0, &u1, &u2)) return false;
  printf("end axis tests\n");

  //passed test for all 13 seperating axies
  return true;
}


T3DMData parseGLTF(const char *gltfPath, float modelScale)
{
  T3DMData t3dm{};
  fs::path gltfBasePath{gltfPath};
  gltfBasePath = gltfBasePath.parent_path();

  cgltf_options options{};
  cgltf_data* data = nullptr;
  cgltf_result result = cgltf_parse_file(&options, gltfPath, &data);

  if(result == cgltf_result_file_not_found) {
    fprintf(stderr, "Error: File not found! (%s)\n", gltfPath);
    throw std::runtime_error("File not found!");
  }

  if(cgltf_validate(data) != cgltf_result_success) {
    fprintf(stderr, "Invalid glTF data!\n");
    throw std::runtime_error("Invalid glTF data!");
  }

  cgltf_load_buffers(&options, data, gltfPath);

  if(data->asset.generator) {
    std::string metaData(data->asset.generator);
    for(auto &badVer : BAD_VERSIONS) {
      if(metaData.find(badVer) != std::string::npos) {
        std::string err = "\n==============================\n"
          "Error: Blender glTF version not supported: '" + badVer + "'\n"
          "Blender versions after 4.0 have a broken or incorrect GLTF exporter (e.g. wrong vertex colors)\n"
          "For the time being, please use Blender 4.0. A fix *may* arrive in 4.4\n"
          "==============================\n";
        throw std::runtime_error(err);
      }
    }
  }

  // Bones / Armature
  int boneCount = 0;
  int neutralBoneCount = 0;
  for(int i=0; i<data->skins_count; ++i) {
    auto &skin = data->skins[i];
    //printf(" - Skeleton: %s\n", skin.skeleton->name);
    if(skin.joints_count == 0)continue;

    while((boneCount+neutralBoneCount) < skin.joints_count)
    {
      const cgltf_node* bone = skin.joints[boneCount];

      // vertices not assigned to any bone are assigned to an artificial "neutral_bone"
      if(strcmp(bone->name, "neutral_bone") == 0) {
        neutralBoneCount++;
        continue;
      }

      Bone armature = parseBoneTree(bone, nullptr, boneCount);

      //printBoneTree(armature, 0);
      t3dm.skeletons.push_back(armature);
    }
  }

  // Resting pose matrix stack, used to pre-transform vertices
  std::vector<Mat4> matrixStack{};
  std::unordered_map<std::string, const Bone*> boneMap{};
  if(!t3dm.skeletons.empty()) {
    auto addBoneMax = [&](auto&& addBoneMax, const Bone &bone) -> void {
      matrixStack.push_back(bone.inverseBindPose);
      boneMap[bone.name] = &bone;
      for(auto &child : bone.children) {
        addBoneMax(addBoneMax, *child);
      }
    };
    for(const auto &skel : t3dm.skeletons) {
      addBoneMax(addBoneMax, skel);
    }
  }

  // Animations
  //printf("Animations: %d\n", data->animations_count);

  for(int i=0; i<data->animations_count; ++i) {
    auto anim = parseAnimation(data->animations[i], boneMap, config.animSampleRate);
    if(anim.duration < 0.0001f)continue; // ignore empty animations
    convertAnimation(anim, boneMap);
    t3dm.animations.push_back(anim);
  }

  // Meshes
  for(int i=0; i<data->nodes_count; ++i)
  {
    auto node = &data->nodes[i];
    //printf("- Node %d: %s\n", i, node->name);

    if(node->name && std::string(node->name).starts_with("fast64_f3d_material_library")) {
      continue;
    }

    auto mesh = node->mesh;
    if(!mesh)continue;

    // printf(" - Mesh %d: %s\n", i, mesh->name);

    for(int j = 0; j < mesh->primitives_count; j++)
    {
      t3dm.models.push_back({});
      auto &model = t3dm.models.back();
      if(node->name)model.name = node->name;

      auto prim = &mesh->primitives[j];
      //printf("   - Primitive %d:\n", j);

      if(prim->material) {
        parseMaterial(gltfBasePath, i, j, model, prim);
      }

      // find vertex count
      int vertexCount = 0;
      for(int k = 0; k < prim->attributes_count; k++) {
        if(prim->attributes[k].type == cgltf_attribute_type_position) {
          vertexCount = prim->attributes[k].data->count;
          break;
        }
      }

      std::vector<VertexNorm> vertices{};
      vertices.resize(vertexCount, {.color = {1.0f, 1.0f, 1.0f, 1.0f}, .boneIndex = -1});
      std::vector<uint16_t> indices{};

      // Read indices
      if(prim->indices != nullptr)
      {
        auto acc = prim->indices;
        auto basePtr = ((uint8_t*)acc->buffer_view->buffer->data) + acc->buffer_view->offset + acc->offset;
        auto elemSize = Gltf::getDataSize(acc->component_type);

        for(int k = 0; k < acc->count; k++) {
          indices.push_back(Gltf::readAsU32(basePtr, acc->component_type));
          basePtr += elemSize;
        }
      }

      // Read vertices
      for(int k = 0; k < prim->attributes_count; k++)
      {
        auto attr = &prim->attributes[k];
        auto acc = attr->data;
        auto basePtr = ((uint8_t*)acc->buffer_view->buffer->data) + acc->buffer_view->offset + acc->offset;
        auto elemSize = Gltf::getDataSize(acc->component_type);

        //printf("     - Attribute %d: %s\n", k, attr->name);
        if(attr->type == cgltf_attribute_type_position)
        {
          assert(attr->data->type == cgltf_type_vec3);

          for(int l = 0; l < acc->count; l++)
          {
            auto &v = vertices[l];
            v.pos[0] = Gltf::readAsFloat(basePtr, acc->component_type); basePtr += elemSize;
            v.pos[1] = Gltf::readAsFloat(basePtr, acc->component_type); basePtr += elemSize;
            v.pos[2] = Gltf::readAsFloat(basePtr, acc->component_type); basePtr += elemSize;
          }
        }

        if(attr->type == cgltf_attribute_type_color)
        {
          for(int l = 0; l < acc->count; l++)
          {
            auto &v = vertices[l];
            auto color = Gltf::readAsColor(basePtr, attr->data->type, acc->component_type);
            //printf("Color[%s]: %f %f %f %f\n", attr->name, color[0], color[1], color[2], color[3]);

            if(!attr->name || strcmp(attr->name, "COLOR_0") == 0) {
              v.color[0] = color[0];
              v.color[1] = color[1];
              v.color[2] = color[2];
              v.color[3] = color[3];

              // linear to gamma
              for(int c=0; c<3; ++c) {
                v.color[c] = powf(v.color[c], 0.4545f);
              }
            }
          }
        }

        if(attr->type == cgltf_attribute_type_normal)
        {
          assert(attr->data->type == cgltf_type_vec3);

          for(int l = 0; l < acc->count; l++)
          {
            auto &v = vertices[l];
            v.norm[0] = Gltf::readAsFloat(basePtr, acc->component_type); basePtr += elemSize;
            v.norm[1] = Gltf::readAsFloat(basePtr, acc->component_type); basePtr += elemSize;
            v.norm[2] = Gltf::readAsFloat(basePtr, acc->component_type); basePtr += elemSize;
          }
        }

        if(attr->type == cgltf_attribute_type_texcoord)
        {
          assert(attr->data->type == cgltf_type_vec2);

          for(int l = 0; l < acc->count; l++)
          {
            auto &v = vertices[l];
            v.uv[0] = Gltf::readAsFloat(basePtr, acc->component_type); basePtr += elemSize;
            v.uv[1] = Gltf::readAsFloat(basePtr, acc->component_type); basePtr += elemSize;
          }
        }

        if(attr->type == cgltf_attribute_type_joints)
        {
          assert(attr->data->type == cgltf_type_vec4);
          for(int l = 0; l < acc->count; l++)
          {
            auto &v = vertices[l];
            u32 joins[4];
            for(int c=0; c<4; ++c) {
              joins[c] = Gltf::readAsU32(basePtr, acc->component_type); basePtr += elemSize;
            }
            //printf("  - %d %d %d %d\n", joins[0], joins[1], joins[2], joins[3]);
            v.boneIndex = joins[0];
            if(v.boneIndex >= boneCount || v.boneIndex < 0)v.boneIndex = -1;
          }
        }

        if(attr->type == cgltf_attribute_type_weights)
        {
          assert(attr->data->type == cgltf_type_vec4);

          for(int l = 0; l < acc->count; l++)
          {
            auto &v = vertices[l];
            float weights[4];
            for(int c=0; c<4; ++c) {
              weights[c] = Gltf::readAsFloat(basePtr, acc->component_type); basePtr += elemSize;
            }
            //printf("  - %f %f %f %f\n", weights[0], weights[1], weights[2], weights[3]);
          }
        }
      }

      std::vector<VertexT3D> verticesT3D{};
      verticesT3D.resize(vertices.size());

      float texSizeX = model.material.texA.texWidth;
      float texSizeY = model.material.texA.texHeight;

      if(texSizeX == 0)texSizeX = 32;
      if(texSizeY == 0)texSizeY = 32;

      // convert vertices
      for(int k = 0; k < vertices.size(); k++) {
        Mat4 mat = parseNodeMatrix(node);
        convertVertex(
          modelScale, texSizeX, texSizeY, vertices[k], verticesT3D[k],
          mat, matrixStack, model.material.uvFilterAdjust
        );
      }

      // optimizations
      meshopt_optimizeVertexCache(indices.data(), indices.data(), indices.size(), vertices.size());
      //meshopt_optimizeOverdraw(indices.data(), indices.data(), indices.size(), &vertices[0].pos.data[0], vertices.size(), sizeof(VertexNorm), 1.05f);

      // expand into triangles, this is used to split up and dedupe data
      model.triangles.reserve(indices.size() / 3);

      for(int k = 0; k < indices.size(); k += 3) {
        model.triangles.push_back({
          verticesT3D[indices[k + 0]],
          verticesT3D[indices[k + 1]],
          verticesT3D[indices[k + 2]],
        });
      }

      if(config.verbose) {
        printf("[%s] Vertices input: %d\n", mesh->name, vertexCount);
        printf("[%s] Indices input: %d\n", mesh->name, indices.size());
      }
    }
  }

  cgltf_free(data);
  return t3dm;
}
