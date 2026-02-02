#include "Box.h"

namespace {
	//uint32_t refCount{ 0 };//still can't be final
	float timeToExplode;

	int numShots = 0;
}

namespace Node
{
	void Box::shootBox(T3DVec3* direction)
	{
		

		Projectile* newBoxShot;
		newBoxShot = new Projectile(modelState, allObjects);
		newBoxShot->setColour(colour, false);
		parent->getParent()->addChild(newBoxShot);
		newBoxShot->setState(EXPLODING);
		newBoxShot->position = getGlobalPosition();
		newBoxShot->velocity = *direction * 400.f;
		//newBoxShot->getGlobalPosition();
		numShots++;
		debugf("num shots: %d\n", numShots);
	}

	Box::Box(T3DModelState* state, std::vector<Node::PhysicsBody3D*>* allPhysicsObjects) : CharacterBody3D::CharacterBody3D(allPhysicsObjects)
	{
		modelState = state;
		allObjects = allPhysicsObjects;

		timer = 0.f;
		timeToExplode = 2.f;
		exploded = false;

		numShots = 0;

		rotation_speed = 0.0f;
		ROT_SPEED = 0.0f;
		BASE_SCALE_1 = 0.5f;
		BASE_SCALE_2 = 0.5f;
		BASE_SCALE_3 = 0.5f;

		mesh_red = new Node::StaticMesh(state);
		addChild(mesh_red);
		//if (refCount++ == 0) {
		mesh_red->loadModelWhole("rom:/box_red.t3dm");
		//}
		mesh_invis_red = new Node::StaticMesh(state);
		addChild(mesh_invis_red);
		mesh_invis_red->loadModelWhole("rom:/box_invis_red.t3dm");

		mesh_blue = new Node::StaticMesh(state);
		addChild(mesh_blue);
		mesh_blue->loadModelWhole("rom:/box_blue.t3dm");

		mesh_invis_blue = new Node::StaticMesh(state);
		addChild(mesh_invis_blue);
		mesh_invis_blue->loadModelWhole("rom:/box_invis_blue.t3dm");



		mesh_yellow = new Node::StaticMesh(state);
		addChild(mesh_yellow);
		//if (refCount++ == 0) {
		mesh_yellow->loadModelWhole("rom:/box_yellow.t3dm");
		//}
		mesh_invis_yellow = new Node::StaticMesh(state);
		addChild(mesh_invis_yellow);
		mesh_invis_yellow->loadModelWhole("rom:/box_yellow_invis.t3dm");

		mesh_green = new Node::StaticMesh(state);
		addChild(mesh_green);
		mesh_green->loadModelWhole("rom:/box_green.t3dm");

		mesh_invis_green = new Node::StaticMesh(state);
		addChild(mesh_invis_green);
		mesh_invis_green->loadModelWhole("rom:/box_invis_green.t3dm");


		position.v[0] = 200.f;
		position.v[1] = 0.f;
		position.v[2] = 200.f;

		//mesh->setLocalTransformEuler(0.2f, 0.2f, 0.2f, 0.0, 0.0, 0.0, mesh->position.v[0], mesh->position.v[1], mesh->position.v[2]);

		

		//body = new Node::CharacterBody3D(allPhysicsObjects);
		targetCollider = true;
		//capsule = new Resource::CapsuleShape3D();
		capsule = new Resource::SphereShape3D(50.f);
		shape = new Node::CollisionShape(capsule);
		//addChild(body);//Should be top node, change later.
		addChild(shape);

		srand(static_cast<unsigned int>(time(nullptr)));
	}
	Box::~Box()
	{

	}

	void Box::update(float deltaTime)
	{
		setLocalTransformEuler(BASE_SCALE_1, BASE_SCALE_2, BASE_SCALE_3, 0.0, rotation_speed, 0.0, position.v[0], position.v[1], position.v[2]);
		rotation_speed += ROT_SPEED;
	}
	void Box::fixedUpdate(float deltaTime)
	{
		if (!exploded)
		{
			timer += deltaTime;
			if (timer >= timeToExplode)
			{
				timeToExplode += deltaTime;
				explode();
				exploded = true;
			}
		}
		//debugf("mesh.x %f, mesh.y %f, mesh.z %f\n", transform->m[3][0], transform->m[3][1], transform->m[3][2]);
	}
	void Box::draw3D(float deltaTime)
	{
		//mesh->draw3D(deltaTime);
	}
	void Box::explode()
	{
		setColour(colour, true); 
		debugf("BOOOM! Explode!\n");
		///mesh->isInvisible = true;
		float randnumx = ((rand() % 101)) - 50;
		float randnumy = ((rand() % 50));
		float randnumz = ((rand() % 101)) - 50;
		T3DVec3 direction = { randnumx, randnumy, randnumz };
		t3d_vec3_norm(&direction);
		shootBox(&direction);
	}
	void Box::wasHit()
	{
		//mesh->isInvisible = true;
		setColour(colour, false);
		targetCollider = false;
		//	NEED SIGNALS!
	}
	void Box::setColour(colourType newColour, bool isTransparent)
	{
		colour = newColour;
		mesh_red->isInvisible = true;
		mesh_invis_red->isInvisible = true;
		mesh_invis_blue->isInvisible = true;
		mesh_blue->isInvisible = true;

		mesh_yellow->isInvisible = true;
		mesh_invis_yellow->isInvisible = true;
		mesh_invis_green->isInvisible = true;
		mesh_green->isInvisible = true;
		switch (newColour)
		{
		case Node::EMPTY:
			break;
		case Node::RED:
			if (isTransparent) mesh_invis_red->isInvisible = false;
			else mesh_red->isInvisible = false;
			break;
		case Node::BLUE:
			if (isTransparent) mesh_invis_blue->isInvisible = false;
			else mesh_blue->isInvisible = false;
			break;
		case Node::GREEN:
			if (isTransparent) mesh_invis_green->isInvisible = false;
			else mesh_green->isInvisible = false;
			break;
		case Node::YELLOW:
			if (isTransparent) mesh_invis_yellow->isInvisible = false;
			else mesh_yellow->isInvisible = false;
			break;
		default:
			break;
		}
	}
}