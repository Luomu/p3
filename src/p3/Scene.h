#pragma once
#include "p3/Common.h"
#include "graphics/Renderer.h"
#include "p3/EntitySystem.h"
#include "p3/Graphic.h"
#include "p3/Camera.h"
#include "graphics/Light.h" //possibly NOOOO

namespace p3
{

/**
 * Rendering
 * - cameras, lights and renderable graphics are
 *   registered/unregistered when game entity
 *   components are added/removed
 */
class Scene
{
public:
	enum class RenderBin
	{
	    NORMAL,    //depth sorted
	    BACKGROUND //drawn in order added
	};
	Scene(Graphics::Renderer*, ent_ptr<EntityManager>, ent_ptr<EventManager>);
	void Render();

	//Register and unregister scene participants
	//Scene doesn't own any of these instances
	void AddCamera(Camera*); //sorted by priority
	void RemoveCamera(Camera*);
	void AddLight(Graphics::Light*);
	void RemoveLight(Graphics::Light*);
	void AddGraphic(Graphic*, RenderBin);
	void RemoveGraphic(Graphic*, RenderBin);

private:
	Graphics::Renderer* m_renderer;

	std::vector<Graphic*> m_bgGraphics;
	std::vector<Graphic*> m_graphics;
	std::vector<Camera*> m_cameras;
	std::unordered_set<Graphics::Light*> m_lights;

	ent_ptr<entityx::BaseSystem> m_modelRenderSystem;
	ent_ptr<EntityManager> m_entities;
	ent_ptr<EventManager> m_events;
};
}
