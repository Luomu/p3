#pragma once
#include "graphics/Renderer.h"
#include "p3/EntitySystem.h"

namespace p3
{

/**
 * Rendering
 */

/**
 * So, I am uncertain about using systems for rendering
 * but going to try it for a while. Other idea is to have
 * a list of generic Graphic objects registered to Scene.
 */
class Scene
{
public:
	Scene(Graphics::Renderer*, ent_ptr<EntityManager>, ent_ptr<EventManager>);
	void Render();

private:
	Graphics::Renderer* m_renderer;

	ent_ptr<entityx::BaseSystem> m_modelRenderSystem;
	ent_ptr<EntityManager> m_entities;
	ent_ptr<EventManager> m_events;
};
}
