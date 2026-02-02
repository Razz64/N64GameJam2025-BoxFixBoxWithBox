#pragma once
#include <libdragon.h>
#include <t3d/t3d.h>
#include "Resource.h"
#include "Shape3D.h"

namespace Resource
{
	class BoxShape3D : public Shape3D
	{
	private:
		T3DVec3 size;//width, height, depth

	protected:

		virtual struct Collision::AABB calcAABB(T3DVec3* pos) override;

	public:
		BoxShape3D(float width = 1.f, float height = 1.f, float depth = 1.f);
		~BoxShape3D();
	};
}