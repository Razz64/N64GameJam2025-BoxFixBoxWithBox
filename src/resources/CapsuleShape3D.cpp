#include "CapsuleShape3D.h"

namespace Resource {


	CapsuleShape3D::CapsuleShape3D(float rad, float h)
	{
		type = Collision::CAPSULE;
		radius = rad;
		height = h;
		mid_height = height - radius * 2;
	}

	Collision::AABB CapsuleShape3D::calcAABB(T3DVec3* pos)
	{
		boundingBox = {
			.min = (T3DVec3) {pos->v[0] - radius, pos->v[1] - height / 2, pos->v[2] - radius},
			.max = (T3DVec3) {pos->v[0] + radius, pos->v[1] + height / 2, pos->v[2] + radius}
		};
		return boundingBox;
	}

}