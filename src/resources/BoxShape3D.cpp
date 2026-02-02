#include "BoxShape3D.h"

namespace Resource {


	BoxShape3D::BoxShape3D(float width, float height, float depth)
	{
		type = Collision::BOX;
		size = (T3DVec3){width, height, depth};
	}

	struct Collision::AABB BoxShape3D::calcAABB(T3DVec3* pos)
	{
		boundingBox = {
			.min = (T3DVec3) {0.f, 0.f, 0.f},
			.max = (T3DVec3) {0.f, 0.f, 0.f}
		};
		return boundingBox;
	}




}