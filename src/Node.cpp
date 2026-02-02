#include "Node.h"
//#include "../main.h"
//#include "../scene/scene.h"

/*bool Actor::Base::checkFrustumAABB(const fm_vec3_t& aabbMin, const fm_vec3_t& aabbMax) const
{
	auto& fr = state.activeScene->getCam().getFrustum();
	return t3d_frustum_vs_aabb(&fr, &aabbMin, &aabbMax);
}

bool Actor::Base::checkFrustumSphere(const fm_vec3_t& center, float radius) const
{
	auto& fr = state.activeScene->getCam().getFrustum();
	return t3d_frustum_vs_sphere(&fr, &center, radius);
}*/


namespace Node
{
	NodeBase::NodeBase()
	{
		isSkybox = false;
		/*if (NodeBase::topNode == NULL)
		{
			NodeBase::topNode = new Node::NodeBase();
		}*/

		position = (T3DVec3){ 0.f, 0.f, 0.f };
		rotation = (T3DVec3){ 0.f, 0.f, 0.f };
		scale = (T3DVec3){ 1.f, 1.f, 1.f };
		transformFP = static_cast<T3DMat4FP*>(malloc_uncached(sizeof(T3DMat4FP)));

		parent = NULL;
		isDead = false;
	}

	void NodeBase::addChild(NodeBase* newChild)
	{
		if (newChild->setParent(this))
		{
			children.push_back(newChild);
		}
	}

	bool NodeBase::setParent(NodeBase* newParent)
	{
		if (parent && parent == newParent)
		{
			return false;
		}
		parent = newParent;
		return true;
		//parent->addChild(this);
	}

	void NodeBase::reparent(NodeBase* newParent)
	{

	}

	void NodeBase::update(float deltaTime)
	{
		
	}

	void NodeBase::setLocalTransformEuler(float scale_x, float scale_y, float scale_z, float rot_x, float rot_y, float rot_z, float pos_x, float pos_y, float pos_z)
	{
		position = (T3DVec3){ pos_x, pos_y, pos_z };
		rotation = (T3DVec3){ rot_x, rot_y, rot_z };
		scale = (T3DVec3){ scale_x, scale_y, scale_z };
	}
	void NodeBase::setLocalTransformEuler(T3DVec3* scl, T3DVec3* rot, T3DVec3* pos)
	{
		T3DMat4 tempTransform = getLocalTransform();
		t3d_mat4_from_srt_euler(&tempTransform,
			scl->v,
			rot->v,
			pos->v
		);
	}

	void NodeBase::deleteNode()
	{
		isDead = true;
		if (parent)
		{
			auto it = std::find(parent->children.begin(), parent->children.end(), this);
			if (it != parent->children.end()) {
				parent->children.erase(it);
				debugf("Erased!!!......\n");
			}
		}
		for (NodeBase* child : children)
		{
			child->deleteNode();
		}
	}
}

