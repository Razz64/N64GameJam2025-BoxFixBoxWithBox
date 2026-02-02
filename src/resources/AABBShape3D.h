#pragma once
#include <libdragon.h>
#include <t3d/t3d.h>
#include "Resource.h"
#include "Shape3D.h"

namespace Resource
{
	class AABBShape3D : public Shape3D
	{
	private:
		T3DVec3 min;//!!! IM ACTUALLY NOT SURE HOW TO DO THIS not have it be affected by rotation?
		T3DVec3 max;

	protected:

		virtual Collision::AABB calcAABB() override {};

	public:
		AABBShape3D();
		~AABBShape3D();
	};
}