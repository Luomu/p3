#pragma once
#include "p3/Common.h"
#include "p3/EntitySystem.h"
#include "collider/CollisionSpace.h"
#include "pi/Frame.h"
#include "galaxy/StarSystem.h"
#include "graphics/Renderer.h"
#include "p3/SpaceSystems.h"

namespace p3
{

/**
 * Simulation world
 * - has a tree of frames
 * - calls update for frames
 * - creates entities
 */
class Space : public entityx::Receiver<Space>
{
public:
	Space(ent_ptr<EntityManager> em, ent_ptr<EventManager> ev);
	void Update(double gameTime, double deltaTime);

	Frame* GetRootFrame() const { return m_rootFrame.get(); }

	void CreateTestScene(Entity player, double gameTime);

	void receive(const entityx::EntityDestroyedEvent&);

	static vector3d GetPosRelTo(Entity e, Frame* relTo);
	static vector3d GetInterpPosRelTo(Entity e, Frame* relTo);
	static vector3d GetVelRelTo(Entity e, Frame* relTo);

private:
	void CollideFrame(Frame*);
	void GenBody(double time, SystemBody* sbody, Frame* parent);
	Frame* MakeFrameFor(double time, SystemBody* sbody, Entity e, Frame* f);
	void CreateStar(Entity e, SystemBody* sbody);
	void CreatePlanet(Entity e, SystemBody* sbody);

	std::unique_ptr<Frame> m_rootFrame;
	ent_ptr<EntityManager> m_entities;
	ent_ptr<EventManager> m_events;
	RefCountedPtr<StarSystem> m_starSystem;

	Graphics::Renderer* m_renderer;

	//systems
	ent_ptr<FrameUpdateSystem> m_frameUpdateSystem;
};
}
