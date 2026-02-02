#pragma once
#include <libdragon.h>
#include <t3d/t3d.h>
#include "CollisionObject3D.h"
#include "CollisionTypes.h"
//#include "BigSprite.h"
#include <vector>
#include <algorithm>

namespace Node
{
	enum colourType {EMPTY, RED, BLUE, GREEN, YELLOW};

	class PhysicsBody3D : public CollisionObject3D
	{
	private:
		std::vector<Node::PhysicsBody3D*>* allPhysicsObjectsPtr;
		int16_t physicsIndex;

	protected:
		

	public:
		colourType colour;

		bool bulletCollider;//temp
		bool playerCollider;//temp
		bool targetCollider;
		bool pickupCollider;
		bool explodingCollider;
		bool entityCollider;
		bool disableCollider;

		bool isStatic;//remove eventually
		PhysicsBody3D(std::vector<Node::PhysicsBody3D*>* allPhysicsObjects);
		~PhysicsBody3D();

		void update(float deltaTime);
		void fixedUpdate(float deltaTime);

		virtual void respondToCollision(T3DVec3* penetration_normal, float penetration_depth, Node::CollisionShape* instigatorShape, Node::CollisionShape* otherShape, float deltaTime) = 0;

		void setPhysicsIndex(int16_t newIndex) { physicsIndex = newIndex; }
		int16_t getPhysicsIndex() { return physicsIndex; }

		virtual void deleteNode() override;

		virtual void wasHit() {};

		//virtual void setColour(colourType newColour, bool isTransparent) = 0;
	};
}