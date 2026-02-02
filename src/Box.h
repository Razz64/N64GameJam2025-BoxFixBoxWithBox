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
#include "projectile.h"
#include <ctime>
#include <iostream>
#include <cstdlib>

namespace Node
{
	class Box : public CharacterBody3D
	{
	private:

		float timer;
		
		//colourType colour;

		T3DModelState* modelState;
		std::vector<Node::PhysicsBody3D*>* allObjects;

		
		//TEMPORARY!!!
		//T3DModelState* t3dState;
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
		Resource::SphereShape3D* capsule;

		void shootBox(T3DVec3* direction);
	protected:

	public:
		bool exploded;

		Box(T3DModelState* state, std::vector<Node::PhysicsBody3D*>* allPhysicsObjects);
		~Box();

		void update(float deltaTime);
		void fixedUpdate(float deltaTime);
		void draw3D(float deltaTime);

		float rotation_speed;
		float ROT_SPEED;
		float BASE_SCALE_1;
		float BASE_SCALE_2;
		float BASE_SCALE_3;

		void explode();

		virtual void wasHit() override;

		void setColour(colourType newColour, bool isTransparent);
	};
}