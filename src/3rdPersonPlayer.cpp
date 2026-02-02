#include "3rdPersonPlayer.h"

namespace
{
    float scale_fact = 1.0001;
    float terminalVel = -700.f;
    //T3DVec3 CamUp = { 0,1,0 };
    bool debugJump = false;
}

namespace Node
{

    ThirdPersonPlayer::ThirdPersonPlayer(T3DViewport* vp, T3DModelState* state, std::vector<Node::PhysicsBody3D*>* allPhysicsObjects) : CharacterBody3D::CharacterBody3D(allPhysicsObjects)
    {
        //viewport = vp;
        physicsObjects = allPhysicsObjects;
        theState = state;
        position.v[1] = 100.f;

        viewAngleVert = T3D_DEG_TO_RAD(30.0f);
        viewAngleHor = T3D_DEG_TO_RAD(0.0f);

        /*target = T3DVec3{{0, 0, 0}};//300
        pos = T3DVec3{ {0, 0, 0} };
        camFrontNorm = T3DVec3{ {0, 0, 0} };
        CamUp = { 0,1,0 };
        zoom = 400.f;// = 1.0f;*/
        camspeed = 0.02f;
        maxHorzSpeed = 500.f;
        extraMaxHorzSpeed = 900.f;
        movementAcceleration = 1000.f;
        vertAcceleration = 900.f;
        gravAcceleration = 600.f;
        staticFriction = 600.f;

        vertBurstSpeed = 250.f;
        prevHeldL = false;

        camera = new Node::Camera(vp, state);
        addChild(camera);
        camera->position.v[1] = 30.f;

        mesh = new Node::StaticMesh(state);
        mesh->loadModelWhole("rom:/box.t3dm");
        addChild(mesh);
        mesh->setLocalTransformEuler(0.2f, 0.2f, 0.2f, 0.0, 0.0, 0.0, mesh->position.v[0], mesh->position.v[1], mesh->position.v[2]);

        meshCrosshair = new Node::StaticMesh(state);
        meshCrosshair->loadModelOBJ("rom:/crosshair.t3dm", 20);
        camera->addChild(meshCrosshair);
        meshCrosshair->isInvisible = true;

        camera->zoom = 100.f;

        //body = new Node::CharacterBody3D(allPhysicsObjects);
        isStatic = false;
        capsule = new Resource::SphereShape3D();
        //capsule = new Resource::CapsuleShape3D();
        shape = new Node::CollisionShape(capsule);
        addChild(shape);//Should be top node, change later.
        //body->addChild(shape);

        playerCollider = true;
        boostPercent = 100.f;
        tempMaxSpeedUp = false;
    }
    ThirdPersonPlayer::~ThirdPersonPlayer()
    {

    }

    void ThirdPersonPlayer::shootBox(T3DVec3* direction)
    {
        if (ammo.size() != 0)
        {
            Projectile* newBoxShot = ammo.back();
            ammo.pop_back();
            //newBoxShot = new Projectile(theState, physicsObjects);
            //parent->addChild(newBoxShot);
            newBoxShot->setState(SHOOTING);
            newBoxShot->position = camera->getGlobalPosition();
            newBoxShot->velocity = *direction * 1000.f;
            //newBoxShot->getGlobalPosition();
        }

    }

    void ThirdPersonPlayer::update(float deltaTime)
    {//This should all probably be moved to camera itself??? but what about input... not sure yet
        //NodeBase::update(deltaTime);

        /*pos.v[0] = position.v[0];
        pos.v[1] = position.v[1];
        pos.v[2] = position.v[2];*/
        
        meshCrosshair->setLocalTransformEuler(0.2f, 0.2f, 0.2f, 0.0, viewAngleHor, -viewAngleVert, mesh->position.v[0], mesh->position.v[1], mesh->position.v[2]);

        //camera->update(deltaTime);
        //mesh->update(deltaTime);
        ///camera->forceUpdate();
    }

    void ThirdPersonPlayer::fixedUpdate(float deltaTime)
    {

        if (isStartMode)
        {
            camera->zoom = 400.f;
            auto camFront = T3DVec3{
           fm_cosf(viewAngleHor) * fm_cosf(viewAngleVert),//how does eye direction compare to camera front? I think they're identical.
           fm_sinf(viewAngleVert),
           fm_sinf(viewAngleHor) * fm_cosf(viewAngleVert)
            };
            camFront *= camera->zoom;//!!!!!!!!!!!!!!!!!!where's da zoom???

            t3d_vec3_norm(&camFront); // is this needed?
            camera->camFrontNorm = camFront;

            T3DVec3 CamRightNorm{};
            t3d_vec3_cross(&CamRightNorm, &camera->camFrontNorm, &camera->CamUp);
            t3d_vec3_norm(&CamRightNorm);
            return;
        }

        //Should be seperated out
        auto btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);
        auto held = joypad_get_buttons_held(JOYPAD_PORT_1);
        auto stick = joypad_get_inputs(JOYPAD_PORT_1);

        if (btn.b) debugJump = !debugJump;

        T3DVec3 stickInput = (T3DVec3){ {
            (float)stick.stick_x,
            0.f,
            (float)stick.stick_y
        } };
        //deadzone
        if (fabs(stickInput.v[0]) < 20.f) stickInput.v[0] = 0.f;
        if (fabs(stickInput.v[2]) < 20.f) stickInput.v[2] = 0.f;

        if (held.z)
        {
            
        }
        else
        {
            
        }
        
        if (held.z)
        {
            camera->camLag = false;
            camera->zoom = 50.f;
            meshCrosshair->isInvisible = false;
            viewAngleHor += stickInput.v[0] * deltaTime * camspeed;
            viewAngleVert -= stickInput.v[2] * deltaTime * camspeed;
        }
        else
        {
            camera->camLag = true;
            camera->zoom = 100.f;
            meshCrosshair->isInvisible = true;
            viewAngleHor += stickInput.v[0] * deltaTime * camspeed * 1.4;
            viewAngleVert -= stickInput.v[2] * deltaTime * camspeed * 1.4;
        }
        if (viewAngleVert > T3D_DEG_TO_RAD(89.f)) viewAngleVert = T3D_DEG_TO_RAD(89.f);//!!! Get the acutal PI var!
        if (viewAngleVert < T3D_DEG_TO_RAD(-89.f)) viewAngleVert = T3D_DEG_TO_RAD(-89.f);//!!! Get the acutal PI var!

        auto camFront = T3DVec3{
           fm_cosf(viewAngleHor) * fm_cosf(viewAngleVert),//how does eye direction compare to camera front? I think they're identical.
           fm_sinf(viewAngleVert),
           fm_sinf(viewAngleHor) * fm_cosf(viewAngleVert)
        };
        camFront *= camera->zoom;//!!!!!!!!!!!!!!!!!!where's da zoom???

        t3d_vec3_norm(&camFront); // is this needed?
        camera->camFrontNorm = camFront;

        T3DVec3 CamRightNorm{};
        t3d_vec3_cross(&CamRightNorm, &camera->camFrontNorm, &camera->CamUp);
        t3d_vec3_norm(&CamRightNorm);

        //  get c-buttons for forward/side to side movement
        //      and check if z-button pressed, then it's up and down movement with c-up/down
        //      and check if r-button pressed, then double speed rotation and movement
        //float moveSpeed = (held.l == 1) ? 2.f : 1.f;//should be made consts!
        float leftRightMovement = (held.d_left - held.d_right);// * moveSpeed; 
        float ForwBackMovement = (-held.d_up + held.d_down); //(!held.z);// * moveSpeed;
        //float upDownMovement = held.l;// * vertMoveAccel;//(held.d_up + -held.d_down) * held.z * moveSpeed;


        T3DVec3 camFrontHorzNorm = camera->camFrontNorm;
        camFrontHorzNorm.v[1] = 0;
        t3d_vec3_norm(&camFrontHorzNorm);
        T3DVec3 camRightHorzNorm = CamRightNorm;
        camRightHorzNorm.v[1] = 0;
        t3d_vec3_norm(&camRightHorzNorm);
        if (leftRightMovement != 0 || ForwBackMovement != 0)
        {
            float accel = movementAcceleration;
            if (tempMaxSpeedUp)
            {
                accel *= 1.5;
            }

            velocity.v[0] += (ForwBackMovement * camFrontHorzNorm.v[0] + leftRightMovement * camRightHorzNorm.v[0]) * deltaTime * accel;// + leftRightMovement*camFrontNorm.v[2];

            velocity.v[2] += (ForwBackMovement * camFrontHorzNorm.v[2] + leftRightMovement * camRightHorzNorm.v[2]) * deltaTime * accel;// + leftRightMovement*camFrontNorm.v[2];
        }
        else if (t3d_vec3_len(&velocity) > 0.f)
        {
            T3DVec3 velocityNorm = velocity;
            float ySpeed = velocity.v[1];
            velocityNorm.v[1] = 0.f;
            float horzSpeed = t3d_vec3_len(&velocityNorm);

            if (horzSpeed != 0)
            {
                velocityNorm = velocityNorm / horzSpeed;
            }

            horzSpeed -= deltaTime * staticFriction;
            if (horzSpeed < 0.f)
            {
                horzSpeed = 0.f;
            }
            velocity = velocityNorm * horzSpeed;
            velocity.v[1] = ySpeed;
        }
        else
        {
            velocity *= 0.f;
        }

        /**/
        T3DVec3 directionVec;

        directionVec.v[0] = (ForwBackMovement * camFrontHorzNorm.v[0] + leftRightMovement * camRightHorzNorm.v[0]);
        directionVec.v[1] = 0.f;
        directionVec.v[2] = (ForwBackMovement * camFrontHorzNorm.v[2] + leftRightMovement * camRightHorzNorm.v[2]);

        float upDownMovement = 0;
        boostPercent += 10.f*deltaTime;
        if (boostPercent >= 100.f) boostPercent = 100.f;
        if (held.z)
        {
            if (held.l && !prevHeldL)
            {
                T3DVec3 camVec = -camera->camFrontNorm;
                shootBox(&camVec);
            }
        }
        else if (held.l && !prevHeldL && boostPercent >= 33.33f)
        {
            upDownMovement = 1;
            prevHeldL = true;
            /*if (horzSpeed >= 200)
            {
                horzSpeed += 100;
            }
            else
            {
                horzSpeed = 300;
            }*/
            /*jumpVelocity.v[0] = (ForwBackMovement * camFrontHorzNorm.v[0] + leftRightMovement * camRightHorzNorm.v[0]) * 200.f;// + leftRightMovement*camFrontNorm.v[2];
            jumpVelocity.v[1] = 0.f;// + leftRightMovement*camFrontNorm.v[2];
            jumpVelocity.v[2] = (ForwBackMovement * camFrontHorzNorm.v[2] + leftRightMovement * camRightHorzNorm.v[2]) * 200.f;// + leftRightMovement*camFrontNorm.v[2];*/
            velocity /= 3;
            velocity += directionVec * 200.f;
            if (!debugJump) boostPercent -= 33.33f;
            if (boostPercent <= 0.f) boostPercent = 0.f;
        }
        prevHeldL = held.l;

        T3DVec3 velocityNorm = velocity;
        float ySpeed = velocity.v[1];
        velocityNorm.v[1] = 0.f;
        float horzSpeed = t3d_vec3_len(&velocityNorm);
        if (horzSpeed != 0)
        {
            velocityNorm = velocityNorm / horzSpeed;
        }
        printf("******************************************************horzspeed: %f\n", horzSpeed);
        if (horzSpeed > maxHorzSpeed)
        {
            if (tempMaxSpeedUp)
            {
                if (horzSpeed > extraMaxHorzSpeed)
                {
                    horzSpeed = extraMaxHorzSpeed;
                }
            }
            else
            {
                horzSpeed = maxHorzSpeed;
            }
            
        }
        else
        {
            tempMaxSpeedUp = false;
        }
        velocity = velocityNorm * horzSpeed;
        velocity.v[1] = ySpeed;







        /*position.v[0] += ForwBackMovement * camera->camFrontNorm.v[0] + leftRightMovement * CamRightNorm.v[0];// + leftRightMovement*camFrontNorm.v[2];
        position.v[1] += upDownMovement - deltaTime * 100;
        position.v[2] += ForwBackMovement * camera->camFrontNorm.v[2] + leftRightMovement * CamRightNorm.v[2];// + leftRightMovement*camFrontNorm.v[2];*/
        velocity.v[1] += (upDownMovement)*vertBurstSpeed - deltaTime * gravAcceleration;
        if (velocity.v[1] <= terminalVel) velocity.v[1] = terminalVel;

        position.v[0] += velocity.v[0] * deltaTime;
        position.v[1] += velocity.v[1] * deltaTime;
        position.v[2] += velocity.v[2] * deltaTime;

        if (position.v[1] <= -4000.f)
        {
            position = (T3DVec3){ 0.f, 100.f, 238.f };
            velocity = (T3DVec3){ 0.f, 0.f, 0.f };
        }

        if (ammo.size() != 0)
        {
            //ammo.front()->isStatic = true;
            ammo.back()->position = position;
            ammo.back()->position.v[1] += 25.f;
            ammo.back()->position.v[0] -= 25.f;
            for (Projectile* proj : ammo)
            {
                if (proj == ammo.back()) continue;
                proj->position.v[1] = -401;
            }
        }

        
        

        //mesh->fixedUpdate(deltaTime);
        ///camera->forceUpdate();
    }
    void ThirdPersonPlayer::draw3D(float deltaTime)
    {
        //mesh->draw3D(deltaTime);

    }
    int ThirdPersonPlayer::getAmmoCount()
    {
        return ammo.size();
    }
    void ThirdPersonPlayer::respondToCollision(T3DVec3* penetration_normal, float penetration_depth, Node::CollisionShape* instigatorShape, Node::CollisionShape* otherShape, float deltaTime)
    {
        Node::Projectile* otherProj = dynamic_cast<Node::Projectile*>(otherShape->getParent());
        if (otherProj && otherProj->pickupCollider)
        {
            if (ammo.size() < 8)
            {
                ammo.insert(ammo.begin(), otherProj);
                otherProj->pickupCollider = false;
                
                otherProj->position.v[1] = -201.f;
                otherProj->setState(HELD);
                ///if otherProj's parent's parent is a EntitySpawner, remove from spawner array
                Node::EntitySpawner* parentSpawner = dynamic_cast<Node::EntitySpawner*>(otherProj->getParent()->getParent());
                if (parentSpawner)
                {
                    parentSpawner->removeElement(otherProj);
                }
                parent->addChild(otherProj);
            }
            
        }
        else
        {
            Node::CharacterBody3D* otherBody = dynamic_cast<Node::CharacterBody3D*>(otherShape->getParent());
            if (otherBody && otherBody->entityCollider)
            {
                otherBody->respondToCollision(penetration_normal, penetration_depth, instigatorShape, otherShape, deltaTime);
                tempMaxSpeedUp = true;
            }
            else
            {
                CharacterBody3D::respondToCollision(penetration_normal, penetration_depth, instigatorShape, otherShape, deltaTime);
                T3DVec3 upDir = { 0.f, 1.f, 0.f };
                if (t3d_vec3_dot(penetration_normal, &upDir) > 0.3f)
                {
                    boostPercent += 5.f;
                    if (boostPercent > 100.f) boostPercent = 100.f;
                }

            }
        }
        
    }
    void ThirdPersonPlayer::startMode(bool on)
    {
        if (on)
        {
            isStartMode = true;
            mesh->isInvisible = true;
            meshCrosshair->isInvisible = true;
        }
        else
        {
            isStartMode = false;
            mesh->isInvisible = false;
            meshCrosshair->isInvisible = false;
            camera->useAltTarget = false;
        }
        
    }
    void ThirdPersonPlayer::setViewAngle(float horz, float vert)
    {
        viewAngleHor = horz;
        viewAngleVert = vert;
    }
}