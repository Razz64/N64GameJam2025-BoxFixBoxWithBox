#pragma once
#include <libdragon.h>
#include <t3d/t3d.h>
#include "Resource.h"
#include "Shape3D.h"

namespace Resource
{
	class CapsuleShape3D : public Shape3D
	{
	private:
		float height;
		float mid_height;
		float radius;

	protected:

		

	public:
		virtual Collision::AABB calcAABB(T3DVec3* pos) override;

		CapsuleShape3D(float rad = 20.f, float h = 80.f);
		~CapsuleShape3D();

		float getRadius()
		{
			return radius;
		}
		float getMidHeight()
		{
			return mid_height;
		}
		float getHeight()
		{
			return height;
		}
	};
}