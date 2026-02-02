#include "Shape3D.h"

namespace Resource
{

	Shape3D::Shape3D()
	{
	}

	Collision::CollisionType Shape3D::getType()
	{
		return type;
	}
}