/**
* @copyright 2023 - Max Bebök
* @license MIT
*/
#include <stdio.h>
#include <string>
#include <filesystem>
#include <algorithm>
#include <cassert>

#include "structs.h"
#include "parser.h"
#include "hash.h"
#include "args.h"

#include "binaryFile.h"
#include "converter/converter.h"
#include "parser/rdp.h"
#include "optimizer/optimizer.h"

Config config;

	typedef struct Octree{
	    int16_t AABB_Min[3];
	    int16_t AABB_Max[3];
	    uint16_t numTris;// (16 bit should be enough)
	    uint16_t id;
	    std::vector<TriangleT3D> tris;
	    std::vector<Octree> children;// (actual child nodes!)
} Octree;

namespace fs = std::filesystem;

namespace {
  uint32_t insertString(std::string &stringTable, const std::string &newString) {
    auto strPos = stringTable.find(newString);
    if(strPos == std::string::npos) {
      strPos = stringTable.size();
      stringTable += newString;
      stringTable.push_back('\0');
    }
    return strPos;
  }

  int writeBone(BinaryFile &file, const Bone &bone, std::string &stringTable, int level) {
    //printf("Bone[%d]: %s -> %d\n", bone.index, bone.name.c_str(), bone.parentIndex);

    file.write(insertString(stringTable, bone.name));
    file.write<uint16_t>(bone.parentIndex);
    file.write<uint16_t>(level); // level

    auto normPos = bone.pos * config.globalScale;
    file.writeArray(bone.scale.data, 3);
    file.writeArray(bone.rot.data, 4);
    file.writeArray(normPos.data, 3);

    int boneCount = 1;
    for(const auto& child : bone.children) {
      boneCount += writeBone(file, *child, stringTable, level+1);
    }
    return boneCount;
  };

  std::string getRomPath(const std::string &path) {
    if(path.find("filesystem/") == 0) {
      return std::string("rom:/") + path.substr(11);
    }
    return path;
  }

  std::string getStreamDataPath(const char* filePath, uint32_t idx) {
    auto sdataPath = std::string(filePath).substr(0, std::string(filePath).size()-5);
    std::replace(sdataPath.begin(), sdataPath.end(), '\\', '/');
    return sdataPath + "." + std::to_string(idx) + ".sdata";
  }
}





int main(int argc, char* argv[])
{
    EnvArgs args{argc, argv};
  if(args.checkArg("--help")) {
    printf("Usage: %s <gltf-file> <t3dm-file> [--bvh] [--base-scale=64] [--ignore-materials] [--verbose]\n", argv[0]);
    return 1;
  }
  
  const std::string gltfPath = args.getFilenameArg(0);
  std::string t3dmPath = args.getFilenameArg(1);
  std::string replacement = ".col";
  std::string toReplace = ".t3dm";
  size_t pos = t3dmPath.find(toReplace);
  t3dmPath.replace(pos, toReplace.size(), replacement);

  printf("gltfPath: %s & t3dmPath%s\n", gltfPath.c_str(), t3dmPath.c_str());

  auto allModels = parseGLTFCustom(gltfPath.c_str(), t3dmPath.c_str(),config.globalScale);
  printf("back in main...\n");
  fs::path gltfBasePath{gltfPath};
  
  uint16_t totalTriCount = 0;
  uint16_t totalVertexCount = 0;
  uint16_t id = 0;
  int16_t AABB_Max[3] = (int16_t[3]){0, 0, 0};
  int16_t AABB_Min[3] = (int16_t[3]){0, 0, 0};
printf("main 0.\n");
 //// BinaryFile file{};
////  file.writeChars("COL", 3);
  //file.write(chunkCount);
////  file.write<uint8_t>(42);//magic
  /*file.write<float>(1);//AABB_Min, T3DVec3 (set later)
  file.write<float>(2);//AABB_Min, T3DVec3 (set later)
  file.write<float>(3);//AABB_Min, T3DVec3 (set later)
  file.write<float>(1);//AABB_Max, T3DVec3 (set later)
  file.write<float>(2);//AABB_Max, T3DVec3 (set later)
  file.write<float>(3);//AABB_Max, T3DVec3 (set later)*/
////  file.write<uint16_t>(69); // total vertex count (set later)
////  file.write<uint16_t>(420);//indices (remove later)
  /*file.write<uint16_t>(69); // id (set later)*/
  //Write Verticies
  printf("main 1.\n");
  //WARNING! this only takes pos fixed point, doesn't include s and t pixel coords. honestly, I don't know if that matters, but it might be why everything is so shakey...
  AABB_Max[0] = allModels[0].triangles[0].vert[0].pos[0];//this copies just the pos, do we want full VertexT3D?
  AABB_Max[1] = allModels[0].triangles[0].vert[0].pos[1];
  AABB_Max[2] = allModels[0].triangles[0].vert[0].pos[2];
  AABB_Min[0] = allModels[0].triangles[0].vert[0].pos[0];//lots more data, is it worth it? needed for collision?
  AABB_Min[1] = allModels[0].triangles[0].vert[0].pos[1];
  AABB_Min[2] = allModels[0].triangles[0].vert[0].pos[2];
  for (int i = 0; i < allModels[0].triangles.size(); i++)//only works if one model...
  {
    //find AABB min/max
        for (int j = 0; j < 3; j++){
            if (allModels[0].triangles[i].vert[0].pos[j] < AABB_Min[j])
            {
                AABB_Min[j] = allModels[0].triangles[i].vert[0].pos[j];
            }
            if (allModels[0].triangles[i].vert[1].pos[j] < AABB_Min[j])
            {
                AABB_Min[j] = allModels[0].triangles[i].vert[1].pos[j];
            }
            if (allModels[0].triangles[i].vert[2].pos[j] < AABB_Min[j])
            {
                AABB_Min[j] = allModels[0].triangles[i].vert[2].pos[j];
            }
        }
        for (int j = 0; j < 3; j++){
            if (allModels[0].triangles[i].vert[0].pos[j] > AABB_Max[j])
            {
                AABB_Max[j] = allModels[0].triangles[i].vert[0].pos[j];
            }
            if (allModels[0].triangles[i].vert[1].pos[j] > AABB_Max[j])
            {
                AABB_Max[j] = allModels[0].triangles[i].vert[1].pos[j];
            }
            if (allModels[0].triangles[i].vert[2].pos[j] > AABB_Max[j])
            {
                AABB_Max[j] = allModels[0].triangles[i].vert[2].pos[j];
            }
        }
  }
printf("main 2.\n");
  struct Octree CollisionOctree = {
    .AABB_Min = {AABB_Min[0], AABB_Min[1], AABB_Min[2]},
    .AABB_Max = {AABB_Max[0], AABB_Max[1], AABB_Max[2]},
    .numTris = allModels[0].triangles.size(),// (16 bit should be enough)
    .id = 0,
    .tris = allModels[0].triangles,
    //.children;// 
  };
printf("main 3.\n");
  //split octree (recursive function)
  //SplitOctree(4, &CollisionOctree);
  printf("main 4.\n");
  //Write child ids






  /*file.write(allModels[0].triangles[0].vert[0].pos[0]);
  file.write(allModels[0].triangles[0].vert[0].pos[1]);
  file.write(allModels[0].triangles[0].vert[0].pos[2]);*/
/******** for (int i = 0; i < allModels[0].triangles.size(); i++)
  {
    for (int j = 0; j < 3; j++)
    {
      file.write(allModels[0].triangles[i].vert[j].pos[0]);
      file.write(allModels[0].triangles[i].vert[j].pos[1]);
      file.write(allModels[0].triangles[i].vert[j].pos[2]);
      
    }
    totalTriCount++;
  }
  printf("middle main\n");

  file.setPos(0x04);
  file.write(totalTriCount);

  file.writeToFile(t3dmPath.c_str());
  printf("main end\n");**********/
}