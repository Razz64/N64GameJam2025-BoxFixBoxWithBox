#include "DroneFirstPerson.h"

namespace
{
    float scale_fact = 1.0001;
    //T3DVec3 CamUp = { 0,1,0 };
}

namespace Node
{
    DroneCamera::DroneCamera(T3DViewport* vp, T3DModelState* state)
    {
        //viewport = vp;

        viewAngleVert = T3D_DEG_TO_RAD(30.0f);
        viewAngleHor = T3D_DEG_TO_RAD(0.0f);

        /*target = T3DVec3{{0, 0, 0}};//300
        pos = T3DVec3{ {0, 0, 0} };
        camFrontNorm = T3DVec3{ {0, 0, 0} };
        CamUp = { 0,1,0 };
        zoom = 400.f;// = 1.0f;*/
        camspeed = 0.02f;

        camera = new Node::Camera(vp, state);
        addChild(camera);
    }
    DroneCamera::~DroneCamera()
    {

    }

    void DroneCamera::update(float deltaTime)
    {
        //NodeBase::update(deltaTime);

        /*pos.v[0] = position.v[0];
        pos.v[1] = position.v[1];
        pos.v[2] = position.v[2];*/

        auto camFront = T3DVec3{
            fm_cosf(viewAngleHor) * fm_cosf(viewAngleVert),//how does eye direction compare to camera front? I think they're identical.
            fm_sinf(viewAngleVert),
            fm_sinf(viewAngleHor) * fm_cosf(viewAngleVert)
        };
        t3d_vec3_norm(&camFront); // is this needed?
        camera->camFrontNorm = camFront;
        camFront *= camera->zoom;//!!!!!!!!!!!!!!!!!!where's da zoom???

        //Should be seperated out
        auto btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);
        auto held = joypad_get_buttons_held(JOYPAD_PORT_1);
        auto stick = joypad_get_inputs(JOYPAD_PORT_1);
        T3DVec3 stickInput = (T3DVec3){ {
            (float)stick.stick_x,
            0.f,
            (float)stick.stick_y
        } };

        viewAngleHor += stickInput.v[0] * deltaTime * camspeed;//should be mult by deltatime
        viewAngleVert += stickInput.v[2] * deltaTime * camspeed;
        if (viewAngleVert > T3D_DEG_TO_RAD(89.f)) viewAngleVert = T3D_DEG_TO_RAD(89.f);//!!! Get the acutal PI var!
        if (viewAngleVert < T3D_DEG_TO_RAD(-89.f)) viewAngleVert = T3D_DEG_TO_RAD(-89.f);//!!! Get the acutal PI var!

        //  get c-buttons for forward/side to side movement
        //      and check if z-button pressed, then it's up and down movement with c-up/down
        //      and check if r-button pressed, then double speed rotation and movement
        float moveSpeed = (held.r == 1) ? 2.f : 1.f;//should be made consts!
        float leftRightMovement = (-held.c_left + held.c_right) * moveSpeed;
        float ForwBackMovement = (held.c_up - held.c_down) * (!held.z) * moveSpeed;
        float upDownMovement = (held.c_up + -held.c_down) * held.z * moveSpeed;

        T3DVec3 CamRightNorm{};
        t3d_vec3_cross(&CamRightNorm, &camera->camFrontNorm, &camera->CamUp);
        t3d_vec3_norm(&CamRightNorm);

        position.v[0] += ForwBackMovement * camera->camFrontNorm.v[0] + leftRightMovement * CamRightNorm.v[0];// + leftRightMovement*camFrontNorm.v[2];
        position.v[1] += upDownMovement;
        position.v[2] += ForwBackMovement * camera->camFrontNorm.v[2] + leftRightMovement * CamRightNorm.v[2];// + leftRightMovement*camFrontNorm.v[2];

        camera->update(deltaTime);
    }

    void DroneCamera::fixedUpdate(float deltaTime)
    {


    }
}