#pragma once
#include <libdragon.h>
#include <t3d/t3d.h>
#include "Node.h"
#include "Camera.h"
#include "StaticMesh.h"
#include "CharacterBody3D.h"
#include "CollisionShape.h"
#include "resources/CapsuleShape3D.h"
#include "resources/SphereShape3D.h"
#include "projectile.h"
#include "EntitySpawner.h"
#include <vector>

namespace Node
{
	class ThirdPersonPlayer : public CharacterBody3D
	{
	private:
		float viewAngleVert{};
		float viewAngleHor{};

		float camspeed;

		float movementAcceleration;
		float vertAcceleration;
		float gravAcceleration;
		float staticFriction;
		float maxHorzSpeed;
		float extraMaxHorzSpeed;

		float vertBurstSpeed;
		bool prevHeldL;

		

		bool tempMaxSpeedUp;

		std::vector<Node::PhysicsBody3D*>* physicsObjects;
		std::vector<Node::Projectile*> ammo;
		T3DModelState* theState;

		
		Node::StaticMesh* mesh;

		Node::StaticMesh* meshCrosshair;
		/////Node::CharacterBody3D* body;
		Node::CollisionShape* shape;
		
		Resource::SphereShape3D* capsule;
		//Resource::CapsuleShape3D* capsule;

		//T3DViewport* viewport;

		void shootBox(T3DVec3* direction);

	protected:

	public:
		Node::Camera* camera;
		bool isStartMode;

		float boostPercent;
		ThirdPersonPlayer(T3DViewport* vp, T3DModelState* state, std::vector<Node::PhysicsBody3D*>* allPhysicsObjects);
		~ThirdPersonPlayer();

		void update(float deltaTime);
		void fixedUpdate(float deltaTime);
		void draw3D(float deltaTime);

		int getAmmoCount();

		virtual void respondToCollision(T3DVec3* penetration_normal, float penetration_depth, Node::CollisionShape* instigatorShape, Node::CollisionShape* otherShape, float deltaTime) override;

		void startMode(bool on);
		float getHorzViewAngle() { return viewAngleHor; }
		float getVertViewAngle() { return viewAngleVert; }
		void setViewAngle(float horz, float vert);
	};
}