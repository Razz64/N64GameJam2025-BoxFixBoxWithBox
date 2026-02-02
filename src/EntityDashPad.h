#pragma once
#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmodel.h>
#include "Node.h"
#include "StaticMesh.h"
#include "resources/SphereShape3D.h"
#include "CharacterBody3D.h"
#include "CollisionShape.h"
#include "CollisionTypes.h"
#include "3rdPersonPlayer.h"


namespace Node
{
	class EntityDashPad : public CharacterBody3D
	{
	private:

		//TEMPORARY!!!
		//T3DModelState* t3dState;
		Node::StaticMesh* mesh;//should child nodes be private?

		//Node::CharacterBody3D* body;
		Node::CollisionShape* shape;

		//Resource::CapsuleShape3D* capsule;
		Resource::SphereShape3D* capsule;

		float speedMul;

		bool burnout;
		float timer;
		float maxTime;

	protected:

	public:

		EntityDashPad(T3DModelState* state, std::vector<Node::PhysicsBody3D*>* allPhysicsObjects, float spd);
		~EntityDashPad();

		void update(float deltaTime);
		void fixedUpdate(float deltaTime);
		void draw3D(float deltaTime);

		virtual void respondToCollision(T3DVec3* penetration_normal, float penetration_depth, Node::CollisionShape* instigatorShape, Node::CollisionShape* otherShape, float deltaTime) override;

		//void deleteNode();
	};
}