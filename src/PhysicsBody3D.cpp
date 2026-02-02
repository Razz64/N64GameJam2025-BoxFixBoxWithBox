#include "PhysicsBody3D.h"

namespace {
	//uint32_t refCount{ 0 };//still can't be final
	std::vector<Node::PhysicsBody3D*>* physicsObjects;
}

namespace Node
{
	PhysicsBody3D::PhysicsBody3D(std::vector<Node::PhysicsBody3D*>* allPhysicsObjects)
	{
		physicsIndex = allPhysicsObjects->size();
		allPhysicsObjects->push_back(this);
		allPhysicsObjectsPtr = allPhysicsObjects;
		isStatic = true;//remove eventually
		bulletCollider = false;
		playerCollider = false;
		targetCollider = false;
		pickupCollider = false;
		explodingCollider = false;
		entityCollider = false;
		disableCollider = false;

		if (physicsObjects == NULL)
		{
			physicsObjects = allPhysicsObjects;
		}
	}
	PhysicsBody3D::~PhysicsBody3D()//should handle multiple destructions at a single time, shouldn't need to store objects list or do this directly here!
	{
		allPhysicsObjectsPtr->back()->setPhysicsIndex(physicsIndex);//swap element index, last elem now has this index
		allPhysicsObjectsPtr->at(physicsIndex) = allPhysicsObjectsPtr->back();//place last elem in this elem place
		allPhysicsObjectsPtr->pop_back();//reduce size by one. Last elem is now properly in this place.
	}

	void PhysicsBody3D::update(float deltaTime)
	{

	}
	void PhysicsBody3D::fixedUpdate(float deltaTime)
	{

	}
	void PhysicsBody3D::deleteNode()
	{
		auto physit = std::find(physicsObjects->begin(), physicsObjects->end(), this);
		if (physit != physicsObjects->end()) {
			physicsObjects->erase(physit);
		}
		for (NodeBase* child : children)
		{
			child->deleteNode();
		}
		NodeBase::deleteNode();
	}
}