#pragma once
#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmodel.h>
#include "Node.h"

namespace Node
{
	class StaticMesh : public NodeBase
	{
	private:
		//TEMPORARY!!!
		T3DModelState* t3dState;
		rspq_block_t* dplDraw{};
		T3DObject* obj{};//maybe moved in some way, not general enough

	protected:

	public:
		bool isInvisible;
		

		T3DModel* model{};
		StaticMesh(T3DModelState* state);
		~StaticMesh();

		void update(float deltaTime);
		void fixedUpdate(float deltaTime);
		void draw3D(float deltaTime);
		void loadModelOBJ(const char* modelPath, int alphaComp = 100, int objIndex = 0);
		void loadModelWhole(const char* modelPath);
	};
}