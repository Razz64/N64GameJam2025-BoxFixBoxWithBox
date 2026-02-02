#include "CharacterBody3D.h"

namespace Node
{
	CharacterBody3D::CharacterBody3D(std::vector<Node::PhysicsBody3D*>* allPhysicsObjects) : PhysicsBody3D::PhysicsBody3D(allPhysicsObjects)
	{
		velocity = (T3DVec3){ 0.f, 0.f, 0.f };
		//bulletCollider = false;
		//playerCollider = false;
	}
	CharacterBody3D::~CharacterBody3D()
	{

	}

	void CharacterBody3D::update(float deltaTime)
	{

	}
	void CharacterBody3D::fixedUpdate(float deltaTime)
	{
		position.v[0] += velocity.v[0] * deltaTime;
		position.v[1] += velocity.v[1] * deltaTime;
		position.v[2] += velocity.v[2] * deltaTime;
	}
	void CharacterBody3D::respondToCollision(T3DVec3* penetration_normal, float penetration_depth, Node::CollisionShape* instigatorShape, Node::CollisionShape* otherShape, float deltaTime)
	{
		
		//debugf("-----------------------Holy cow!\n");
		//Collide and Slide
		float velocity_length = t3d_vec3_len(velocity);
		T3DVec3 velocityNormalized = velocity;
		if (velocity_length == 0.f)//This may be wrong ...
		{
			velocityNormalized.v[0] = 0;
			velocityNormalized.v[1] = 1;
			velocityNormalized.v[2] = 0;
		}
		else
		{
			velocityNormalized /= velocity_length;
		}

		T3DVec3 undesired_motion = *penetration_normal * t3d_vec3_dot(&velocityNormalized, penetration_normal);
		T3DVec3 desired_motion = velocityNormalized - undesired_motion;
		velocity = desired_motion * velocity_length;
		//remove penetration, directly using local position seems wrong...
		position += *penetration_normal * (penetration_depth + 0.0001f);
		//do we apply velocity here? did before but should we?
	}
	/*void CharacterBody3D::deleteNode()
	{
		//for ()
		PhysicsBody3D::deleteNode();
	}*/
}