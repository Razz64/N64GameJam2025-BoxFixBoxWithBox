#include "StaticMesh.h"

namespace {// only accessible within this file, should be changed eventually
	//constexpr float BASE_SCALE = 0.2f;
	//constexpr float ROT_SPEED = 0.1f;
	
}

// Shared resources: ???
namespace {
	//T3DModel* model{};
	//T3DObject* obj{};
	//uint32_t refCount{ 0 };
}

namespace Node
{
	StaticMesh::StaticMesh(T3DModelState* state)
	{
		isInvisible = false;
		t3dState = state;
	}
	StaticMesh::~StaticMesh()
	{

	}

	void StaticMesh::update(float deltaTime)
	{

	}
	void StaticMesh::fixedUpdate(float deltaTime)
	{

	}
	void StaticMesh::draw3D(float deltaTime)
	{
		if (!isInvisible)
		{
			updateTransformFP();
			t3d_matrix_set(transformFP, true);
			rspq_block_run(dplDraw);
		}
	}

	void StaticMesh::loadModelWhole(const char* modelPath)
	{
		model = t3d_model_load(modelPath);//FS_BASE_PATH no declared in scope

		rspq_block_begin();
		t3d_model_draw(model);
		
		dplDraw = rspq_block_end();
	}
	void StaticMesh::loadModelOBJ(const char* modelPath, int alphaComp, int objIndex)
	{
		//if (refCount++ == 0) {//Good idea, but needs to be per model, not StaticMesh node. Maybe a preload or model prepare function?
		model = t3d_model_load(modelPath);//FS_BASE_PATH no declared in scope
		obj = t3d_model_get_object_by_index(model, objIndex);

		rspq_block_begin();
		t3d_model_draw_material(obj->material, t3dState);//for draw_object() function, it only handles the mesh part. to load/change matterials must run this function before it
		rdpq_mode_alphacompare(alphaComp);
		t3d_model_draw_object(obj, nullptr);//nullptr = bonematricies
		dplDraw = rspq_block_end();//obj->userBlock = rspq_block_end();
		//}
	}
}