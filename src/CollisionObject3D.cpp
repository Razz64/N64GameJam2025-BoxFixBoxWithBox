#include "CollisionObject3D.h"

namespace Node
{
	CollisionObject3D::CollisionObject3D()
	{

	}
	CollisionObject3D::~CollisionObject3D()
	{

	}

	void CollisionObject3D::update(float deltaTime)
	{

	}
	void CollisionObject3D::fixedUpdate(float deltaTime)
	{

	}

	void CollisionObject3D::addNewShape(Node::CollisionShape* newShape)
	{
		shapeChildren.push_back(newShape);
	}

	std::vector<Node::CollisionShape*> CollisionObject3D::GetShapeChildren()
	{
		return shapeChildren;
	}

	Collision::AABB CollisionObject3D::getGlobalAABB()
	{
		return Collision::AABB();
	}

	void CollisionObject3D::addChild(NodeBase* newChild)
	{
		CollisionShape* newShape = dynamic_cast<Node::CollisionShape*>(newChild);
		if (newShape)
		{
			addNewShape(newShape);
		}
		newChild->setParent(this);
		children.push_back(newChild);
		
	}
}