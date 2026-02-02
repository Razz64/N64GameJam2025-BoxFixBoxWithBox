#include "MeshShape3D.h"

namespace Resource {


	MeshShape3D::MeshShape3D()
	{
		type = Collision::MESH;
		octree = &actualOctree;
		//Collision::GenerateCollisionFromFile(file_path, &octree);
		
	}

	Collision::AABB MeshShape3D::calcAABB(T3DVec3* pos)
	{
		boundingBox = {
			.min = octree->boundingBox.min + *pos,
			.max = octree->boundingBox.max + *pos
		};
		return boundingBox;
	}




}