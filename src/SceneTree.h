#pragma once
#include <libdragon.h>
#include <vector>
//#include "../actors/base.h"
//#include "../camera/camera.h"
#include "Node.h"

class SceneTree
{
private:

protected:
    Node::NodeBase root{};
    std::vector<Node::NodeBase*> children{};//temporary
    //Camera camera{};

    //virtual void updateSceneTree(float deltaTime) = 0;
    //virtual void draw3D(float deltaTime) = 0;

public:

    virtual ~SceneTree() {}

    void update(float deltaTime);
    void draw(float deltaTime);

    //virtual void draw2D(float deltaTime) {}

    //Camera& getCam() { return camera; }
};