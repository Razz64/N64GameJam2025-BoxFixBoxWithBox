#include "Projectile.h"

namespace {
	//uint32_t refCount{ 0 };//still can't be final
	//std::vector<Node::PhysicsBody3D*>* physicsObjects;
}

namespace Node
{

	Projectile::Projectile(T3DModelState* state, std::vector<Node::PhysicsBody3D*>* allPhysicsObjects) : CharacterBody3D::CharacterBody3D(allPhysicsObjects)
	{
		
	//	mesh = new Node::StaticMesh(state);
	//	addChild(mesh);
		//if (refCount++ == 0) {
	//	mesh->loadModelWhole("rom:/box.t3dm");
		//}

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
		

		//body = new Node::CharacterBody3D(allPhysicsObjects);
		//capsule = new Resource::CapsuleShape3D();
		capsule = new Resource::SphereShape3D();
		shape = new Node::CollisionShape(capsule);
		addChild(shape);

/////////velocity = (T3DVec3){ 500.f, 0.f, 500.f };

		//physicsObjects = allPhysicsObjects;
		setState(SHOOTING);
		bulletCollider = false;
	}
	Projectile::~Projectile()
	{
		
	}

	void Projectile::update(float deltaTime)
	{
		setLocalTransformEuler(meshSize, meshSize, meshSize, 0.f, 0.f, 0.f, position.v[0], position.v[1], position.v[2]);
		//rotation_speed += ROT_SPEED;
	}
	void Projectile::fixedUpdate(float deltaTime)
	{
		//debugf("9999999999 I'm still alive! 9999999999\n");
		if (projState == SHOOTING)
		{
			velocity.v[1] -= 300.f * deltaTime;
		}
		else if (projState == EXPLODING)
		{
			velocity.v[1] -= 500.f * deltaTime;
		}
		else if (projState == IDLE)
		{
			velocity.v[0] = 0.f;
			velocity.v[1] = 0.f;
			velocity.v[2] = 0.f;
		}

		position.v[0] += velocity.v[0] * deltaTime;
		position.v[1] += velocity.v[1] * deltaTime;
		position.v[2] += velocity.v[2] * deltaTime;

		if (position.v[1] <= -5000.f)
		{
			deleteNode();
			/*velocity.v[0] = 0.f;
			velocity.v[1] = 0.f;
			velocity.v[2] = 0.f;
			if (position.v[0] < 0) position.v[0] += 100.f;
			else position.v[0] -= 100.f;
			position.v[1] = 500.f;
			if (position.v[2] < 0) position.v[0] += 100.f;
			else position.v[2] -= 100.f;*/

			/*auto it = std::find(parent->children.begin(), parent->children.end(), this);
			if (it != parent->children.end()) {
				parent->children.erase(it);
				debugf("Erased!!!......\n");
			}
			else
			{
				debugf("Wow......\n");
			}
			auto physit = std::find(physicsObjects->begin(), physicsObjects->end(), this);
			if (physit != physicsObjects->end()) {
				physicsObjects->erase(physit);
			}*/
			//delete this;!!!!!!!!!!!!!!!!!!!!!!!!!!!! STill need to properly delete!!!!!
		}
		
		//debugf("mesh.x %f, mesh.y %f, mesh.z %f\n", transform->m[3][0], transform->m[3][1], transform->m[3][2]);
	}
	void Projectile::draw3D(float deltaTime)
	{
		//mesh->draw3D(deltaTime);
	}

	void Projectile::respondToCollision(T3DVec3* penetration_normal, float penetration_depth, Node::CollisionShape* instigatorShape, Node::CollisionShape* otherShape, float deltaTime)
	{
		PhysicsBody3D* instBody = dynamic_cast<PhysicsBody3D*>(instigatorShape->getParent());
		PhysicsBody3D* otherBody = dynamic_cast<PhysicsBody3D*>(otherShape->getParent());
		
		//debugf("\n\n\n\n\n\n\n\n\n\n\n%d vs %d !!!!!!!!!!!!!!!!!!!!!!!44444444444!44444444444\n\n\n\n\n", otherBody->colour, instBody->colour);
		if (otherBody && otherBody->targetCollider && instBody && instBody->bulletCollider && (otherBody->colour == instBody->colour))
		//if (otherBody &&  instBody &&  (otherBody->colour == instBody->colour))
		{
			
			//debugf("Meow Meow Meow It's time to delete both!!!\n");
			//otherBody->deleteNode();
			otherBody->wasHit();
			instBody->deleteNode();
		}

		//CharacterBody3D::respondToCollision(penetration_normal, penetration_depth, instigatorShape, otherShape, deltaTime);
		if (projState == IDLE)
		{

		}
		else if (projState == SHOOTING)
		{
			if (otherShape->getType() == Collision::MESH)
			{
				setState(IDLE);
				//mesh->isInvisible = true;
			}
			else
			{
				
			}
		}
		else if (projState == EXPLODING)
		{
			if (otherShape->getType() == Collision::MESH)
			{
				setState(IDLE);
				//mesh->isInvisible = true;
			}
		}
	}

	void Projectile::setState(projectileState state)
	{
		disableCollider = false;
		switch (state)
		{
		case Node::EXPLODING:
			meshSize = 0.5f;
			projState = EXPLODING;
			isStatic = true;
			//position = getGlobalPosition();
			//newBoxShot->velocity = *direction * 400.f;
			pickupCollider = false;
			explodingCollider = true;
			bulletCollider = false;
			disableCollider = true;
			break;
		case Node::SHOOTING:
			meshSize = 0.2f;
			projState = SHOOTING;
			capsule->setRadius(20.f);
			isStatic = false;
			pickupCollider = false;
			explodingCollider = false;
			bulletCollider = true;
			break;
		case Node::IDLE:
			meshSize = 0.3f;
			projState = IDLE;
			isStatic = true;
			pickupCollider = true;
			explodingCollider = false;
			bulletCollider = false;
			capsule->setRadius(50.f);
			break;
		case Node::HELD:
			meshSize = 0.1f;
			projState = HELD;
			isStatic = true;
			pickupCollider = false;
			explodingCollider = false;
			bulletCollider = false;
			capsule->setRadius(1.f);
			disableCollider = true;
			break;
		default:
			break;
		}
	}

	void Projectile::setColour(colourType newColour, bool isTransparent)
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

	/*void Projectile::deleteNode()
	{
		CharacterBody3D::deleteNode();
	}*/
}

/*bool bulletCollider;
bool playerCollider;
bool targetCollider;
bool pickupCollider;
bool explodingCollider;*/