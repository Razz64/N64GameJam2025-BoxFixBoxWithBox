#include "SceneTree.h"
//#include "../main.h"
//#include <t3d/tpx.h>

void SceneTree::update(float deltaTime)
{
    //Travel through tree, updating each one
        //root->1->a,b->z,c |->2->a->z,y

    //camera.update(deltaTime);
    //camera.attach();

    
}

void SceneTree::draw(float deltaTime)
{


    //t3d_matrix_push_pos(1);
        //Travel through tree, Draw3d each one
    //t3d_matrix_pop(1);

    // particles
    /*rdpq_sync_pipe();

    rdpq_mode_begin();
    rdpq_mode_zbuf(true, true);
    rdpq_mode_zoverride(true, 0, 0);
    rdpq_mode_alphacompare(20);
    rdpq_mode_persp(false);
    rdpq_mode_filter(FILTER_BILINEAR);
    rdpq_mode_combiner(RDPQ_COMBINER1((0, 0, 0, PRIM), (0, 0, 0, 1)));
    rdpq_mode_end();

    tpx_state_from_t3d();
    tpx_state_set_scale(0.5f, 0.5f);*/

    //for (auto actor : actors) {
        //Travel through tree, draw particles for each one
    //}

    //Lastly, Travel through tree, Draw2D each one
}