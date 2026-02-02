#include "CollisionShape.h"

namespace Node
{
	CollisionShape::CollisionShape(Resource::Shape3D* s3D)
	{
		shape3D = s3D;
	}
	CollisionShape::~CollisionShape()
	{
		
	}

	void CollisionShape::update(float deltaTime)
	{

	}
	void CollisionShape::fixedUpdate(float deltaTime)
	{

	}


	Collision::AABB CollisionShape::calcAABB()
	{
		T3DVec3 pos = getGlobalPosition();
		return shape3D->calcAABB(&pos);
	}
	Collision::CollisionType CollisionShape::getType()
	{
		return shape3D->getType();
	}
	Resource::Shape3D* CollisionShape::getShape3D()
	{
		return shape3D;
	}
	/*void CollisionShape::setParent(NodeBase* newParent)
	{
		Node::CollisionObject3D* collisionParent = dynamic_cast<Node::CollisionObject3D*>(newParent);
		if (!collisionParent) return;//!!! This is an error, should throw one!
		parent = newParent;
		collisionParent->addNewShape(this);
	}*/
}