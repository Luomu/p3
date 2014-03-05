#include "p3/Space.h"

namespace p3
{

Frame::Frame(Frame* parent, const std::string& label, Uint32 flags)
	: m_label(label)
	, m_parent(parent)
	, m_flags(flags)
	, m_radius(0.0)
	, m_pos(0.0)
	, m_oldPos(0.0)
	, m_interpPos(0.0)
	, m_initialOrient(1.0)
	, m_orient(1.0)
	, m_interpOrient(1.0)
	, m_vel(0.0)
	, m_angSpeed(0.0)
	, m_oldAngDisplacement(0.0)
	, m_rootVel(0.0)
	, m_rootPos(0.0)
	, m_rootInterpPos(0.0)
	, m_rootOrient(1.0)
	, m_rootInterpOrient(1.0)
{
	if (m_parent) m_parent->AddChild(this);
}

Frame::~Frame()
{
	//delete m_collisionSpace;
	for (Frame* child : m_children)
		delete child;
}

void Frame::AddChild(Frame* f)
{
    m_children.push_back(f);
}

Space::Space(ent_ptr<EntityManager>)
{
}

void Space::Update(double dt)
{

}

}
