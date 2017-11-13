#include "CNode.hh"

#include "NodeManager.hh"

CNode::CNode(NodeManager *manager) : irr::IReferenceCounted(), visible(false), pos(irr::core::position2d<irr::f32>(0.0, 0.0)), box(irr::core::dimension2d<irr::f32>(0.0, 0.0)), manager(manager), updateFlag(EUF_ANY)
{
	manager->addNode(this);

	manager->grab();
}

CNode::~CNode()
{
	manager->removeNode(this);

	manager->drop();
}
