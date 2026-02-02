#include "BigSprite.h"

namespace {
	//uint32_t refCount{ 0 };//still can't be final
}

namespace Node
{
	BigSprite::BigSprite(T3DModelState* state, std::vector<Node::PhysicsBody3D*>* allPhysicsObjects, int levelNum)
	{
		//int intArray2D[8][8];

		int intArray0[8][8] = {
			{0, 0, 1, 2, 1, 2, 0, 0},
			{0, 0, 2, 1, 1, 2, 0, 0},
			{0, 0, 1, 2, 1, 2, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0},
		};
		int intArray1[8][8] = {
			{0, 0, 0, 0, 2, 0, 0, 0},
			{0, 0, 0, 2, 1, 2, 0, 0},
			{0, 0, 2, 1, 1, 1, 2, 0},
			{0, 0, 2, 1, 2, 1, 2, 0},
			{0, 0, 0, 2, 0, 2, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0},
		};
		int intArray2[8][8] = {
			{0, 0, 2, 2, 2, 0, 0, 0},
			{0, 2, 3, 3, 3, 2, 0, 0},
			{0, 3, 2, 3, 2, 3, 0, 0},
			{0, 3, 2, 3, 2, 3, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0},
		};
		int intArray3[8][8] = {
			{0, 0, 0, 2, 0, 0, 0, 0},
			{0, 0, 0, 4, 0, 0, 0, 0},
			{0, 0, 4, 2, 4, 0, 0, 0},
			{2, 4, 2, 4, 2, 4, 2, 0},
			{0, 0, 4, 2, 4, 0, 0, 0},
			{0, 0, 0, 4, 0, 0, 0, 0},
			{0, 0, 0, 2, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0},
		};
		int intArray4[8][8] = {
			{0, 0, 2, 2, 2, 0, 0, 0},
			{0, 1, 2, 2, 2, 1, 0, 0},
			{0, 0, 4, 4, 4, 0, 0, 0},
			{0, 4, 2, 4, 4, 0, 0, 0},
			{0, 0, 1, 1, 1, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0},
		};
		int intArray5[8][8] = {
			{3, 0, 0, 3, 3, 2, 0, 1},
			{3, 0, 3, 3, 3, 2, 1, 2},
			{3, 3, 3, 1, 3, 2, 2, 2},
			{3, 3, 1, 1, 3, 2, 2, 2},
			{3, 1, 3, 4, 4, 2, 3, 2},
			{4, 4, 3, 2, 2, 3, 4, 2},
			{0, 0, 0, 3, 2, 0, 0, 4},
			{0, 0, 0, 4, 4, 0, 0, 0},
		};

		switch (levelNum)
		{
		case 0:
			setupArray(state, allPhysicsObjects, intArray0);
			break;
		case 1:
			setupArray(state, allPhysicsObjects, intArray1);
			break;
		case 2:
			setupArray(state, allPhysicsObjects, intArray2);
			break;
		case 3:
			setupArray(state, allPhysicsObjects, intArray3);
			break;
		case 4:
			setupArray(state, allPhysicsObjects, intArray4);
			break;
		case 5:
			setupArray(state, allPhysicsObjects, intArray5);
			break;
		default:
			break;
		}


		

	}
	BigSprite::~BigSprite()
	{
		
	}

	void BigSprite::update(float deltaTime)
	{

	}
	void BigSprite::fixedUpdate(float deltaTime)
	{
		numBoxes = 0;
		for (int i = 0; i < (int)(sizeof(array2D) / sizeof(array2D[0])); i++)//num of columns
		{
			for (int j = 0; j < (int)(sizeof(array2D[i]) / sizeof(array2D[0][0])); j++)//num of rows
			{
				if (array2D[i][j] && array2D[i][j]->targetCollider)
				{
					numBoxes++;
				}
			}
		}
		if (numBoxes == 0)
		{
			levelEnd = true;
		}
		//debugf("mesh.x %f, mesh.y %f, mesh.z %f\n", transform->m[3][0], transform->m[3][1], transform->m[3][2]);
	}
	void BigSprite::draw3D(float deltaTime)
	{
		//mesh->draw3D(deltaTime);
	}
	void BigSprite::setupArray(T3DModelState* state, std::vector<Node::PhysicsBody3D*>* allPhysicsObjects, int intArr[8][8])
	{
		numBoxes = 0;
		bool spriteCompleted = false;
		for (int i = 0; i < (int)(sizeof(array2D) / sizeof(array2D[0])); i++)//num of columns
		{
			for (int j = 0; j < (int)(sizeof(array2D[i]) / sizeof(array2D[0][0])); j++)//num of rows
			{
				if (intArr[j][i] == 0) continue;
				numBoxes++;
				array2D[j][i] = new Node::Box(state, allPhysicsObjects);
				addChild(array2D[j][i]);
				array2D[j][i]->position.v[0] = i * 75.f - 75.f * ((int)(sizeof(array2D) / sizeof(array2D[0])) - 1) / 2;
				array2D[j][i]->position.v[1] = j * 75.f + 75.f / 2;// - 75.f * (int)(sizeof(array2D[i]) / sizeof(array2D[0][0])) / 2;
				array2D[j][i]->position.v[2] = 0;
				array2D[j][i]->setColour(static_cast<colourType>(intArr[j][i]), false);
			}
		}
	}
}