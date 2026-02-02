#pragma once
#include <libdragon.h>
#include <t3d/t3d.h>
#include "Node.h"
//#include "collision.h"
#include "resources/Shape3D.h"
//#include "CollisionObject3D.h"
#include "CollisionTypes.h"

namespace Node
{
	//class CollisionObject3D;

	class CollisionShape : public NodeBase
	{
	private:
		//TEMPORARY!!!
		//T3DModelState* t3dState;
		//rspq_block_t* dplDraw{};
		//T3DObject* obj{};//maybe moved in some way, not general enough
		Resource::Shape3D* shape3D;//a little worried about pointer here
		//struct Collision::AABB boundingBox;

	protected:

	public:
		///// Get bounding box function, call shape3D calcAABB and apply transform to it?
			// maybe apply transform to shape itself first, then calc AABB?
				// is AABB even necessary if not using mesh?

		bool collisionDisabled;
		//bool isDynamic; Is determined by parent: StaticBody3D, RigidBody3D, Area3D, CharacterBody3D
			//These parent nodes should have isPhysical set to true, and collision system should look for their collision shape child

		CollisionShape(Resource::Shape3D* s3D);
		~CollisionShape();

		void update(float deltaTime);
		void fixedUpdate(float deltaTime);
		//void draw3D(float deltaTime);

		struct Collision::AABB calcAABB();
		Collision::CollisionType getType();
		Resource::Shape3D* getShape3D();

		//virtual void setParent(NodeBase* newParent);
	};
}