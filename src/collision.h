#pragma once
#include <libdragon.h>
#include <t3d/t3d.h>
#include <string>
#include <vector>
#include "Node.h"
#include "CollisionObject3D.h"
#include "PhysicsBody3D.h"
#include "CollisionShape.h"
#include "resources/CapsuleShape3D.h"
#include "resources/SphereShape3D.h"
#include "resources/MeshShape3D.h"
#include "CollisionTypes.h"
//#include <t3d/t3dmath.h>
#include <vector>

//std::vector<Node::NodeBase*> AllNodes{};

namespace Collision
{


    /****** Import Collision ******/
    void GenerateCollisionFromFile(std::string file_path, Octree* collisionOctree);//public DONE
    void ReadOctreeFromFile(FILE* col_file, Octree* collisionOctree);//private DONE

    /****** Support ****/
    bool TestAABBvsAABB(const T3DVec3* aMin, const T3DVec3* aMax, const T3DVec3* bMin, const T3DVec3* bMax);// DONE
    bool TestAABBCapsuleTriangle();//Do we want to do this?
    void ClosestPointOnLineSegment(T3DVec3* res, const T3DVec3* A, const T3DVec3* B, const T3DVec3* Point);// DONE

    void scaleVector(T3DVec3* vector, float scale);// May change later... DONE
    float clamp_Float(float value, float min, float max);//DONE

    /****** Test Collision *********/
    bool TestAllCollision(std::vector<Node::PhysicsBody3D*>* allPhysicsObjects, float deltaTime);//public DONE
    bool TestCollision(Node::PhysicsBody3D* instigatorObject, Node::PhysicsBody3D* otherObject, float deltaTime);
    bool SpecificCollisionTest(Node::CollisionShape* instigatorShape, Node::CollisionShape* otherShape, float deltaTime, T3DVec3* penetration_normal, float* penetration_depth);

    /*Broad Test*/
    bool TestCapsuleMeshCollision(Node::CollisionShape* instigatorShape, Node::CollisionShape* otherShape, float deltaTime, T3DVec3* penetration_normal, float* penetration_depth);
    bool TestSphereMeshCollision(Node::CollisionShape* instigatorShape, Node::CollisionShape* otherShape, float deltaTime, T3DVec3* penetration_normal, float* penetration_depth);
    bool TestCapsuleCapsuleCollision(Node::CollisionShape* instigatorShape, Node::CollisionShape* otherShape, float deltaTime, T3DVec3* penetration_normal, float* penetration_depth);
    bool TestCapsuleSphereCollision(Node::CollisionShape* instigatorShape, Node::CollisionShape* otherShape, float deltaTime, T3DVec3* penetration_normal, float* penetration_depth);
    bool TestSphereSphereCollision(Node::CollisionShape* instigatorShape, Node::CollisionShape* otherShape, float deltaTime, T3DVec3* penetration_normal, float* penetration_depth);

    /*Attempt Collide*/
    bool CollideCapsuleMeshOctree();
    bool CollideCapsuleSphereOctree();

    /*Octree Traversal*/
    bool TraverseOctreeCapsule(Octree* node, Resource::CapsuleShape3D* capsule, Node::CollisionShape* instigatorShape, float deltaTime, T3DVec3* penetration_normal, float* penetration_depth);//Can we make this general? Need a general Callide_shape_Triangle function to do that.
    bool TraverseOctreeSphere(Octree* node, Resource::SphereShape3D* sphere, Node::CollisionShape* instigatorShape, float deltaTime, T3DVec3* penetration_normal, float* penetration_depth);//Can we make this general? Need a general Callide_shape_Triangle function to do that.

    /*Primitive Collision Tests*/
    bool CollideCapsuleTriangle(T3DVec3* vertices, Resource::CapsuleShape3D* capsule, Node::CollisionShape* instigatorShape, T3DVec3* penetration_normal, float* penetration_depth);
    bool CollideSphereTriangle(T3DVec3* vertices, T3DVec3 center, float radius, Node::CollisionShape* instigatorShape, T3DVec3* penetration_normal, float* penetration_dept);
    bool CollideSphereSphere();
    bool CollideCapsuleSphere();// not implemented yet
    bool CollideCapsuleCapsule(Resource::CapsuleShape3D* instigatingCapsule, Resource::CapsuleShape3D* otherCapsule);// I think this works? Check.

    /*Respond to Collision*/
    void RespondCollideNSlide(T3DVec3 penetration_normal, float penetration_depth);
    void RespondCollideNStop(T3DVec3 penetration_normal, float penetration_depth);//should return collision info for custom collision
}