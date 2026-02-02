#pragma once
#include <libdragon.h>
#include <t3d/t3d.h>
#include "Resource.h"
#include "Shape3D.h"
//#include "../CollisionTypes.h"
//#include "../collision.h"
#include <vector>
#include <string>

namespace Resource
{
	class MeshShape3D : public Shape3D
	{
	private:
		//float radius;
		Collision::Octree actualOctree;

	protected:

		

	public:
		virtual struct Collision::AABB calcAABB(T3DVec3* pos) override;

		Collision::Octree* octree;

		MeshShape3D();
		~MeshShape3D();
	};
}