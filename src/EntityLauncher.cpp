#include "EntityLauncher.h"

namespace {
	//uint32_t refCount{ 0 };//still can't be final
	//std::vector<Node::PhysicsBody3D*>* physicsObjects;
}

namespace Node
{

	EntityLauncher::EntityLauncher(T3DModelState* state, std::vector<Node::PhysicsBody3D*>* allPhysicsObjects, T3DVec3* dir, float spd) : CharacterBody3D::CharacterBody3D(allPhysicsObjects)
	{
		direction = *dir;
		speed = spd;

		burnout = false;
		timer = 0.f;
		maxTime = 0.5f;

		entityCollider = true;

		mesh = new Node::StaticMesh(state);
		addChild(mesh);
		//if (refCount++ == 0) {
		mesh->loadModelWhole("rom:/launch_pad.t3dm");
		//}

		capsule = new Resource::SphereShape3D();
		shape = new Node::CollisionShape(capsule);
		addChild(shape);
		capsule->setRadius(60.f);

		setLocalTransformEuler(0.4f, 0.4f, 0.4f, 0.f, 0.f, 0.f, position.v[0], position.v[1], position.v[2]);
	}
	EntityLauncher::~EntityLauncher()
	{

	}

	void EntityLauncher::update(float deltaTime)
	{
	}

	void EntityLauncher::fixedUpdate(float deltaTime)
	{
		if (burnout)
		{
			timer += deltaTime;
			if (timer >= maxTime)
			{
				burnout = false;
				timer = 0.f;
			}
		}
	}

	void EntityLauncher::draw3D(float deltaTime)
	{
	}

	//Needs to be moved to 3rdPersonPlayer
	void EntityLauncher::respondToCollision(T3DVec3* penetration_normal, float penetration_depth, Node::CollisionShape* instigatorShape, Node::CollisionShape* otherShape, float deltaTime)
	{
		Node::ThirdPersonPlayer* otherBody = dynamic_cast<Node::ThirdPersonPlayer*>(instigatorShape->getParent());
		if (otherBody && !burnout)
		{
			t3d_vec3_norm(&direction);
			otherBody->velocity.v[1] = 0.f;
			otherBody->velocity += direction * speed;
			burnout = true;
			timer = 0.f;
		}
		else
		{
			//CharacterBody3D::respondToCollision(penetration_normal, penetration_depth, instigatorShape, otherShape, deltaTime);
		}
		//add too velocity: direction
	}

}