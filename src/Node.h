#pragma once
#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <vector>
#include <algorithm>



namespace Node
{
    class NodeBase
    {
    private:

    protected:
        NodeBase* parent;
        
        bool isPhysical;
        /*fm_vec3_t pos{};

        bool checkFrustumAABB(const fm_vec3_t& aabbMin, const fm_vec3_t& aabbMax) const;
        bool checkFrustumSphere(const fm_vec3_t& center, float radius) const;*/

    public:
        bool isSkybox;

        std::vector<Node::NodeBase*> children{};

        //static NodeBase* topNode = new Node::NodeBase();
        //needs to be changed

        //T3DMat4* transform{}; THAT'S RIGHT we're killing transform, gonna do components instead and then just compute for the matrix
        T3DVec3 position;
        T3DVec3 rotation;//euler for now, maybe should be quat?
        T3DVec3 scale;
        T3DMat4FP* transformFP{};
        //Flags?
        bool isDead;

        NodeBase();
        virtual ~NodeBase() {}//Destructor
        virtual void update(float deltaTime);// = 0 would be pure, couldn't instatiate a base node
        virtual void fixedUpdate(float deltaTime) {};// Should we have to pass deltaTime?

        virtual void draw3D(float deltaTime) {};//Done at a seperate time than update
        virtual void drawPTX(float deltaTime) {};//Draws particles, maybe 2d too?

        void addChild(NodeBase* newChild);
        virtual bool setParent(NodeBase* newParent);
        void reparent(NodeBase* newParent);
        NodeBase* getParent() { return parent; }

        virtual void deleteNode();

        T3DMat4 getLocalTransform() //Both of these return copy of transform, not a ref.
        { 
            T3DMat4 tempTransform;
            t3d_mat4_from_srt_euler(&tempTransform,
                scale.v,
                rotation.v,
                position.v
            );
            return tempTransform;
        }

        T3DVec3 getGlobalScale()
        {
            if (parent == NULL)
            {
                return scale;
            }
            else
            {
                return parent->getGlobalScale() * scale;
            }
        }        
        T3DVec3 getGlobalRotation()
        {
            if (parent == NULL)
            {
                return rotation;
            }
            else
            {
                return parent->getGlobalRotation() + rotation;
            }
        }        
        T3DVec3 getGlobalPosition()//DOESN'T WORK
        {
            if (parent == NULL)
            {
                return position;
            }
            else
            {
                return parent->getGlobalPosition() + position;
                //T3DMat4 globalTransform = getGlobalTransform();
                //return { globalTransform.m[3][0], globalTransform.m[3][1], globalTransform.m[3][2] };

            }
        }

        T3DMat4 getGlobalTransform() 
        { 
            if (parent == NULL)
            {
                return getLocalTransform();
            }
            else
            {
                T3DMat4 tempTransform = getLocalTransform();
                T3DMat4 parentTransform = parent->getGlobalTransform();

                T3DMat4 finalMat;
                t3d_mat4_mul(&finalMat, &parentTransform, &tempTransform);
                
                return finalMat;
                
            }
        
        }

        void updateTransformFP()
        {
            T3DMat4 tempGlobalTransform = getGlobalTransform();
            t3d_mat4_to_fixed(transformFP, &tempGlobalTransform);
        }

        //void setLocalTransform(T3DMat4* newTransform) { *transform = *newTransform; }Removing for now
        void setLocalTransformEuler(float scale_x, float scale_y, float scale_z, float rot_x, float rot_y, float rot_z, float pos_x, float pos_y, float pos_z);
        void setLocalTransformEuler(T3DVec3* scl, T3DVec3* rot, T3DVec3* pos);
        void setGlobalTransform(T3DMat4* newTransform) //Not currently used, should it???
        {
            //parent == NULL ?  : transform = parent->getGlobalTransform() * newTransform;//Is this right?
            if (parent == NULL)
            {
                //*transform = *newTransform;
            }
            else
            {
                //T3DMat4* tempMat;
                //T3DMat4 parentMat = parent->getGlobalTransform();
                //t3d_mat4_mul(transform, &parentMat, newTransform);
                //transform = tempMat;
                //return parent->getGlobalTransform() * transform;
            }
        }

    };
}