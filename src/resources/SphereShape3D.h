#pragma once
#include <libdragon.h>
#include <t3d/t3d.h>
#include "Resource.h"
#include "Shape3D.h"

namespace Resource
{
	class SphereShape3D : public Shape3D
	{
	private:
		float radius;

	protected:

		

	public:
		virtual Collision::AABB calcAABB(T3DVec3* pos) override;

		SphereShape3D(float rad = 20.f);
		~SphereShape3D();

		float getRadius()
		{
			return radius;
		}
		void setRadius(float newRad)
		{
			radius = newRad;
		}
	};
}