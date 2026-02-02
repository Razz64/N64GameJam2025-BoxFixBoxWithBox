#pragma once
#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmodel.h>
#include "Node.h"
#include "Box.h"

namespace Node
{
	class BigSprite : public NodeBase
	{
	private:
		

	protected:

	public:

		BigSprite(T3DModelState* state, std::vector<Node::PhysicsBody3D*>* allPhysicsObjects, int levelNum);
		~BigSprite();

		void update(float deltaTime);
		void fixedUpdate(float deltaTime);
		void draw3D(float deltaTime);

		void setupArray(T3DModelState* state, std::vector<Node::PhysicsBody3D*>* allPhysicsObjects, int intArr[8][8]);

		Box* array2D[8][8];

		bool spriteCompleted;

		int numBoxes;
		bool levelEnd;
		
	};
}