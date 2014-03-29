#pragma once
#include "p3/Common.h"
#include "graphics/Renderer.h"
#include "p3/EntitySystem.h"
#include "p3/Graphic.h"
#include "p3/Camera.h"
#include "graphics/Light.h" //possibly NOOOO
#include "p3/CoreComponents.h"

namespace p3
{

/**
 * For each camera, construct a temporary camera frame, then
 * get a frame render transform for each entity. From that, give
 * each Graphic view coords and orientation.
 */
class FrameRenderSystem : public entityx::System<FrameRenderSystem>
{
public:
	FrameRenderSystem(Scene* scene_) : scene(scene_) {}
	virtual void update(ent_ptr<EntityManager> em, ent_ptr<EventManager> events, double dt) override;

	Scene* scene;
};

/**
 * Rendering
 * - cameras, lights and renderable graphics are
 *   registered/unregistered when game entity
 *   components are added/removed
 */
class Scene : public entityx::Receiver<Scene>
{
public:
	enum class RenderBin
	{
	    NORMAL,    //depth sorted
	    BACKGROUND //drawn in order added
	};
	Scene(Graphics::Renderer*, ent_ptr<EntityManager>, ent_ptr<EventManager>);
	void Render();
	void Render(Camera* camera);

	//Register and unregister scene participants
	//Scene doesn't own any of these instances
	void AddLight(Graphics::Light*);
	//void RemoveLight(Graphics::Light*);
	void AddGraphic(Graphic*, RenderBin = RenderBin::NORMAL);
	void RemoveGraphic(Graphic*, RenderBin = RenderBin::NORMAL);

	//only non-background graphics are expected to be in components
	void receive(const entityx::ComponentAddedEvent<GraphicComponent>&);
	void receive(const entityx::ComponentRemovedEvent<GraphicComponent>&);
	void receive(const entityx::EntityDestroyedEvent&);

private:
	Graphics::Renderer* m_renderer;

	std::vector<Graphic*> m_bgGraphics;
	std::vector<Graphic*> m_graphics;
	//std::unordered_set<Graphics::Light*> m_lights;

	//filled & emptied each frame
	std::vector<Graphics::Light*> m_lights;

	ent_ptr<FrameRenderSystem> m_frameRenderSystem;
	ent_ptr<entityx::BaseSystem> m_modelRenderSystem;
	ent_ptr<EntityManager> m_entities;
	ent_ptr<EventManager> m_events;
};
}
