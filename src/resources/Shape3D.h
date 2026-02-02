#pragma once
#include <libdragon.h>
#include <t3d/t3d.h>
#include "Resource.h"
#include "../CollisionTypes.h"
//#include "../collision.h"


namespace Resource
{
	//struct Collision::AABB;
	class Shape3D : public ResourceBase
	{
	private:

	protected:
		Collision::CollisionType type;
		struct Collision::AABB boundingBox;

	

	public:
		Shape3D();
		~Shape3D();

		virtual struct Collision::AABB calcAABB(T3DVec3* pos) = 0;
		Collision::CollisionType getType();
	};
}

