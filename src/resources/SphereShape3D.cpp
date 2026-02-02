#include "SphereShape3D.h"

namespace Resource {


	SphereShape3D::SphereShape3D(float rad)
	{
		type = Collision::SPHERE;
		radius = rad;
	}

	Collision::AABB SphereShape3D::calcAABB(T3DVec3* pos)
	{
		boundingBox = {
			.min = (T3DVec3) {pos->v[0] - radius, pos->v[1] - radius, pos->v[2] - radius},
			.max = (T3DVec3) {pos->v[0] + radius, pos->v[1] + radius, pos->v[2] + radius}
		};
		return boundingBox;
	}

}