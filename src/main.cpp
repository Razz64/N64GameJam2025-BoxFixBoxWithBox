#include "main.h"
#include "StaticMesh.h"
#include "Box.h"
#include "Camera.h"
#include "DroneFirstPerson.h"
#include "Node.h"
#include "3rdPersonPlayer.h"
#include "collision.h"
#include "PhysicsBody3D.h"
#include "BigSprite.h"
#include "projectile.h"
#include "EntityDashPad.h"
#include "EntityLauncher.h"
#include "EntitySpawner.h"
#include "MenuNode.h"


std::vector<Node::PhysicsBody3D*> allPhysicsObjects;

namespace {//ALSO SHOULD BE MOVED TO OVERSEER FILE
  //Game* game;
    float accumulator = 0;
    const float dt = 1.0f/(double)120;//where 30 is tickrate
    T3DViewport viewport{};//where should this be put?
    surface_t *currentFB{};

    constexpr int BUFF_COUNT = 3;

    rspq_profile_data_t profileData{};//How to use profiler
    uint64_t lastUcodeTime = 0;

    T3DModelState t3dState{};

    /*Node::StaticMesh* mesh;
    Node::StaticMesh* childMesh;
    Node::StaticMesh* childMesh2;*/
    Node::MenuNode* menuNode;
    Node::NodeBase* topNode;
    Node::ThirdPersonPlayer* player;
    Node::BigSprite* bigSprite;
    //Collision::Octree test_octree;//to be in MeshShape3D

    bool isBasicDebugDisplayOn = false;
    bool startMenu = false;
    bool isMenu = false;

    float levelTimer = 0.f;
    float endTimer = 0.f;
    int currLevel = -1.f;
}

//init
void SystemInit()
{
    profileData.frame_count = 0;
    debug_init_isviewer();
    debug_init_usblog();
    asset_init_compression(2);
    //asset_init_compression(3);
    //wav64_init_compression(3);
    //  There are RSP and RDP debuggers and profiliers. I don't understand them, but I should
    dfs_init(DFS_DEFAULT_LOCATION);//???

    //don't understand these parts too much, should look into.
    joypad_init();
    timer_init();
    rdpq_init();
    audio_init(32000, 3);
    mixer_init(32);

    //rdpq_debug_start();
    #if RSPQ_PROFILE
        rspq_profile_start();
    #endif

    
    constexpr resolution_t res{SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_HEIGHT > 240 ? INTERLACE_HALF : INTERLACE_OFF};//maybe
    display_init(res, DEPTH_16_BPP, BUFF_COUNT, GAMMA_NONE, FILTERS_RESAMPLE);

    t3d_init((T3DInitParams){});
    tpx_init((TPXInitParams){});
    t3d_debug_print_init();
    //t3d_fog_set_enabled(false); ???

    viewport = t3d_viewport_create();//where to put

    
   
}

void loadMenu()
{
    currLevel = -1;

    display_set_fps_limit(30);
    allPhysicsObjects.clear();
    Node::CharacterBody3D* skyBoxChar;
    Node::StaticMesh* skyBoxMesh;
    Node::StaticMesh* sky;

    isMenu = true;

    topNode = new Node::NodeBase();

    menuNode = new Node::MenuNode();
    topNode->addChild(menuNode);

    player = new Node::ThirdPersonPlayer(&viewport, &t3dState, &allPhysicsObjects);
    player->startMode(true);

    skyBoxChar = new Node::CharacterBody3D(&allPhysicsObjects);
    topNode->addChild(skyBoxChar);
    skyBoxMesh = new Node::StaticMesh(&t3dState);
    skyBoxMesh->loadModelWhole("rom:/TEST_ENV_Xramp.t3dm");
    sky = new Node::StaticMesh(&t3dState);
    player->addChild(sky);
    ////topNode->addChild(sky);
    sky->loadModelWhole("rom:/skybox2.t3dm");
    sky->setLocalTransformEuler(4.5f, 4.5f, 4.5f, 0.0, 0.0, 0.0, 0.f, 0.f, 0.f);



    topNode->addChild(player);
    player->position = (T3DVec3){ 0.f, 100.f, 238.f };

    //Transparent must be rendered last!
    bigSprite = new Node::BigSprite(&t3dState, &allPhysicsObjects, 1);
    topNode->addChild(bigSprite);
    bigSprite->position = (T3DVec3){ 0.f, 100.f, -275.f };

    T3DVec3 newTarget = bigSprite->getGlobalPosition();
    newTarget.v[1] += 100.f;
    player->camera->setAltTarget(&newTarget);
    player->setViewAngle(3.14159f / 2.f, 0.f);
    player->camera->useAltTarget = true;
    player->startMode(true);
}

void loadLevel0()
{
    currLevel = 0;
    allPhysicsObjects.clear();
    topNode = NULL;
    player = NULL;
    bigSprite = NULL;
    levelTimer = 0.f;
    //Collision::Octree test_octree;

    //Node::Box* boxMain;
    //Node::Box* boxChild1;
    //Node::Box* boxChild2;
    //Node::DroneCamera* droneCamera;
    

    Node::CharacterBody3D* skyBoxChar;
    Node::StaticMesh* skyBoxMesh;
    Node::CollisionShape* skyShape;
    Resource::MeshShape3D* skyMeshCol;

    Node::StaticMesh* sky;

    //Node::Projectile* testProj;

    //Node::EntityLauncher* launchPad;
    //Node::EntityDashPad* dashPad;

    Node::EntitySpawner* spawner1;
    Node::EntitySpawner* spawner2;


    topNode = new Node::NodeBase();

    player = new Node::ThirdPersonPlayer(&viewport, &t3dState, &allPhysicsObjects);
    

    skyBoxChar = new Node::CharacterBody3D(&allPhysicsObjects);
    topNode->addChild(skyBoxChar);
    skyBoxMesh = new Node::StaticMesh(&t3dState);
    skyBoxMesh->loadModelWhole("rom:/TEST_ENV_X3.t3dm");
    sky = new Node::StaticMesh(&t3dState);
    player->addChild(sky);
    ////topNode->addChild(sky);
    sky->loadModelWhole("rom:/skybox2.t3dm");
    sky->setLocalTransformEuler(4.5f, 4.5f, 4.5f, 0.0, 0.0, 0.0, 0.f, 0.f, 0.f);
    sky->isSkybox = true;

    /*Node::Box* tempBox = new Node::Box(&t3dState, &allPhysicsObjects);
    topNode->addChild(tempBox);
    tempBox->setColour(Node::RED, false);
    tempBox->position = (T3DVec3){ 100.f, 100.f, 238.f };
    tempBox->exploded = true;*/
    
    //debugf("Num verts: ", test_octree.numVertices);
    //debugf("Do children == null? ", test_octree.children == NULL);
    skyMeshCol = new Resource::MeshShape3D();
    Collision::GenerateCollisionFromFile("rom:/TEST_ENV_X3-col.col", skyMeshCol->octree);
    skyShape = new Node::CollisionShape(skyMeshCol);

    skyBoxChar->addChild(skyBoxMesh);
    skyBoxChar->addChild(skyShape);

    /*testProj = new Node::Projectile(&t3dState, &allPhysicsObjects);
    topNode->addChild(testProj);
    testProj->setState(Node::IDLE);
    testProj->position = (T3DVec3){ -200.f, 200.f, -200.f };*/

    /*T3DVec3 dir = {0.f, 1.f, 0.f};
    float spdLaunch = 500.f;
    launchPad = new Node::EntityLauncher(&t3dState, &allPhysicsObjects, &dir, spdLaunch);
    topNode->addChild(launchPad);
    launchPad->position = (T3DVec3){ -138.f, 215.f, 640.f };

    float spdMulDash = 6.f;
    dashPad = new Node::EntityDashPad(&t3dState, &allPhysicsObjects, spdMulDash);
    topNode->addChild(dashPad);
    dashPad->position = (T3DVec3){ 0.f, 433.27, -812.6f };*/

    spawner1 = new Node::EntitySpawner(&t3dState, &allPhysicsObjects, Node::RED);
    topNode->addChild(spawner1);
    spawner1->position = (T3DVec3){ 0.f, 20.f, -85.f };

    spawner2 = new Node::EntitySpawner(&t3dState, &allPhysicsObjects, Node::BLUE);
    topNode->addChild(spawner2);
    spawner2->position = (T3DVec3){ 300.f, 20.f, 225.f };

    topNode->addChild(player);
    player->position = (T3DVec3){ 0.f, 100.f, 238.f };

    //Transparent must be rendeNode::RED last!
    bigSprite = new Node::BigSprite(&t3dState, &allPhysicsObjects, 0);
    topNode->addChild(bigSprite);
    bigSprite->position = (T3DVec3){ 0.f, 100.f, -275.f };

    T3DVec3 newTarget = bigSprite->getGlobalPosition();
    newTarget.v[1] += 100.f;
    player->camera->setAltTarget(&newTarget);
    player->setViewAngle(3.14159f / 2.f, 0.f);
    player->camera->useAltTarget = true;
    player->startMode(true);

}

void loadLevel1()//trampoline
{
    currLevel = 1;
    allPhysicsObjects.clear();
    topNode = NULL;
    player = NULL;
    bigSprite = NULL;
    levelTimer = 0.f;

    Node::CharacterBody3D* skyBoxChar;
    Node::StaticMesh* skyBoxMesh;
    Node::CollisionShape* skyShape;
    Resource::MeshShape3D* skyMeshCol;

    Node::StaticMesh* sky;

    Node::EntitySpawner* spawner1;
    Node::EntitySpawner* spawner2;

    Node::EntityLauncher* launchPad;

    topNode = new Node::NodeBase();

    player = new Node::ThirdPersonPlayer(&viewport, &t3dState, &allPhysicsObjects);


    skyBoxChar = new Node::CharacterBody3D(&allPhysicsObjects);
    topNode->addChild(skyBoxChar);
    skyBoxMesh = new Node::StaticMesh(&t3dState);
    skyBoxMesh->loadModelWhole("rom:/ENV_1_trampoline.t3dm");

    sky = new Node::StaticMesh(&t3dState);
    player->addChild(sky);
    sky->loadModelWhole("rom:/skybox2.t3dm");
    sky->setLocalTransformEuler(4.5f, 4.5f, 4.5f, 0.0, 0.0, 0.0, 0.f, 0.f, 0.f);
    sky->isSkybox = true;

    T3DVec3 dir = { 0.f, 1.f, 0.f };
    float spdLaunch = 1300.f;
    launchPad = new Node::EntityLauncher(&t3dState, &allPhysicsObjects, &dir, spdLaunch);
    topNode->addChild(launchPad);
    launchPad->position = (T3DVec3){ 0.f, -50.f, -70.f };


    skyMeshCol = new Resource::MeshShape3D();
    Collision::GenerateCollisionFromFile("rom:/ENV_1_trampoline-col.col", skyMeshCol->octree);
    skyShape = new Node::CollisionShape(skyMeshCol);

    skyBoxChar->addChild(skyBoxMesh);
    skyBoxChar->addChild(skyShape);


    spawner1 = new Node::EntitySpawner(&t3dState, &allPhysicsObjects, Node::RED);
    topNode->addChild(spawner1);
    spawner1->position = (T3DVec3){ 50.f, -50.f, -70.f };

    spawner2 = new Node::EntitySpawner(&t3dState, &allPhysicsObjects, Node::BLUE);
    topNode->addChild(spawner2);
    spawner2->position = (T3DVec3){ -50.f, -50.f, -70.f };

    topNode->addChild(player);
    player->position = (T3DVec3){ 0.f, 100.f, 238.f };

    //Transparent must be rendeNode::RED last!
    bigSprite = new Node::BigSprite(&t3dState, &allPhysicsObjects, 1);
    topNode->addChild(bigSprite);
    bigSprite->position = (T3DVec3){ 28.f, 896.f, -550.f };

    T3DVec3 newTarget = bigSprite->getGlobalPosition();
    newTarget.v[1] += 100.f;
    player->camera->setAltTarget(&newTarget);
    player->setViewAngle(3.14159f / 2.f, 0.f);
    player->camera->useAltTarget = true;
    player->startMode(true);

}

void loadLevel2()
{
    currLevel = 2;
    allPhysicsObjects.clear();
    topNode = NULL;
    player = NULL;
    bigSprite = NULL;
    levelTimer = 0.f;

    Node::CharacterBody3D* skyBoxChar;
    Node::StaticMesh* skyBoxMesh;
    Node::CollisionShape* skyShape;
    Resource::MeshShape3D* skyMeshCol;

    Node::StaticMesh* sky;

    Node::EntitySpawner* spawner1;
    Node::EntitySpawner* spawner2;

    Node::EntityDashPad* dashPad;

    topNode = new Node::NodeBase();

    player = new Node::ThirdPersonPlayer(&viewport, &t3dState, &allPhysicsObjects);


    skyBoxChar = new Node::CharacterBody3D(&allPhysicsObjects);
    topNode->addChild(skyBoxChar);
    skyBoxMesh = new Node::StaticMesh(&t3dState);
    skyBoxMesh->loadModelWhole("rom:/ENV_2_ramp.t3dm");

    sky = new Node::StaticMesh(&t3dState);
    player->addChild(sky);
    sky->loadModelWhole("rom:/skybox2.t3dm");
    sky->setLocalTransformEuler(4.5f, 4.5f, 4.5f, 0.0, 0.0, 0.0, 0.f, 0.f, 0.f);
    sky->isSkybox = true;


    skyMeshCol = new Resource::MeshShape3D();
    Collision::GenerateCollisionFromFile("rom:/ENV_2_ramp-col.col", skyMeshCol->octree);
    skyShape = new Node::CollisionShape(skyMeshCol);

    skyBoxChar->addChild(skyBoxMesh);
    skyBoxChar->addChild(skyShape);


    spawner1 = new Node::EntitySpawner(&t3dState, &allPhysicsObjects, Node::GREEN);
    topNode->addChild(spawner1);
    spawner1->position = (T3DVec3){ 376.f, 500.f, -3217.f };

    spawner2 = new Node::EntitySpawner(&t3dState, &allPhysicsObjects, Node::BLUE);
    topNode->addChild(spawner2);
    spawner2->position = (T3DVec3){ -418.f, 500.f, -3217.f };

    float spdMulDash = 6.f;
    dashPad = new Node::EntityDashPad(&t3dState, &allPhysicsObjects, spdMulDash);
    topNode->addChild(dashPad);
    dashPad->position = (T3DVec3){ 0.f, 433.27, -812.6f }; 

    topNode->addChild(player);
    player->position = (T3DVec3){ 0.f, 100.f, 238.f };

    //Transparent must be rendeNode::RED last!
    bigSprite = new Node::BigSprite(&t3dState, &allPhysicsObjects, 2);
    topNode->addChild(bigSprite);
    bigSprite->position = (T3DVec3){ -440.f, 20.f, 275.f };

    T3DVec3 newTarget = bigSprite->getGlobalPosition();
    newTarget.v[1] += 100.f;
    player->camera->setAltTarget(&newTarget);
    player->setViewAngle(3.14159f / 2.f, 0.f);
    player->camera->useAltTarget = true;
    player->startMode(true);

}

void loadLevel3()//trampoline
{
    currLevel = 3;
    allPhysicsObjects.clear();
    topNode = NULL;
    player = NULL;
    bigSprite = NULL;
    levelTimer = 0.f;

    Node::CharacterBody3D* skyBoxChar;
    Node::StaticMesh* skyBoxMesh;
    Node::CollisionShape* skyShape;
    Resource::MeshShape3D* skyMeshCol;

    Node::StaticMesh* sky;

    Node::EntitySpawner* spawner1;
    Node::EntitySpawner* spawner2;
    Node::EntitySpawner* spawner3;
    Node::EntitySpawner* spawner4;

    Node::EntityDashPad* dashPad1;
    Node::EntityDashPad* dashPad2;
    Node::EntityDashPad* dashPad3;

    topNode = new Node::NodeBase();

    player = new Node::ThirdPersonPlayer(&viewport, &t3dState, &allPhysicsObjects);


    skyBoxChar = new Node::CharacterBody3D(&allPhysicsObjects);
    topNode->addChild(skyBoxChar);
    skyBoxMesh = new Node::StaticMesh(&t3dState);
    skyBoxMesh->loadModelWhole("rom:/ENV_3_halfpipe.t3dm");

    sky = new Node::StaticMesh(&t3dState);
    player->addChild(sky);
    sky->loadModelWhole("rom:/skybox2.t3dm");
    sky->setLocalTransformEuler(4.5f, 4.5f, 4.5f, 0.0, 0.0, 0.0, 0.f, 0.f, 0.f);
    sky->isSkybox = true;


    skyMeshCol = new Resource::MeshShape3D();
    Collision::GenerateCollisionFromFile("rom:/ENV_3_halfpipe-col.col", skyMeshCol->octree);
    skyShape = new Node::CollisionShape(skyMeshCol);

    skyBoxChar->addChild(skyBoxMesh);
    skyBoxChar->addChild(skyShape);


    spawner1 = new Node::EntitySpawner(&t3dState, &allPhysicsObjects, Node::BLUE);
    topNode->addChild(spawner1);
    spawner1->position = (T3DVec3){ 1118.f, 25.f, -468.f };

    spawner2 = new Node::EntitySpawner(&t3dState, &allPhysicsObjects, Node::BLUE);
    topNode->addChild(spawner2);
    spawner2->position = (T3DVec3){ 1118.f, 25.f, 549.f };

    spawner3 = new Node::EntitySpawner(&t3dState, &allPhysicsObjects, Node::YELLOW);
    topNode->addChild(spawner3);
    spawner3->position = (T3DVec3){ -1118.f, 25.f, 728.f };

    spawner4 = new Node::EntitySpawner(&t3dState, &allPhysicsObjects, Node::YELLOW);
    topNode->addChild(spawner4);
    spawner4->position = (T3DVec3){ -1118.f, 25.f, -453.f };

    topNode->addChild(player);
    player->position = (T3DVec3){ 0.f, 100.f, 238.f };

    float spdMulDash = 6.f;
    dashPad1 = new Node::EntityDashPad(&t3dState, &allPhysicsObjects, spdMulDash);
    topNode->addChild(dashPad1);
    dashPad1->position = (T3DVec3){ -8.f, -757.f, -121.f };

    //float spdMulDash = 6.f;
    dashPad2 = new Node::EntityDashPad(&t3dState, &allPhysicsObjects, spdMulDash);
    topNode->addChild(dashPad2);
    dashPad2->position = (T3DVec3){ -8.f, -757.f, 668.f };

    //float spdMulDash = 6.f;
    dashPad3 = new Node::EntityDashPad(&t3dState, &allPhysicsObjects, spdMulDash);
    topNode->addChild(dashPad3);
    dashPad3->position = (T3DVec3){ -8.f, -757.f, -766.f };

    //Transparent must be rendeNode::RED last!
    bigSprite = new Node::BigSprite(&t3dState, &allPhysicsObjects, 3);
    topNode->addChild(bigSprite);
    bigSprite->position = (T3DVec3){ 0.f, -160.f, -275.f };

    T3DVec3 newTarget = bigSprite->getGlobalPosition();
    newTarget.v[1] += 100.f;
    player->camera->setAltTarget(&newTarget);
    player->setViewAngle(3.14159f / 2.f, 0.f);
    player->camera->useAltTarget = true;
    player->startMode(true);

}

void loadLevel4()//trampoline
{
    currLevel = 4;
    allPhysicsObjects.clear();
    topNode = NULL;
    player = NULL;
    bigSprite = NULL;
    levelTimer = 0.f;

    Node::CharacterBody3D* skyBoxChar;
    Node::StaticMesh* skyBoxMesh;
    Node::CollisionShape* skyShape;
    Resource::MeshShape3D* skyMeshCol;

    Node::StaticMesh* sky;

    Node::EntitySpawner* spawner1;
    Node::EntitySpawner* spawner2;
    Node::EntitySpawner* spawner3;

    Node::EntityDashPad* dashPad1;
    //Node::EntityDashPad* dashPad2;
    Node::EntityDashPad* dashPad3;
    Node::EntityDashPad* dashPad4;

    Node::EntityLauncher* launchPad;

    topNode = new Node::NodeBase();

    player = new Node::ThirdPersonPlayer(&viewport, &t3dState, &allPhysicsObjects);


    skyBoxChar = new Node::CharacterBody3D(&allPhysicsObjects);
    topNode->addChild(skyBoxChar);
    skyBoxMesh = new Node::StaticMesh(&t3dState);
    skyBoxMesh->loadModelWhole("rom:/ENV_4_bowl.t3dm");

    sky = new Node::StaticMesh(&t3dState);
    player->addChild(sky);
    sky->loadModelWhole("rom:/skybox2.t3dm");
    sky->setLocalTransformEuler(4.5f, 4.5f, 4.5f, 0.0, 0.0, 0.0, 0.f, 0.f, 0.f);
    sky->isSkybox = true;


    skyMeshCol = new Resource::MeshShape3D();
    Collision::GenerateCollisionFromFile("rom:/ENV_4_bowl-col.col", skyMeshCol->octree);
    skyShape = new Node::CollisionShape(skyMeshCol);

    skyBoxChar->addChild(skyBoxMesh);
    skyBoxChar->addChild(skyShape);

    float spdMulDash = 12.f;
    dashPad1 = new Node::EntityDashPad(&t3dState, &allPhysicsObjects, spdMulDash);
    topNode->addChild(dashPad1);
    dashPad1->position = (T3DVec3){ 300.f, -3465.f, 116.f };

    /*dashPad2 = new Node::EntityDashPad(&t3dState, &allPhysicsObjects, spdMulDash);
    topNode->addChild(dashPad2);
    dashPad2->position = (T3DVec3){ -600.f, -2811.f, -18.f };*/
    T3DVec3 dir = { 0.f, 1.f, 0.f };
    float spdLaunch = 1100.f;
    launchPad = new Node::EntityLauncher(&t3dState, &allPhysicsObjects, &dir, spdLaunch);
    topNode->addChild(launchPad);
    launchPad->position = (T3DVec3){ -600.f, -2811.f, -18.f };

    dashPad3 = new Node::EntityDashPad(&t3dState, &allPhysicsObjects, spdMulDash);
    topNode->addChild(dashPad3);
    dashPad3->position = (T3DVec3){ -700.f, -2201.f, 111.f };

    dashPad4 = new Node::EntityDashPad(&t3dState, &allPhysicsObjects, spdMulDash);
    topNode->addChild(dashPad4);
    dashPad4->position = (T3DVec3){ -829.f, -1687.f, -108.f };


    spawner1 = new Node::EntitySpawner(&t3dState, &allPhysicsObjects, Node::BLUE);
    topNode->addChild(spawner1);
    spawner1->position = (T3DVec3){ 7.f, -3665.f, 106.f };

    spawner2 = new Node::EntitySpawner(&t3dState, &allPhysicsObjects, Node::RED);
    topNode->addChild(spawner2);
    spawner2->position = (T3DVec3){ -46.f, -3610.f, -264.f};

    spawner3 = new Node::EntitySpawner(&t3dState, &allPhysicsObjects, Node::YELLOW);
    topNode->addChild(spawner3);
    spawner3->position = (T3DVec3){ 0.f, -3700.f, -90 };

    topNode->addChild(player);
    player->position = (T3DVec3){ 0.f, -2500.f, 238.f };

    //Transparent must be rendeNode::RED last!
    bigSprite = new Node::BigSprite(&t3dState, &allPhysicsObjects, 4);
    topNode->addChild(bigSprite);
    bigSprite->position = (T3DVec3){ 0.f, -700.f, 0.f };

    T3DVec3 newTarget = bigSprite->getGlobalPosition();
    newTarget.v[1] += 100.f;
    player->camera->setAltTarget(&newTarget);
    player->setViewAngle(3.14159f / 2.f, 0.f);
    player->camera->useAltTarget = true;
    player->startMode(true);
}

void loadLevel5()//trampoline
{
    currLevel = 5;
    display_set_fps_limit(20);
    allPhysicsObjects.clear();
    topNode = NULL;
    player = NULL;
    bigSprite = NULL;
    levelTimer = 0.f;

    Node::CharacterBody3D* skyBoxChar;
    Node::StaticMesh* skyBoxMesh;
    Node::CollisionShape* skyShape;
    Resource::MeshShape3D* skyMeshCol;

    Node::StaticMesh* sky;

    Node::EntitySpawner* spawner1;
    Node::EntitySpawner* spawner2;
    Node::EntitySpawner* spawner3;
    Node::EntitySpawner* spawner4;

    Node::EntityDashPad* dashPad3;
    Node::EntityDashPad* dashPad4;

    Node::EntityLauncher* launchPad1;
    Node::EntityLauncher* launchPad2;
    Node::EntityLauncher* launchPad3;

    topNode = new Node::NodeBase();

    player = new Node::ThirdPersonPlayer(&viewport, &t3dState, &allPhysicsObjects);


    skyBoxChar = new Node::CharacterBody3D(&allPhysicsObjects);
    topNode->addChild(skyBoxChar);
    skyBoxMesh = new Node::StaticMesh(&t3dState);
    skyBoxMesh->loadModelWhole("rom:/ENV_5_finale.t3dm");

    sky = new Node::StaticMesh(&t3dState);
    player->addChild(sky);
    sky->loadModelWhole("rom:/skybox2.t3dm");
    sky->setLocalTransformEuler(4.5f, 4.5f, 4.5f, 0.0, 0.0, 0.0, 0.f, 0.f, 0.f);
    sky->isSkybox = true;


    skyMeshCol = new Resource::MeshShape3D();
    Collision::GenerateCollisionFromFile("rom:/ENV_5_finale-col.col", skyMeshCol->octree);
    skyShape = new Node::CollisionShape(skyMeshCol);

    skyBoxChar->addChild(skyBoxMesh);
    skyBoxChar->addChild(skyShape);

    float spdMulDash = 6.f;
    dashPad3 = new Node::EntityDashPad(&t3dState, &allPhysicsObjects, spdMulDash);
    topNode->addChild(dashPad3);
    dashPad3->position = (T3DVec3){ 1914.f, 731.f, 1210.f };

    dashPad4 = new Node::EntityDashPad(&t3dState, &allPhysicsObjects, spdMulDash);
    topNode->addChild(dashPad4);
    dashPad4->position = (T3DVec3){ 1850.f, 529.f, -1220.f};


    spawner1 = new Node::EntitySpawner(&t3dState, &allPhysicsObjects, Node::YELLOW);
    topNode->addChild(spawner1);
    spawner1->position = (T3DVec3){ 1977.f, 1093.f, -37.f };

    spawner2 = new Node::EntitySpawner(&t3dState, &allPhysicsObjects, Node::RED);
    topNode->addChild(spawner2);
    spawner2->position = (T3DVec3){ 190.f, 1222.f, -1838.f };

    spawner1 = new Node::EntitySpawner(&t3dState, &allPhysicsObjects, Node::BLUE);
    topNode->addChild(spawner1);
    spawner1->position = (T3DVec3){ -1642.f, 747.f, -58.f };

    spawner2 = new Node::EntitySpawner(&t3dState, &allPhysicsObjects, Node::GREEN);
    topNode->addChild(spawner2);
    spawner2->position = (T3DVec3){ 72.f, 531.f, 1524.f };

    T3DVec3 dir = { 0.f, 1.f, 0.f };
    float spdLaunch = 750.f;
    launchPad1 = new Node::EntityLauncher(&t3dState, &allPhysicsObjects, &dir, spdLaunch);
    topNode->addChild(launchPad1);
    launchPad1->position = (T3DVec3){ 27.f, 20.f, -568.f };

    launchPad2 = new Node::EntityLauncher(&t3dState, &allPhysicsObjects, &dir, spdLaunch);
    topNode->addChild(launchPad2);
    launchPad2->position = (T3DVec3){ 33.f, 463.f, -1082.f };

    launchPad3 = new Node::EntityLauncher(&t3dState, &allPhysicsObjects, &dir, spdLaunch);
    topNode->addChild(launchPad3);
    launchPad3->position = (T3DVec3){ 70.f, 905.f, -1418.f };

    topNode->addChild(player);
    player->position = (T3DVec3){ 0.f, 100.f, 238.f };

    //Transparent must be rendeNode::RED last!
    bigSprite = new Node::BigSprite(&t3dState, &allPhysicsObjects, 5);
    topNode->addChild(bigSprite);
    bigSprite->position = (T3DVec3){ 0.f, 0.f, -275.f };

    T3DVec3 newTarget = bigSprite->getGlobalPosition();
    newTarget.v[1] += 100.f;
    player->camera->setAltTarget(&newTarget);
    player->setViewAngle(3.14159f / 2.f, 0.f);
    player->camera->useAltTarget = true;
    player->startMode(true);
}



void updateAll(Node::NodeBase* currentNode, float deltatime)
{
    currentNode->update(deltatime);
    if (currentNode->children.size() != 0)
    {
        for (Node::NodeBase* node : currentNode->children)
        {
            updateAll(node, deltatime);
        }
    }
}

void killNodes(Node::NodeBase* currentNode)
{
    if (currentNode->children.size() != 0)
    {
        for (Node::NodeBase* node : currentNode->children)
        {
            killNodes(node);
        }
    }
    delete currentNode;
}

void fixedUpdateAll(Node::NodeBase* currentNode, float deltatime)
{
    /*if (currentNode->isDead)
    {
        killNodes(currentNode);
    }*/
    currentNode->fixedUpdate(deltatime);
    if (currentNode->children.size() != 0)
    {
        for (Node::NodeBase* node : currentNode->children)
        {
            fixedUpdateAll(node, deltatime);
        }
    }
}

void draw3DAll(Node::NodeBase* currentNode, float deltatime)
{
    /*if (currentNode->isSkybox)
    {
        t3d_viewport_set_projection(viewport, T3D_DEG_TO_RAD(75.0f), 500.f, 1000.f);
    }*/
    currentNode->draw3D(deltatime);
    if (currentNode->children.size() != 0)
    {
        for (Node::NodeBase* node : currentNode->children)
        {
            draw3DAll(node, deltatime);
        }
    }
}


//Fixed Update - call all objects fixed update
void FixedUpdate(float deltatime)
{
    joypad_poll();
    //droneCamera->fixedUpdate(deltatime);
    auto btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    if (btn.a) isBasicDebugDisplayOn = !isBasicDebugDisplayOn;
    if (btn.start && isMenu == false) startMenu = !startMenu;

    if (!startMenu)
    {
        fixedUpdateAll(topNode, deltatime);
        if (!isMenu)
        {
            levelTimer += deltatime;
            if (levelTimer >= 3.f && player->isStartMode)
            {
                player->startMode(false);
            }
            Collision::TestAllCollision(&allPhysicsObjects, deltatime);
            player->camera->forceUpdate();
            if (bigSprite->levelEnd)
            {
                T3DVec3 newTarget = bigSprite->getGlobalPosition();
                newTarget.v[1] += 100.f;
                player->camera->setAltTarget(&newTarget);
                player->setViewAngle(3.14159f / 2.f, 0.f);
                player->camera->useAltTarget = true;
                player->startMode(true);
                endTimer += deltatime;
                if (endTimer >= 3.0f)
                {
                    endTimer = 0.f;
                    //isMenu = true;
                    startMenu = false;
                    //loadMenu();
                    switch (currLevel)
                    {
                    case(0):
                        loadLevel1();
                        break;
                    case(1):
                        loadLevel2();
                        break;
                    case(2):
                        loadLevel3();
                        break;
                    case(3):
                        loadLevel4();
                        break;
                    case(4):
                        loadLevel5();
                        break;
                    case(5):
                        loadMenu();
                        break;
                    default:
                        break;
                    }
                    return;
                }
                
            }
        }
        else if (menuNode->playGame != -1)
        {
            isMenu = false;
            switch (menuNode->playGame)
            {
            case 0:
                loadLevel0();
                break;
            case 1:
                loadLevel1();
                break;
            case 2:
                loadLevel2();
                break;
            case 3:
                loadLevel3();
                break;
            case 4:
                loadLevel4();
                break;
            case 5:
                loadLevel5();
                break;
            default:
                break;
            }
        }
        else
        {
            player->camera->forceUpdate();
        }
    }
    else
    {
        player->camera->forceUpdate();
        if (btn.b)
        {
            isMenu = true;
            startMenu = false;
            loadMenu();
        }
    }

    
    
}


//Update - call all entities fixed update
void Update(float deltatime)
{
    currentFB = display_get();
    rdpq_attach(currentFB, display_get_zbuf());
    t3d_viewport_attach(&viewport);
    
    t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(75.0f), 40.f, 5000.0f);

    /**********************************************
    -----------------Update------------------------
    ***********************************************/
    //droneCamera->update(deltatime);
    /*player->update(deltatime);
    boxMain->update(deltatime);
    boxChild1->update(deltatime);
    boxChild2->update(deltatime);*/
    updateAll(topNode, deltatime);

    
 

    t3d_frame_start();
    //rdpq_mode_dithering(DITHER_NONE_NONE);
    //rdpq_mode_antialias(AA_NONE);
    t3d_screen_clear_color(color_from_packed32(0x9ab3ed<<8));//should not hard code this colour
    t3d_screen_clear_depth();

    T3DVec3 lightDir{{0.6f, 0.6f, 0.31f}};
    t3d_vec3_norm(lightDir);//Could be done in init if never changes?
    t3d_light_set_ambient({0xAA, 0x7A, 0x7A, 0x00});
    t3d_light_set_directional(0, color_t{0xFF, 0xFF, 0xFF, 0x00}, lightDir);
    ///T3DMat4 child2TempGlobalTransform = boxChild2->getGlobalTransform();
    ///T3DVec3 pppos = boxChild2->getGlobalPosition();
    ///pppos.v[1] += 25.f;
    T3DVec3 lightDir2{ {-0.6f, 0.6f, -0.31f} };
    t3d_light_set_directional(1, color_t{ 0xfc, 0xba, 0x03, 0x00 }, lightDir2);
    ///t3d_light_set_point(1, color_t{ 0xfc, 0xba, 0x03, 0x00 },pppos, 0.125f, false);

    t3d_light_set_count(1);///

    //Point light stuff
    /*for(uint32_t i=0; i<pointLightCount; ++i) {
    float strength = fabsf(pointLights[i].strength);
    t3d_light_set_point(i+1, pointLights[i].color, pointLights[i].pos, strength, pointLights[i].strength < 0.0f);
    }
    t3d_light_set_count(1 + pointLightCount);
    pointLightCount = 0;*/

    //Normal 3D Drawing
    t3d_matrix_push_pos(1);

        rdpq_set_prim_color({0xFF, 0xFF, 0xFF, 0xFF});
        rdpq_set_env_color({0xFF, 0xFF, 0xFF, 0x00});
        //t3d_light_set_ambient(ambientColor); could set it here I guess, but typically global

        t3dState = t3d_model_state_create();//T3D model states?!? how do these work? NEEDS to be here.

        /**********************************************
        -----------------Draw3D------------------------
        ***********************************************/
        /*player->draw3D(deltatime);
        //sky->draw3D(deltatime);
        skyBoxMesh->draw3D(deltatime);
        boxMain->draw3D(deltatime);
        boxChild1->draw3D(deltatime);
        boxChild2->draw3D(deltatime);*/
        draw3DAll(topNode, deltatime);
        
        /*skyBox->updateTransformFP();
        //t3d_model_draw_material(obj->material, t3dState);//how should we deal with t3dstate? global? global getter for visual nodes?
        t3d_matrix_set(skyBox->transformFP, true);
        rspq_block_run(skyBox->model->userBlock);*/


    t3d_matrix_pop(1);


    //Stuff after (before 2d)!!!!!!!!! Particles, coins!? very interesting, located at bottom of file.
    //2D Draw, lots of cool stuff in Hail's draw code (rpdq_blits, progress bars, debug stuff)
    //  Also cool math stuff, look more into!
    rdpq_sync_pipe();
    rdpq_sync_tile();
    rdpq_set_mode_standard();//what this do?
    rdpq_mode_filter(FILTER_POINT);//Important, didn't use in gamejam!
    rdpq_mode_zbuf(false, false);//Interesting, how to use/disable zbuf more.

    /**********************************************
    -----------------Draw2D------------------------
    ***********************************************/

    

    t3d_debug_print_start();

    if (isBasicDebugDisplayOn)
    {

        T3DVec3 tempCamTransform = player->camera->pos;
        T3DMat4 tempPlayerTransform = player->getGlobalTransform();
        T3DVec3 tempTargetTransform = player->camera->target;
        //T3DMat4 tempGlobalTransform = boxMain->getGlobalTransform();
        //T3DMat4 childTempGlobalTransform = boxChild1->getGlobalTransform();
        //T3DMat4 child2TempGlobalTransform = boxChild2->getGlobalTransform();
        //tempGlobalTransform.m[3][0] += 200.f;
        //tempGlobalTransform.m[3][1] += 0.f;
        //tempGlobalTransform.m[3][2] += 200.f;
        t3d_debug_printf(16, 20, "PlayerPos : %.2f, %.2f, %.2f", tempPlayerTransform.m[3][0], tempPlayerTransform.m[3][1], tempPlayerTransform.m[3][2], tempPlayerTransform.m[3][3]);
        t3d_debug_printf(16, 30, "CamPos : %.2f, %.2f, %.2f", tempCamTransform.v[0], tempCamTransform.v[1], tempCamTransform.v[2]);
        t3d_debug_printf(16, 40, "TargetPos : %.2f, %.2f, %.2f", tempTargetTransform.v[0], tempTargetTransform.v[1], tempTargetTransform.v[2]);

        t3d_debug_printf(16, 50, "boostpercent   : %.2f", player->boostPercent);
        t3d_debug_printf(16, 60, "ammo : %d", player->getAmmoCount());
        /*
        t3d_debug_printf(16, 60, "Mesh[0] : %.2f, %.2f, %.2f", tempGlobalTransform.m[0][0], tempGlobalTransform.m[0][1], tempGlobalTransform.m[0][2], tempGlobalTransform.m[0][3]);
        t3d_debug_printf(16, 70, "Mesh[1] : %.2f, %.2f, %.2f", tempGlobalTransform.m[1][0], tempGlobalTransform.m[1][1], tempGlobalTransform.m[1][2], tempGlobalTransform.m[1][3]);
        t3d_debug_printf(16, 80, "Mesh[2] : %.2f, %.2f, %.2f", tempGlobalTransform.m[2][0], tempGlobalTransform.m[2][1], tempGlobalTransform.m[2][2], tempGlobalTransform.m[2][3]);
        t3d_debug_printf(16, 90, "MeshPos : %.2f, %.2f, %.2f", tempGlobalTransform.m[3][0], tempGlobalTransform.m[3][1], tempGlobalTransform.m[3][2], tempGlobalTransform.m[3][3]);


        t3d_debug_printf(16, 110, "MeshChild[0] : %.2f, %.2f, %.2f", tempGlobalTransform.m[0][0], childTempGlobalTransform.m[0][1], childTempGlobalTransform.m[0][2], childTempGlobalTransform.m[0][3]);
        t3d_debug_printf(16, 120, "MeshChild[1] : %.2f, %.2f, %.2f", childTempGlobalTransform.m[1][0], childTempGlobalTransform.m[1][1], childTempGlobalTransform.m[1][2], childTempGlobalTransform.m[1][3]);
        t3d_debug_printf(16, 130, "MeshChild[2] : %.2f, %.2f, %.2f", childTempGlobalTransform.m[2][0], childTempGlobalTransform.m[2][1], childTempGlobalTransform.m[2][2], childTempGlobalTransform.m[2][3]);
        t3d_debug_printf(16, 140, "MeshChildPos : %.2f, %.2f, %.2f", childTempGlobalTransform.m[3][0], childTempGlobalTransform.m[3][1], childTempGlobalTransform.m[3][2], childTempGlobalTransform.m[3][3]);
        */
        ///t3d_debug_printf(16, 160, "MeshChild2Pos : %.2f, %.2f, %.2f", pppos.v[0], pppos.v[1], pppos.v[2]);
        T3DVec3 horzVel = player->velocity;
        float speed = t3d_vec3_len(&horzVel);
        horzVel.v[1] = 0.f;
        float horzSpeed = t3d_vec3_len(&horzVel);
        t3d_debug_printf(16, 160, "horz speed   : %.2f", horzSpeed);
        t3d_debug_printf(16, 170, "num boxes   : %d", bigSprite->numBoxes);
        t3d_debug_printf(16, 180, "speed   : %.2f", speed);
        t3d_debug_printf(180, 180, "FPS   : %.2f", display_get_fps());
    }
    if (isMenu)
    {
        menuNode->drawMenu();
    }
    else if (currLevel == 0)
    {
        t3d_debug_printf(30, 170, "Use D-pad to move");
        t3d_debug_printf(30, 180, "Use L to jump...");
        t3d_debug_printf(30, 190, "HOLD Z to aim");
        t3d_debug_printf(30, 200, "WHILE aiming, press L to Fire!");
        t3d_debug_printf(30, 210, "Boost: %.0f", player->boostPercent);
        t3d_debug_printf(130, 210, "Ammo: %d", player->getAmmoCount());
    }
    else
    {
        t3d_debug_printf(30, 210, "Boost: %.0f", player->boostPercent);
        t3d_debug_printf(130, 210, "Ammo: %d", player->getAmmoCount());
    }
    if (startMenu)
    {
        t3d_debug_printf(30, 100, "PAUSE");
        t3d_debug_printf(30, 120, "Press START to resume");
        t3d_debug_printf(30, 130, "Press B to return to main menu");
    }

    rdpq_detach_show();
}

//cleanup?





//Main func
int main()
{
    SystemInit();
    display_set_fps_limit(30);
    loadLevel0();
    isMenu = true;
    loadMenu();

    //*************Main Loop************* */
    for(;;)
    {
        float frametime = display_get_delta_time();

        // Perform the update in discrete steps (ticks)
        accumulator += frametime;
        while (accumulator >= dt)
        {
            FixedUpdate(dt);
            accumulator -= dt;
        }
        // Read controler data
        // joypad_poll();
        //mixer_try_play();
        
        // Perform the unfixed loop
        //core_set_subtick(((double)accumulator)/((double)dt));!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        ////FixedUpdate(frametime);
        Update(frametime);
    }
}

/*
  tpx_state_from_t3d();
  tpx_state_set_scale(0.5f, 0.5f);

  for(auto actor : actors) {
    if(actor->drawMask & Actor::DRAW_MASK_PTX) {
      actor->drawPtx(deltaTime);
    }
  }

  rdpq_sync_load();
  ptCoins.draw(deltaTime);

  rdpq_sync_pipe();
  rdpq_set_env_color({0xFF, 0xFF, 0xFF, 0xFF});
  ptSpark.draw(deltaTime); // @TODO: disable blending

  rdpq_sync_pipe();
  rdpq_sync_tile();
  rdpq_sync_load();

  rdpq_set_env_color({0xFF, 0xFF, 0xFF, 0xAA});
  ptSwirl.draw(deltaTime);

  rdpq_sync_pipe();
  rdpq_mode_blender(0);
  rdpq_mode_zbuf(true, true);
  rdpq_set_env_color({0xFF, 0xFF, 0xFF, 0xFF});

  rdpq_mode_pop();
  t3d_matrix_push_pos(1);
  for(auto & player : players) {
    player.drawTransp(deltaTime);
  }
  t3d_matrix_pop(1);
  */

