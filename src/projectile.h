#pragma once
#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmodel.h>
#include "Node.h"
#include "StaticMesh.h"
#include "resources/CapsuleShape3D.h"
#include "resources/SphereShape3D.h"
#include "CharacterBody3D.h"
#include "CollisionShape.h"
#include "CollisionTypes.h"


namespace Node
{
	enum projectileState { EXPLODING, SHOOTING, IDLE, HELD };
	class Projectile : public CharacterBody3D
	{
	private:
		
		//TEMPORARY!!!
		//T3DModelState* t3dState;
		//Node::StaticMesh* mesh;//should child nodes be private?
		Node::StaticMesh* mesh_red;
		Node::StaticMesh* mesh_invis_red;
		Node::StaticMesh* mesh_blue;
		Node::StaticMesh* mesh_invis_blue;
		Node::StaticMesh* mesh_yellow;
		Node::StaticMesh* mesh_invis_yellow;
		Node::StaticMesh* mesh_green;
		Node::StaticMesh* mesh_invis_green;

		//Node::CharacterBody3D* body;
		Node::CollisionShape* shape;

		//Resource::CapsuleShape3D* capsule;
		//Resource::SphereShape3D* capsule;

		float meshSize;
		

	protected:

	public:

		Resource::SphereShape3D* capsule;
		
		projectileState projState;

		Projectile(T3DModelState* state, std::vector<Node::PhysicsBody3D*>* allPhysicsObjects);
		~Projectile();

		void update(float deltaTime);
		void fixedUpdate(float deltaTime);
		void draw3D(float deltaTime);

		virtual void respondToCollision(T3DVec3* penetration_normal, float penetration_depth, Node::CollisionShape* instigatorShape, Node::CollisionShape* otherShape, float deltaTime) override;

		void setState(projectileState state);
		//void deleteNode();
		void setColour(colourType newColour, bool isTransparent);
	};
}