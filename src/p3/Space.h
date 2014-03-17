#pragma once
#include "p3/Common.h"
#include "p3/EntitySystem.h"
#include "collider/CollisionSpace.h"
#include "pi/Frame.h"

namespace p3
{

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
	void Update(double gameTime, double deltaTime);

	Frame *GetRootFrame() const { return m_rootFrame.get(); }

	void CreateTestScene(Entity player);

private:
	void CollideFrame(Frame*);

	std::unique_ptr<Frame> m_rootFrame;
	ent_ptr<EntityManager> m_entities;
};
}
