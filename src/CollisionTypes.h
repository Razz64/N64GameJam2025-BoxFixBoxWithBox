#pragma once
#include <libdragon.h>
#include <t3d/t3d.h>

namespace Collision
{
    struct AABB {
        T3DVec3 min;
        T3DVec3 max;
    };

    struct Octree {
        struct AABB boundingBox;
        struct Octree* children;
        T3DVec3* vertices;
        int numVertices;
    };

    enum CollisionType {
        BOX,
        CAPSULE,
        SPHERE,
        MESH//godot has special map types, look into.
        //really want a box shape
    };
}