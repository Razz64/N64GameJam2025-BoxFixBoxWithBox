#include "EntitySpawner.h"

namespace {
	//uint32_t refCount{ 0 };//still can't be final
	//std::vector<Node::PhysicsBody3D*>* physicsObjects;
	T3DModelState* modelState;
	std::vector<Node::PhysicsBody3D*>* allObjects;
}

namespace Node
{

	EntitySpawner::EntitySpawner(T3DModelState* state, std::vector<Node::PhysicsBody3D*>* allPhysicsObjects, colourType newColour)// : CharacterBody3D::CharacterBody3D(allPhysicsObjects)
	{
		colour = newColour;

		if (modelState == NULL)
		{
			modelState =  state;
		}
		if (allObjects == NULL)
		{
			allObjects = allPhysicsObjects;
		}
		timer = 0.f;
		maxTime = 3.f;

		rotation_speed = 0.0f;
		ROT_SPEED = 1.f;

		mesh = new Node::StaticMesh(state);
		addChild(mesh);
		//if (refCount++ == 0) {
		mesh->loadModelWhole("rom:/box.t3dm");
		mesh->setLocalTransformEuler(0.1f, 0.1f, 0.1f, 0.f, 0.f, 0.f, position.v[0], position.v[1], position.v[2]);
		//}

		//capsule = new Resource::SphereShape3D();
		//shape = new Node::CollisionShape(capsule);
		centerOfProj = new Node::NodeBase();
		addChild(centerOfProj);

		projectiles[0] = new Node::Projectile(state, allPhysicsObjects);
		centerOfProj->addChild(projectiles[0]);
		//projectiles[0]->position.v[0] = 100.f;
		setupProjectile(projectiles[0], 0);
		projectiles[0]->setColour(colour, false);

		projectiles[1] = new Node::Projectile(state, allPhysicsObjects);
		centerOfProj->addChild(projectiles[1]);
		//projectiles[1]->position.v[2] = 100.f;
		setupProjectile(projectiles[1], 1);
		projectiles[1]->setColour(colour, false);

		projectiles[2] = new Node::Projectile(state, allPhysicsObjects);
		centerOfProj->addChild(projectiles[2]);
		//projectiles[2]->position.v[0] = -100.f;
		setupProjectile(projectiles[2], 2);
		projectiles[2]->setColour(colour, false);

		projectiles[3] = new Node::Projectile(state, allPhysicsObjects);
		centerOfProj->addChild(projectiles[3]);
		//projectiles[3]->position.v[2] = -100.f;
		setupProjectile(projectiles[3], 3);
		projectiles[3]->setColour(colour, false);

		setLocalTransformEuler(1.f, 1.f, 1.f, 0.f, 0.f, 0.f, position.v[0], position.v[1], position.v[2]);
	}
	EntitySpawner::~EntitySpawner()
	{

	}

	void EntitySpawner::update(float deltaTime)
	{
		
		//shape->setLocalTransformEuler(1.f, 1.f, 1.f, 0.0, rotation_speed, 0.0, shape->position.v[0], shape->position.v[1], shape->position.v[2]);
		//rotation_speed += ROT_SPEED * deltaTime;
	}

	void EntitySpawner::fixedUpdate(float deltaTime)
	{
		timer += deltaTime;
		if (timer >= maxTime)
		{
			tryRespawn();
			timer = 0.f;
		}
	}

	void EntitySpawner::draw3D(float deltaTime)
	{
	}

	void EntitySpawner::setupProjectile(Projectile* proj, int i)
	{
		switch (i)
		{
		case 0:
			proj->position.v[0] = 100.f;
			break;
		case 1:
			proj->position.v[2] = 100.f;
			break;
		case 2:
			proj->position.v[0] = -100.f;
			break;
		case 3:
			proj->position.v[2] = -100.f;
			break;
		default:
			break;
		}

		proj->setState(IDLE);
	}

	void EntitySpawner::tryRespawn()
	{
		for (int i = 0; i < 4; i++)
		{
			if (projectiles[i] == NULL)
			{
				projectiles[i] = new Node::Projectile(modelState, allObjects);
				centerOfProj->addChild(projectiles[i]);
				setupProjectile(projectiles[i], i);
				projectiles[i]->setColour(colour, false);
			}
		}
	}

	void EntitySpawner::removeElement(Projectile* proj)
	{
		for (int i = 0; i < 4; i++)
		{
			if (projectiles[i] == proj)
			{
				projectiles[i] = NULL;
				timer = 0.f;
				return;
			}
		}
	}

	

}