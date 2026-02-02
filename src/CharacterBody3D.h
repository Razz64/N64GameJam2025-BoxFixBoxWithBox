#pragma once
#include <libdragon.h>
#include <t3d/t3d.h>
#include "PhysicsBody3D.h"
#include "CollisionTypes.h"

namespace Node
{
	class CharacterBody3D : public PhysicsBody3D
	{
	private:
		
	protected:

	public:
		T3DVec3 velocity;

		CharacterBody3D(std::vector<Node::PhysicsBody3D*>* allPhysicsObjects);
		~CharacterBody3D();

		void update(float deltaTime);
		void fixedUpdate(float deltaTime);

		virtual void respondToCollision(T3DVec3* penetration_normal, float penetration_depth, Node::CollisionShape* instigatorShape, Node::CollisionShape* otherShape, float deltaTime) override;

		//void deleteNode();
	};
}