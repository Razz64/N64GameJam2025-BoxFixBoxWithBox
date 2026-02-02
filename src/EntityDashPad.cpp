#include "EntityDashPad.h"

namespace {
	//uint32_t refCount{ 0 };//still can't be final
	//std::vector<Node::PhysicsBody3D*>* physicsObjects;
}

namespace Node
{

	EntityDashPad::EntityDashPad(T3DModelState* state, std::vector<Node::PhysicsBody3D*>* allPhysicsObjects, float spd) : CharacterBody3D::CharacterBody3D(allPhysicsObjects)
	{
		speedMul = spd;

		burnout = false;
		timer = 0.f;
		maxTime = 0.3f;

		entityCollider = true;

		mesh = new Node::StaticMesh(state);
		addChild(mesh);
		//if (refCount++ == 0) {
		mesh->loadModelWhole("rom:/speed_pad.t3dm");
		//}

		capsule = new Resource::SphereShape3D();
		shape = new Node::CollisionShape(capsule);
		addChild(shape);
		capsule->setRadius(60.f);

		setLocalTransformEuler(0.4f, 0.4f, 0.4f, 0.f, 0.f, 0.f, position.v[0], position.v[1], position.v[2]);
	}
	EntityDashPad::~EntityDashPad()
	{

	}

	void EntityDashPad::update(float deltaTime)
	{
	}

	void EntityDashPad::fixedUpdate(float deltaTime)
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

	void EntityDashPad::draw3D(float deltaTime)
	{
	}

	void EntityDashPad::respondToCollision(T3DVec3* penetration_normal, float penetration_depth, Node::CollisionShape* instigatorShape, Node::CollisionShape* otherShape, float deltaTime)
	{
		Node::ThirdPersonPlayer* otherBody = dynamic_cast<Node::ThirdPersonPlayer*>(instigatorShape->getParent());
		if (otherBody && !burnout)
		{
			///T3DVec3 horzVel = otherBody->velocity;
			///float velY = horzVel.v[1];
			///horzVel.v[1] = 0.f;
			///float horzSpeed = t3d_vec3_len(&horzVel);
			///t3d_vec3_norm(horzVel);
			///horzSpeed *= speedMul;
			otherBody->velocity.v[0] *= speedMul;
			otherBody->velocity.v[2] *= speedMul;
			burnout = true;
			timer = 0.f;
		}
		else
		{
			//CharacterBody3D::respondToCollision(penetration_normal, penetration_depth, instigatorShape, otherShape, deltaTime);
		}
	}

}