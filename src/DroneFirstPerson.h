#pragma once
#include <libdragon.h>
#include <t3d/t3d.h>
#include "Node.h"
#include "Camera.h"

namespace Node
{
	class DroneCamera : public NodeBase
	{
	private:
		float viewAngleVert{};
		float viewAngleHor{};

		float camspeed;

		Node::Camera* camera;

		//T3DViewport* viewport;

	protected:

	public:
		DroneCamera(T3DViewport* vp, T3DModelState* state);
		~DroneCamera();

		void update(float deltaTime);
		void fixedUpdate(float deltaTime);
		//void draw3D(float deltaTime);
	};
}