#pragma once
#include "p3/Common.h"
#include "p3/EntitySystem.h"

namespace p3
{

/**
 * Frame of reference
 * - can have child frames
 * - can be rotating
 * - has a CollisionSpace
 */
class Frame
{
public:
	enum {
		FLAG_ROTATING = (1 << 1),
		FLAG_HAS_ROT = (1 << 2)
	};

	Frame(Frame* parent, const std::string& label, Uint32 flags);
	~Frame();

	const std::string& GetLabel() const { return m_label; }

	void AddChild(Frame* f);

private:
	std::string m_label;
	Frame* m_parent;
	std::vector<Frame*> m_children;
	//SystemBody* m_sbody;
	Uint32 m_flags;
	double m_radius;

	vector3d m_pos;
	vector3d m_oldPos;
	vector3d m_interpPos;

	matrix3x3d m_initialOrient;
	matrix3x3d m_orient;
	matrix3x3d m_interpOrient;

	vector3d m_vel;

	double m_angSpeed;
	double m_oldAngDisplacement;

	vector3d m_rootVel;
	vector3d m_rootPos;
	vector3d m_rootInterpPos;
	matrix3x3d m_rootOrient;
	matrix3x3d m_rootInterpOrient;
};

/**
 * Simulation world
 * - has a tree of frames
 * - calls update for frames
 * - creates entities
 */
class Space
{
public:
	Space(ent_ptr<EntityManager> em);
	void Update(double time);
};
}
