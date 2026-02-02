#pragma once
#include <libdragon.h>
#include <vector>
#include "Node.h"

//Not sure what to do with scenes now...
//seems like they should be a data type, a saved object, loaded and then that's it
//the tree, and it's nodes, should be what's actually used

namespace Node
{
    class Scene : public NodeBase
    {
    private:

    protected:
        //std::vector<Actor::Base*> actors{};
        //Camera camera{};

        virtual void updateScene(float deltaTime) = 0;


    public:

        /*virtual ~Scene() {
            for (auto actor : actors) {
                delete actor;
            }
        }*/

        void update(float deltaTime);
        virtual void draw3D(float deltaTime) = 0;

        virtual void draw2D(float deltaTime) {}

        //Camera& getCam() { return camera; }
    };
}