#pragma once
#include <libdragon.h>
#include <t3d/t3d.h>
#include "Node.h"
#include "StaticMesh.h"

namespace Node
{
	class Camera : public NodeBase
	{
	private:
		T3DViewport* viewport;

		

	protected:

	public:
		bool camLag;
		T3DVec3 target{};
		T3DVec3 prevTarget{};
		T3DVec3 pos{};
		T3DVec3 camFrontNorm{};
		T3DVec3 CamUp = { 0,1,0 };
		float zoom{};
		bool useAltTarget;
		T3DVec3 altTarget;
		

		Camera(T3DViewport* vp, T3DModelState* state);
		~Camera();

		void update(float deltaTime);
		void fixedUpdate(float deltaTime);

		void forceUpdate();
		//void draw3D(float deltaTime);
		void setAltTarget(T3DVec3* newAltTarget) { altTarget = *newAltTarget; }
	};
}