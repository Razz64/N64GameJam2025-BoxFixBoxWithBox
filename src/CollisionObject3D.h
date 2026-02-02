#pragma once
#include <libdragon.h>
#include <t3d/t3d.h>
#include <vector>
#include "Node.h"
#include "CollisionShape.h"
#include "CollisionTypes.h"


namespace Node
{
	//class CollisionShape;

	class CollisionObject3D : public NodeBase
	{
	private:
		std::vector<Node::CollisionShape*> shapeChildren{};

	protected:

	public:
		CollisionObject3D();
		~CollisionObject3D();

		void update(float deltaTime);
		void fixedUpdate(float deltaTime);

		void addNewShape(Node::CollisionShape* newShape);
		std::vector<Node::CollisionShape*> GetShapeChildren();

		struct Collision::AABB getGlobalAABB();
		void addChild(NodeBase* newChild);
	};
}