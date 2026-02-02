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
#include "projectile.h"


namespace Node
{
	class EntitySpawner : public NodeBase
	{
	private:

		

		//TEMPORARY!!!
		//T3DModelState* t3dState;
		Node::StaticMesh* mesh;//should child nodes be private?

		//Node::CharacterBody3D* body;
		//Node::CollisionShape* shape;
		Node::NodeBase* centerOfProj;

		//Resource::CapsuleShape3D* capsule;
		//Resource::SphereShape3D* capsule;

		Projectile* projectiles[4];

		float rotation_speed;
		float ROT_SPEED;

		float timer;
		float maxTime;


	protected:

	public:
		colourType colour;

		EntitySpawner(T3DModelState* state, std::vector<Node::PhysicsBody3D*>* allPhysicsObjects, colourType newColour);
		~EntitySpawner();

		void update(float deltaTime);
		void fixedUpdate(float deltaTime);
		void draw3D(float deltaTime);

		void setupProjectile(Projectile* proj, int i);

		void tryRespawn();

		void removeElement(Projectile* proj);

		//virtual void respondToCollision(T3DVec3* penetration_normal, float penetration_depth, Node::CollisionShape* instigatorShape, Node::CollisionShape* otherShape, float deltaTime) override;

		//void deleteNode();
	};
}