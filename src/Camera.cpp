#include "Camera.h"

namespace
{
    float scale_fact = 1.0001;
}

namespace Node
{
	Camera::Camera(T3DViewport* vp, T3DModelState* state)
	{
		viewport = vp;

		target = T3DVec3{ {0.1f, 0.1f, 0.1f} };//300
		pos = T3DVec3{ {0.1f, 0.1f, 0.1f} };
		camFrontNorm = T3DVec3{ {0.1f, 0.1f, 0.1f} };
		CamUp = { 0,1,0 };
		zoom = 1.f;// = 1.0f;

		camLag = true;
		
	}
	Camera::~Camera()
	{

	}

	void Camera::update(float deltaTime)
	{
		if (useAltTarget) target = altTarget;

		prevTarget = target;
        //NodeBase::update(deltaTime);
		if (camLag)
		{
			t3d_viewport_look_at(*viewport, pos, prevTarget, T3DVec3{ {0, 1, 0} });
		}
		else
		{
			t3d_viewport_look_at(*viewport, pos, target, T3DVec3{ {0, 1, 0} });
		}
			
		

		//meshCrosshair->setLocalTransformEuler(0.2f, 0.2f, 0.2f, 0.0, 0.0, 0.0, target.v[0], target.v[1]+10.f , target.v[2]);
	}

	void Camera::fixedUpdate(float deltaTime)
	{
		if (useAltTarget) target = altTarget;
		else target = getGlobalPosition();

		if (camLag)
		{
			pos = prevTarget + camFrontNorm * zoom;
		}
		else
		{
			pos = target + camFrontNorm * zoom;
		}
		

		
	}
	void Camera::forceUpdate()
	{
		t3d_viewport_attach(viewport);

		t3d_viewport_set_projection(viewport, T3D_DEG_TO_RAD(75.0f), 40.f, 5000.0f);

		if (useAltTarget) target = altTarget;
		else target = getGlobalPosition();

		if (camLag)
		{
			pos = prevTarget + camFrontNorm * zoom;
			t3d_viewport_look_at(*viewport, pos, prevTarget, T3DVec3{ {0, 1, 0} });
		}
		else
		{
			pos = target + camFrontNorm * zoom;
			t3d_viewport_look_at(*viewport, pos, target, T3DVec3{ {0, 1, 0} });
		}

		
		//meshCrosshair->setLocalTransformEuler(0.2f, 0.2f, 0.2f, 0.0, 0.0, 0.0, target.v[0], target.v[1] + 10.f, target.v[2]);
	}
}