#include "collision.h"

namespace Collision
{
    int numInstigatorsAcutallyTested = 0;
    int numTriCollisionsChecked = 0;

    void GenerateCollisionFromFile(std::string file_path, Octree* collisionOctree)
    {
        //debugf("Generate file: %s\n", file_path);
        FILE* col_file;
        int size;
        col_file = asset_fopen(file_path.c_str(), &size);

        /********First Read ***********/
        int8_t* header = (int8_t*)malloc(sizeof(char) * 48);
        int h_size = fread(header, 1, sizeof(char) * 48, col_file);
        //file is compressed, must use fread

        uint16_t* id = (uint16_t*)&header[4];
        uint16_t* numverts = (uint16_t*)&header[6];
        float* AABB_Min_0 = (float*)&header[8];
        float* AABB_Min_1 = (float*)&header[12];
        float* AABB_Min_2 = (float*)&header[16];
        float* AABB_Max_0 = (float*)&header[20];
        float* AABB_Max_1 = (float*)&header[24];
        float* AABB_Max_2 = (float*)&header[28];
        uint16_t* children_ids[8];
        children_ids[0] = (uint16_t*)&header[32];
        children_ids[1] = (uint16_t*)&header[34];
        children_ids[2] = (uint16_t*)&header[36];
        children_ids[3] = (uint16_t*)&header[38];
        children_ids[4] = (uint16_t*)&header[40];
        children_ids[5] = (uint16_t*)&header[42];
        children_ids[6] = (uint16_t*)&header[44];
        children_ids[7] = (uint16_t*)&header[46];

        *collisionOctree = {
            .boundingBox = {
                (T3DVec3) { { *AABB_Min_0,* AABB_Min_1,* AABB_Min_2 }
 },
(T3DVec3) { { *AABB_Max_0,* AABB_Max_1,* AABB_Max_2 }
}
},
.children = NULL,
//.vertices = malloc(sizeof(T3DVec3) * (*numverts)),
.numVertices = *numverts
        };

        bool dumbSetupBool = true;

        for (int i = 0; i < 8; i++)
        {
            if (*children_ids[i] != 9999)
            {
                if (dumbSetupBool)
                {
                    collisionOctree->children = (Octree*)malloc(8 * sizeof(Octree));
                    dumbSetupBool = false;
                }
                ReadOctreeFromFile(col_file, &collisionOctree->children[i]);
            }
        }

    }

    void ReadOctreeFromFile(FILE* col_file, Octree* collisionOctree)
    {
        int8_t* header = (int8_t*)malloc(sizeof(char) * 48);
        int h_size = fread(header, 1, sizeof(char) * 48, col_file);
        //file is compressed, must use fread

        uint16_t* id = (uint16_t*)&header[4];
        uint16_t* numverts = (uint16_t*)&header[6];
        float* AABB_Min_0 = (float*)&header[8];
        float* AABB_Min_1 = (float*)&header[12];
        float* AABB_Min_2 = (float*)&header[16];
        float* AABB_Max_0 = (float*)&header[20];
        float* AABB_Max_1 = (float*)&header[24];
        float* AABB_Max_2 = (float*)&header[28];
        uint16_t* children_ids[8];
        children_ids[0] = (uint16_t*)&header[32];
        children_ids[1] = (uint16_t*)&header[34];
        children_ids[2] = (uint16_t*)&header[36];
        children_ids[3] = (uint16_t*)&header[38];
        children_ids[4] = (uint16_t*)&header[40];
        children_ids[5] = (uint16_t*)&header[42];
        children_ids[6] = (uint16_t*)&header[44];
        children_ids[7] = (uint16_t*)&header[46];
        //...keep going...
        float* vert_floats = (float*)malloc(sizeof(float) * (*numverts) * 3);
        int vert_float_size = fread(vert_floats, 1, sizeof(float) * (*numverts) * 3, col_file);

        *collisionOctree = {
          .boundingBox = {
                (T3DVec3) { { *AABB_Min_0,* AABB_Min_1,* AABB_Min_2 }
 },
(T3DVec3) { { *AABB_Max_0,* AABB_Max_1,* AABB_Max_2 }
}
},
.children = NULL,
.vertices = (T3DVec3*)malloc(sizeof(T3DVec3) * (*numverts)),
.numVertices = *numverts
        };
        //debugf("      verts at %d\n", collisionOctree->numVertices);
        bool dumbSetupBool = true;

        for (int i = 0; i < *numverts * 3; i += 3)
        {
            //debugf("      verts at %d =  %f, %f, %f\n", i/3, vert_floats[i], vert_floats[i+1], vert_floats[i+2]);
            collisionOctree->vertices[i / 3].v[0] = vert_floats[i];// + actor->Transform.m[3][0];
            collisionOctree->vertices[i / 3].v[1] = vert_floats[i + 1];// + actor->Transform.m[3][1];
            collisionOctree->vertices[i / 3].v[2] = vert_floats[i + 2];// + actor->Transform.m[3][2]; 
        }

        for (int i = 0; i < 8; i++)
        {
            if (*children_ids[i] != 9999)
            {
                if (dumbSetupBool)
                {
                    collisionOctree->children = (Octree*)malloc(8 * sizeof(Octree));
                    dumbSetupBool = false;
                }
                ReadOctreeFromFile(col_file, &collisionOctree->children[i]);
            }
        }
    }

    /****** Support ****/
    bool TestAABBvsAABB(const T3DVec3* aMin, const T3DVec3* aMax, const T3DVec3* bMin, const T3DVec3* bMax)
    {
        return aMin->v[0] <= bMax->v[0] && aMax->v[0] >= bMin->v[0] &&
            aMin->v[1] <= bMax->v[1] && aMax->v[1] >= bMin->v[1] &&
            aMin->v[2] <= bMax->v[2] && aMax->v[2] >= bMin->v[2];
    }
    bool TestAABBCapsuleTriangle(const T3DVec3* capMin, const T3DVec3* capMax, const T3DVec3* vertices)
    {
        T3DVec3 tri_min;
        T3DVec3 tri_max;

        for (int i = 0; i < 3; i++)
        {
            float min_value = vertices[0].v[i];
            if (vertices[1].v[i] < min_value)
            {
                min_value = vertices[1].v[i];
            }
            if (vertices[2].v[i] < min_value)
            {
                min_value = vertices[2].v[i];
            }
            tri_min.v[i] = min_value;
        }
        for (int i = 0; i < 3; i++)
        {
            float max_value = vertices[0].v[i];
            if (vertices[1].v[i] > max_value)
            {
                max_value = vertices[1].v[i];
            }
            if (vertices[2].v[i] > max_value)
            {
                max_value = vertices[2].v[i];
            }
            tri_max.v[i] = max_value;
        }

        return TestAABBvsAABB(capMin, capMax, &tri_min, &tri_max);
    }

    void ClosestPointOnLineSegment(T3DVec3* res, const T3DVec3* A, const T3DVec3* B, const T3DVec3* Point)
    {
        T3DVec3 AB = *B - *A;
        T3DVec3 PointA = *Point - *A;

        float t;
        if (t3d_vec3_len2(&AB) < 0.001)
        {
            t = 1;
        }
        else
        {
            t = t3d_vec3_dot(&PointA, &AB) / t3d_vec3_len2(&AB);
        }

        float satT = clamp_Float(t, 0, 1);
        scaleVector(&AB, satT);
        *res = *A + AB;
    }

    void scaleVector(T3DVec3* vector, float scale)// May change later...
    {
        vector->v[0] *= scale;
        vector->v[1] *= scale;
        vector->v[2] *= scale;
    }
    float clamp_Float(float value, float min, float max)
    {
        if (value > max)
        {
            value = max;
        }
        if (value < min)
        {
            value = min;
        }
        return value;
    }

    /****** Test Collision *********/
    bool TestAllCollision(std::vector<Node::PhysicsBody3D*>* allPhysicsObjects, float deltaTime)//no instigating node, done for all at once after everything has finished their desired movements
        //list of all collision objects, deltaTime
        //loop through them, if object is physical (not just collidable), test it.
    {
        //debugf("Hello! %d\n", allPhysicsObjects->size());
        if (allPhysicsObjects->size() <= 1) { return false; }
        //debugf("Begin Collision\n");
        bool anyCollision = false;
        int numCollisions = 0;
        
        for (Node::PhysicsBody3D* instigatingBody : *allPhysicsObjects)//!!!!!!!!!!!Worried about this...
        {
            //debugf("++++Collision 1\n");
            if (instigatingBody->isStatic) { continue; }
            /*Node::PhysicsBody3D* instigatingBody = dynamic_cast<Node::PhysicsBody3D*>(instigatingObject);//character, rigid, or animatable Visitor? Maybe just check it's NOT static?
            if (!instigatingBody)//is object not physics based?
            {
                continue;
            }*///!!!!!!!!!!!!!!!!!!!!!!! Check for statics/types that don't initiate. Also, BVH or something?
            numInstigatorsAcutallyTested++;
            for (Node::PhysicsBody3D* otherObject : *allPhysicsObjects)//!!!!!!!!!!!Worried about this...
            {
                if (instigatingBody->isDead) return anyCollision;
                if (otherObject->isDead) continue;
                if (otherObject->disableCollider || instigatingBody->disableCollider) continue;

                if (instigatingBody->playerCollider && otherObject->bulletCollider && !otherObject->pickupCollider)// || otherObject->targetCollider))
                {
                    continue;
                }
                else if (instigatingBody->bulletCollider && otherObject->playerCollider)
                {
                    continue;
                }
                else if (instigatingBody->explodingCollider && (otherObject->playerCollider || otherObject->bulletCollider || otherObject->pickupCollider || otherObject->targetCollider))
                {
                    continue;
                }
                //debugf("++++++Collision 2\n");
                numCollisions = 0;
                //for (int i = 0; i < 4; i++)//MUST CHANGE TO ACTUAL ARRAY LENGTH
                //{
                while ((otherObject != instigatingBody) && TestCollision(instigatingBody, otherObject, deltaTime) && numCollisions <= 3) //&& anyCollision == false)
                {
                    //debugf("++++++++Collision 3\n");
                    anyCollision = true;//Now that this func does ALL collisions for real, this may be less/differently relevant
                    numCollisions++;
                    /*if (numCollisions >= 3)
                    {
                        return anyCollision;
                    }
                    else
                    {
                        i = -1;
                    }*/
                }
                //}
            }
        }
        //debugf("End test all collision, num physics objects: %d\n", allPhysicsObjects->size());
        //debugf("Num Instigators: %d\n", numInstigatorsAcutallyTested);
        //debugf("Num tris tested: %d\n", numTriCollisionsChecked);
        numInstigatorsAcutallyTested = 0;
        numTriCollisionsChecked = 0;
        return anyCollision;
    }
    bool TestCollision(Node::PhysicsBody3D* instigatorObject, Node::PhysicsBody3D* otherObject, float deltaTime)
    {
        struct Collision::AABB instigatorAABB = instigatorObject->getGlobalAABB();
        struct Collision::AABB otherAABB = otherObject->getGlobalAABB();
        if (!TestAABBvsAABB(&instigatorAABB.min, &instigatorAABB.max, &otherAABB.min, &otherAABB.max))
        {
            return false;
        }
        //debugf("------Collision A\n");
        //! Is there multiple child shapes? then check each one!

        //! Else, just check the one v one.
        Node::CollisionShape* instigatorShape = instigatorObject->GetShapeChildren()[0];
        Node::CollisionShape* otherShape = otherObject->GetShapeChildren()[0];
        T3DVec3 penetration_normal;
        float penetration_depth;
        if (SpecificCollisionTest(instigatorShape, otherShape, deltaTime, &penetration_normal, &penetration_depth))
        {
            //debugf("------Collision Z\n");
            instigatorObject->respondToCollision(&penetration_normal, penetration_depth, instigatorShape, otherShape, deltaTime);//need other object too?
            return true;
        }
        else
        {
            return false;
        }

    }

    bool SpecificCollisionTest(Node::CollisionShape* instigatorShape, Node::CollisionShape* otherShape, float deltaTime, T3DVec3* penetration_normal, float* penetration_depth)
    {
        //debugf("Begin SPECIFIC Test Collision:\n");
        if (instigatorShape->getType() == Collision::CAPSULE)
        {
            switch (otherShape->getType())
            {
                case Collision::CAPSULE:
                    return TestCapsuleCapsuleCollision(instigatorShape, otherShape, deltaTime, penetration_normal, penetration_depth);
                break;
                case Collision::MESH:
                    //debugf("Test cap - mesh !@#@! \n");
                    return TestCapsuleMeshCollision(instigatorShape, otherShape, deltaTime, penetration_normal, penetration_depth);
                break;
                case Collision::BOX:

                break;
                case Collision::SPHERE:
                    return TestCapsuleSphereCollision(instigatorShape, otherShape, deltaTime, penetration_normal, penetration_depth);
                break;
            }
        }
        else if (instigatorShape->getType() == Collision::SPHERE)
        {
            switch (otherShape->getType())
            {
            case Collision::CAPSULE:
                return TestCapsuleSphereCollision(otherShape, instigatorShape, deltaTime, penetration_normal, penetration_depth);
                break;
            case Collision::MESH:
                return TestSphereMeshCollision(instigatorShape, otherShape, deltaTime, penetration_normal, penetration_depth);
                break;
            case Collision::BOX:

                break;
            case Collision::SPHERE:
                    return TestSphereSphereCollision(instigatorShape, otherShape, deltaTime, penetration_normal, penetration_depth);
                break;
            }
        }
        return false;
    }

    /*Broad Test*/
    bool TestCapsuleMeshCollision(Node::CollisionShape* instigatorShape, Node::CollisionShape* otherShape, float deltaTime, T3DVec3* penetration_normal, float* penetration_depth)//Must be made more specific shape!
    {
        Resource::CapsuleShape3D* instigatingCapsule = static_cast<Resource::CapsuleShape3D*>(instigatorShape->getShape3D());
        Resource::MeshShape3D* otherMesh = static_cast<Resource::MeshShape3D*>(otherShape->getShape3D());
        //cast otherShape to MeshShape3D
        //get octree from it

        if (TraverseOctreeCapsule(otherMesh->octree, instigatingCapsule, instigatorShape, deltaTime, penetration_normal, penetration_depth))
        {
            //instigatorShape->getParent()->respondToCollision(penetration_normal, penetration_depth, instigatorShape, otherShape, deltaTime);
            //RespondCollideNSlide();
            return true;
        }
        return false;
    }
    bool TestSphereMeshCollision(Node::CollisionShape* instigatorShape, Node::CollisionShape* otherShape, float deltaTime, T3DVec3* penetration_normal, float* penetration_depth)
    {
        //debugf("Begin Sphere Mesh Collision Collision:\n");
        Resource::SphereShape3D* instigatingSphere = static_cast<Resource::SphereShape3D*>(instigatorShape->getShape3D());
        Resource::MeshShape3D* otherMesh = static_cast<Resource::MeshShape3D*>(otherShape->getShape3D());
        //cast otherShape to MeshShape3D
        //get octree from it

        if (TraverseOctreeSphere(otherMesh->octree, instigatingSphere, instigatorShape, deltaTime, penetration_normal, penetration_depth))
        {
            //instigatorShape->getParent()->respondToCollision(penetration_normal, penetration_depth, instigatorShape, otherShape, deltaTime);
            //RespondCollideNSlide();
            return true;
        }
        return false;
    }
    bool TestCapsuleCapsuleCollision(Node::CollisionShape* instigatorShape, Node::CollisionShape* otherShape, float deltaTime, T3DVec3* penetration_normal, float* penetration_depth)
    {
        //debugf("------Collision C\n");
        Resource::CapsuleShape3D* instigatingCapsule = static_cast<Resource::CapsuleShape3D*>(instigatorShape->getShape3D());
        Resource::CapsuleShape3D* otherCapsule = static_cast<Resource::CapsuleShape3D*>(otherShape->getShape3D());

        T3DVec3 instCapA = instigatorShape->getGlobalPosition();// -instigatingCapsule->getMidHeight() / 2;// = 0.0f; !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        instCapA.v[1] -= instigatingCapsule->getMidHeight() / 2;        // no rotation or scaling yet!!! Only vertical capsules!
        T3DVec3 instCapB = instigatorShape->getGlobalPosition();// +instigatingCapsule->getMidHeight() / 2; = 0.0f;
        instCapB.v[1] += instigatingCapsule->getMidHeight() / 2;

        T3DVec3 otherCapA = otherShape->getGlobalPosition();// -otherCapsule->getMidHeight() / 2; = 0.0f;
        otherCapA.v[1] -= otherCapsule->getMidHeight() / 2;
        T3DVec3 otherCapB = otherShape->getGlobalPosition();// +otherCapsule->getMidHeight() / 2; = 0.0f;
        otherCapA.v[1] += otherCapsule->getMidHeight() / 2;

        //vec btwn end points
        T3DVec3 v0 = otherCapA - instCapA;
        T3DVec3 v1 = otherCapB - instCapA;
        T3DVec3 v2 = otherCapA - instCapB;
        T3DVec3 v3 = otherCapB - instCapB;

        //squared dist
        float d0 = t3d_vec3_dot(v0, v0);
        float d1 = t3d_vec3_dot(v1, v1);
        float d2 = t3d_vec3_dot(v2, v2);
        float d3 = t3d_vec3_dot(v3, v3);

        T3DVec3 bestA;
        if (d2 < d0 || d2 < d1 || d3 < d0 || d3 < d1)
        {
            bestA = instCapB;
        }
        else
        {
            bestA = instCapA;
        }

        //best point on otherCap line segment nearest to best potential endpoint on instCap capsule
        T3DVec3 bestB;
        ClosestPointOnLineSegment(&bestB, &otherCapA, &otherCapB, &bestA);
        //and then the same for instCap line segment
        ClosestPointOnLineSegment(&bestA, &instCapA, &instCapB, &bestB);


        *penetration_normal = bestA - bestB;
        float len = t3d_vec3_len(*penetration_normal);
        *penetration_normal /= len;
        *penetration_depth = instigatingCapsule->getRadius() + otherCapsule->getRadius() - len;
        bool intersects = *penetration_depth > 0;

        if (intersects)
        {
            //debugf("------Collision D\n");
            //RespondCollideNSlide();
            //instigatorShape->getParent()->respondToCollision(penetration_normal, penetration_depth, instigatorShape, otherShape, deltaTime);
            return true;
        }
        //debugf("------ No Collision, %f\n", *penetration_depth);
        return false;
    }
    bool TestCapsuleSphereCollision(Node::CollisionShape* instigatorShape, Node::CollisionShape* otherShape, float deltaTime, T3DVec3* penetration_normal, float* penetration_depth)
    {
        Resource::CapsuleShape3D* instigatingCapsule = static_cast<Resource::CapsuleShape3D*>(instigatorShape->getShape3D());
        Resource::SphereShape3D* otherSphere = static_cast<Resource::SphereShape3D*>(otherShape->getShape3D());

        T3DVec3 CapA = instigatorShape->getGlobalPosition();// -instigatingCapsule->getMidHeight() / 2;// = 0.0f; !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        CapA.v[1] -= instigatingCapsule->getMidHeight() / 2;        // no rotation or scaling yet!!! Only vertical capsules!
        T3DVec3 CapB = instigatorShape->getGlobalPosition();// +instigatingCapsule->getMidHeight() / 2; = 0.0f;
        CapB.v[1] += instigatingCapsule->getMidHeight() / 2;

        T3DVec3 spherePos = otherShape->getGlobalPosition();
        T3DVec3 bestInstSphere;
        ClosestPointOnLineSegment(&bestInstSphere, &CapA, &CapB, &spherePos);


        *penetration_normal = bestInstSphere - otherShape->getGlobalPosition();
        float len = t3d_vec3_len(*penetration_normal);
        *penetration_normal /= len;
        *penetration_depth = instigatingCapsule->getRadius() + otherSphere->getRadius() - len;
        bool intersects = *penetration_depth > 0;

        if (intersects)
        {
            //instigatorShape->getParent()->respondToCollision(penetration_normal, penetration_depth, instigatorShape, otherShape, deltaTime);
            //RespondCollideNSlide(penetration_normal, penetration_depth);
            return true;
        }
        
        return false;
    }
    bool TestSphereSphereCollision(Node::CollisionShape* instigatorShape, Node::CollisionShape* otherShape, float deltaTime, T3DVec3* penetration_normal, float* penetration_depth)
    {
        //debugf("------Collision Shpee\n");
        Resource::SphereShape3D* instigatingSphere = static_cast<Resource::SphereShape3D*>(instigatorShape->getShape3D());
        Resource::SphereShape3D* otherSphere = static_cast<Resource::SphereShape3D*>(otherShape->getShape3D());
        //debugf("^^^^^^^ %f, % f, % f\n", instigatorShape->getGlobalPosition().v[0], instigatorShape->getGlobalPosition().v[1], instigatorShape->getGlobalPosition().v[2]);
        //debugf("^^^^^^^ %f, %f, %f\n", otherShape->getGlobalPosition().v[0], otherShape->getGlobalPosition().v[1], otherShape->getGlobalPosition().v[2]);
        *penetration_normal = instigatorShape->getGlobalPosition() - otherShape->getGlobalPosition();
     ////////*penetration_normal = (T3DVec3){0.f, 0.f, 10000000.f};
        //debugf("^^^B4^^ %f, %f, %f\n", penetration_normal->v[0], penetration_normal->v[1], penetration_normal->v[2]);
        float len = t3d_vec3_len(*penetration_normal);
        if (len == 0)
        {
            *penetration_normal *= 0.f;
        }
        else
        {
            *penetration_normal /= len;
        }
        
        //debugf("^^^AF^^ %f, %f, %f\n", penetration_normal->v[0], penetration_normal->v[1], penetration_normal->v[2]);
        //debugf("------ rad1+rad2-len, %f, %f, %f\n", instigatingSphere->getRadius(), otherSphere->getRadius(), len);
        *penetration_depth = instigatingSphere->getRadius() + otherSphere->getRadius() - len;
        bool intersects = *penetration_depth > 0;

        if (intersects)
        {
            //debugf("------Collision D=shpee\n");
            //instigatorShape->getParent()->respondToCollision(penetration_normal, penetration_depth, instigatorShape, otherShape, deltaTime);
            //RespondCollideNSlide(penetration_normal, penetration_depth);
            return true;
        }
        //debugf("------ No shpee Collision, %f\n", *penetration_depth);
        return false;
    }

    /*Attempt Collide*/
    bool CollideCapsuleMeshOctree()
    {
        return false;
    }
    bool CollideCapsuleSphereOctree()
    {
        return false;
    }

    /*Octree Traversal*/
    bool TraverseOctreeCapsule(Octree* node, Resource::CapsuleShape3D* capsule, Node::CollisionShape* instigatorShape, float deltaTime, T3DVec3* penetration_normal, float* penetration_depth)//Can we make this general???
    {
        //debugf("traverse Mesh +++++++++++++++\n");
        //if (*ColHappened) return true; This necessary?
        if (node->children == NULL) return false;

        for (int i = 0; i < 8; i++)
        {
            //debugf("++ traverse %d +++++++++++++++\n", i);
            Collision::AABB capAABB = instigatorShape->calcAABB();
            //debugf("AABB Min: %f %f %f\n", capAABB.min.v[0], capAABB.min.v[1], capAABB.min.v[2]);
            //debugf("AABB Max: %f %f %f\n", capAABB.max.v[0], capAABB.max.v[1], capAABB.max.v[2]);
            if (TestAABBvsAABB(&capAABB.min, &capAABB.max, &node->children[i].boundingBox.min, &node->children[i].boundingBox.max))
            {
                if (node->children[i].children != NULL)
                {
                    //debugf("++++ Deeper Traverse\n");
                    if (TraverseOctreeCapsule(&node->children[i], capsule, instigatorShape, deltaTime, penetration_normal, penetration_depth))
                    {
                        return true;
                    }
                }
                else
                {
                    for (int j = 0; j < node->children[i].numVertices; j += 3)
                    {
                        //debugf("++++++ test verts at %d\n", j);
                        if (CollideCapsuleTriangle(&node->children[i].vertices[j], capsule, instigatorShape, penetration_normal, penetration_depth))
                        {
                            return true;
                        }
                    }
                }
            }
        }



        return false;
    }

    bool TraverseOctreeSphere(Octree* node, Resource::SphereShape3D* sphere, Node::CollisionShape* instigatorShape, float deltaTime, T3DVec3* penetration_normal, float* penetration_depth)
    {
        if (node->children == NULL) return false;
        //debugf("Traverse Octree begin sphere:\n");
        //debugf("++++++++++++ center sphere? %f %f %f\n", instigatorShape->getGlobalPosition().v[0], instigatorShape->getGlobalPosition().v[1], instigatorShape->getGlobalPosition().v[2]);
        for (int i = 0; i < 8; i++)
        {
            //debugf("sphere ++ traverse %d +++++++++++++++\n", i);
            Collision::AABB capAABB = instigatorShape->calcAABB();
            //debugf("AABB Min: %f %f %f\n", capAABB.min.v[0], capAABB.min.v[1], capAABB.min.v[2]);
            //debugf("AABB Max: %f %f %f\n", capAABB.max.v[0], capAABB.max.v[1], capAABB.max.v[2]);
            if (TestAABBvsAABB(&capAABB.min, &capAABB.max, &node->children[i].boundingBox.min, &node->children[i].boundingBox.max))
            {
                if (node->children[i].children != NULL)
                {
                    //debugf("sphere ++++ Deeper Traverse\n");
                    if (TraverseOctreeSphere(&node->children[i], sphere, instigatorShape, deltaTime, penetration_normal, penetration_depth))
                    {
                        return true;
                    }
                }
                else
                {
                    for (int j = 0; j < node->children[i].numVertices; j += 3)
                    {
                        Collision::AABB sphereAABB = instigatorShape->calcAABB();
                        if (!TestAABBCapsuleTriangle(&sphereAABB.min, &sphereAABB.max, &node->children[i].vertices[j]))
                        {
                            continue;
                        }
                        numTriCollisionsChecked++;
                        if (CollideSphereTriangle(&node->children[i].vertices[j], instigatorShape->getGlobalPosition(), sphere->getRadius(), instigatorShape, penetration_normal, penetration_depth))
                        {
                            return true;
                        }
                    }
                }
            }
        }
        return false;
    }

    /*Primitive Collision Tests*/
    bool CollideCapsuleTriangle(T3DVec3* vertices, Resource::CapsuleShape3D* capsule, Node::CollisionShape* instigatorShape, T3DVec3* penetration_normal, float* penetration_depth)
    {
        Collision::AABB capAABB = instigatorShape->calcAABB();
        if (!TestAABBCapsuleTriangle(&capAABB.min, &capAABB.max, vertices))
        {
            return false;
        }
        numTriCollisionsChecked++;
        T3DVec3 instCapA = instigatorShape->getGlobalPosition();// -instigatingCapsule->getMidHeight() / 2;// = 0.0f; !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        T3DVec3 instCapBase = instCapA;
        instCapA.v[1] -= capsule->getMidHeight() / 2;        // no rotation or scaling yet!!! Only vertical capsules!
        instCapBase.v[1] -= capsule->getHeight() / 2;        // no rotation or scaling yet!!! Only vertical capsules!

        T3DVec3 instCapB = instigatorShape->getGlobalPosition();// +instigatingCapsule->getMidHeight() / 2; = 0.0f;
        T3DVec3 instCapTip = instCapB;
        instCapB.v[1] += capsule->getMidHeight() / 2;
        instCapTip.v[1] += capsule->getHeight() / 2;

        T3DVec3 capsuleNormal = instCapB - instCapA;

        T3DVec3 v1v0 = vertices[1] - vertices[0];
        T3DVec3 v2v1 = vertices[2] - vertices[1];
        T3DVec3 v0v2 = vertices[0] - vertices[2];
        T3DVec3 v2v0 = vertices[2] - vertices[0];
        // Calc t
        T3DVec3 reference_point;

        T3DVec3 planeNormal;
        t3d_vec3_cross(&planeNormal, &v1v0, &v2v0);
        t3d_vec3_norm(&planeNormal);
        float dotncap = fabs(t3d_vec3_dot(planeNormal, capsuleNormal));

        if (dotncap > 0.0001f) {
            float t = t3d_vec3_dot(planeNormal, (vertices[0] - instCapBase)) / dotncap;
            T3DVec3 line_plane_intersection = instCapBase + capsuleNormal * t;
                // find closest point on triangle to line_plane_intersection, same as sphere-tri case
            //instead of the sphere centre projection, we use the line_plane_intersection point
            //Determine whether line_plane_intersection is inside ALL triangle edges

            T3DVec3 c0;
            T3DVec3 L0v0 = line_plane_intersection - vertices[0];

            //t3d_vec3_diff(&L0v0, &line_plane_intersection, &vertices[0]);
            ////t3d_vec3_diff(&v1v0, &vertices[1], &vertices[0]);
            t3d_vec3_cross(&c0, &L0v0, &v1v0);

            T3DVec3 c1;
            T3DVec3 L0v1 = line_plane_intersection - vertices[1];

            //t3d_vec3_diff(&L0v1, &line_plane_intersection, &vertices[1]);
            //t3d_vec3_diff(&v2v1, &vertices[2], &vertices[1]);
            t3d_vec3_cross(&c1, &L0v1, &v2v1);

            T3DVec3 c2;
            T3DVec3 L0v2 = line_plane_intersection - vertices[2];

            //t3d_vec3_diff(&L0v2, &line_plane_intersection, &vertices[2]);
            //t3d_vec3_diff(&v0v2, &vertices[0], &vertices[2]);
            t3d_vec3_cross(&c2, &L0v2, &v0v2);

            bool inside = (t3d_vec3_dot(&c0, &planeNormal) <= 0.f && t3d_vec3_dot(&c1, &planeNormal) <= 0.f && t3d_vec3_dot(&c2, &planeNormal) <= 0.f);

            if (inside)
            {
                reference_point = line_plane_intersection;
            }
            else
            {
                //Edge 1:
                T3DVec3 point1;
                ClosestPointOnLineSegment(&point1, &vertices[0], &vertices[1], &line_plane_intersection);
                T3DVec3 E1 = line_plane_intersection - point1;
                float distsq = t3d_vec3_len2(&E1);
                float best_dist = distsq;
                reference_point = point1;

                //Edge 2:
                T3DVec3 point2;
                ClosestPointOnLineSegment(&point2, &vertices[1], &vertices[2], &line_plane_intersection);
                T3DVec3 E2 = line_plane_intersection - point2;
                distsq = t3d_vec3_len2(&E2);
                if (distsq < best_dist)
                {
                    reference_point = point2;
                    best_dist = distsq;
                }

                //Edge 3:
                T3DVec3 point3;
                ClosestPointOnLineSegment(&point3, &vertices[2], &vertices[0], &line_plane_intersection);
                T3DVec3 E3 = line_plane_intersection - point3;
                distsq = t3d_vec3_len2(&E3);
                if (distsq < best_dist)
                {
                    reference_point = point3;
                    best_dist = distsq;
                }
            }
        }
        else
        {
            reference_point = vertices[0];
        }

        //finally calc center
        T3DVec3 center;
        ClosestPointOnLineSegment(&center, &instCapA, &instCapB, &reference_point);
        //Perform sphere-tri collision using capsult's radius and this center as it's position

        return CollideSphereTriangle(vertices, center, capsule->getRadius(), instigatorShape, penetration_normal, penetration_depth);//collidespheretri
    }
    bool CollideSphereTriangle(T3DVec3* vertices, T3DVec3 center, float radius, Node::CollisionShape* instigatorShape, T3DVec3* penetration_normal, float* penetration_depth)
    {
        T3DVec3 v1v0 = vertices[1] - vertices[0];
        T3DVec3 v2v1 = vertices[2] - vertices[1];
        T3DVec3 v0v2 = vertices[0] - vertices[2];
        T3DVec3 v2v0 = vertices[2] - vertices[0];

        T3DVec3 planeNormal;
        t3d_vec3_cross(&planeNormal, &v1v0, &v2v0);
        t3d_vec3_norm(&planeNormal);

        float dist = t3d_vec3_dot(center - vertices[0], planeNormal);

        if (fabs(dist) > radius)
        {
            return false;
        }

        T3DVec3 point0 = center - planeNormal * dist;

        T3DVec3 c0;
        T3DVec3 L0v0 = point0 - vertices[0];
        t3d_vec3_cross(&c0, &L0v0, &v1v0);

        T3DVec3 c1;
        T3DVec3 L0v1 = point0 - vertices[1];
        t3d_vec3_cross(&c1, &L0v1, &v2v1);

        T3DVec3 c2;
        T3DVec3 L0v2 = point0 - vertices[2];
        t3d_vec3_cross(&c2, &L0v2, &v0v2);

        bool inside = (t3d_vec3_dot(&c0, &planeNormal) <= 0.f && t3d_vec3_dot(&c1, &planeNormal) <= 0.f && t3d_vec3_dot(&c2, &planeNormal) <= 0.f);

        float rad2 = radius * radius;

        T3DVec3 best_point;
        T3DVec3 intersection_vec;

        if (inside)
        {
            intersection_vec = center - point0;
        }
        else
        {
            //Edge 1:
            T3DVec3 point1;
            ClosestPointOnLineSegment(&point1, &vertices[0], &vertices[1], &center);
            T3DVec3 E1 = center - point1;
            float distsq1 = t3d_vec3_len2(&E1);
            bool intersects = distsq1 < rad2;

            //Edge 2:
            T3DVec3 point2;
            ClosestPointOnLineSegment(&point2, &vertices[1], &vertices[2], &center);
            T3DVec3 E2 = center - point2;
            float distsq2 = t3d_vec3_len2(&E2);
            intersects |= distsq2 < rad2;

            //Edge 3:
            T3DVec3 point3;
            ClosestPointOnLineSegment(&point3, &vertices[2], &vertices[0], &center);
            T3DVec3 E3 = center - point3;
            float distsq3 = t3d_vec3_len2(&E3);
            intersects |= distsq3 < rad2;

            if (intersects)
            {
                T3DVec3 d = center - point1;
                float best_distsq = t3d_vec3_len2(d);
                best_point = point1;
                intersection_vec = d;

                d = center - point2;
                float distsq = t3d_vec3_len2(d);
                if (distsq < best_distsq)
                {
                    best_distsq = distsq;
                    best_point = point2;
                    intersection_vec = d;
                }

                d = center - point3;
                distsq = t3d_vec3_len2(d);
                if (distsq < best_distsq)
                {
                    best_distsq = distsq;
                    best_point = point3;
                    intersection_vec = d;
                }
            }
            else
            {
                return false;
            }
        }

        float len = t3d_vec3_len(&intersection_vec);
        *penetration_normal = intersection_vec / len;
        *penetration_depth = radius - len;
        return true;
    }
    bool CollideSphereSphere()
    {
        //need transform
        //find distance between sphere, if it is less than the sum of their two radiuses, they are intersecting 
        return false;
    }
    bool CollideCapsuleSphere()// not implemented yet
    {
        return false;
    }
    bool CollideCapsuleCapsule(Resource::CapsuleShape3D* instigatingCapsule, Resource::CapsuleShape3D* otherCapsule)// I think this works? Check.
    {
        //need transform! tranform exists at exact midpoint of capsule.
        ////T3DVec3 c1_normal = instigatingCapsule->
            // Tranform UP vector
        return false;
    }

    /*Respond to Collision*/
    /*void RespondCollideNSlide(T3DVec3 penetration_normal, float penetration_depth)
    {
        float velocity_length = t3d_vec3_len(velocity);
        if (velocity_length == 0.f)//This may be wrong ...
        {
            velocity_normalized.v[0] = 0;
            velocity_normalized.v[1] = 1;
            velocity_normalized.v[2] = 0;
        }
        else
        {
            T3DVec3 velocity_normalized = CapsuleActor->CurrentVelocity / velocity_length;
        }

        T3DVec3 undesired_motion = penetration_normal * t3d_vec3_dot(&velocity_normalized, &penetration_normal);
        T3DVec3 desired_motion = velocity_normalized - undesired_motion;
        velocity = desired_motion * velocity_length;
        //remove penetration, directly using local position seems wrong...
        position += penetration_normal * (penetration_depth + 0.0001f);
        //do we apply velocity here? did before but should we?

    }*/


}