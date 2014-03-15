#include "p3/Scene.h"
#include "p3/CoreComponents.h"
#include "scenegraph/Model.h"
#include "graphics/Light.h"

namespace p3
{

class ModelRenderSystem : public entityx::System<ModelRenderSystem>
{
public:
	virtual void update(ent_ptr<EntityManager> em, ent_ptr<EventManager> events, double dt) override
	{
		//turn interpTransform into something more renderable
		for (auto entity : em->entities_with_components<GraphicComponent, PosOrientComponent>()) {
			ent_ptr<GraphicComponent> gc = entity.component<GraphicComponent>();
			ent_ptr<PosOrientComponent> poc = entity.component<PosOrientComponent>();

			//matrix4x4d modelMatrix = poc->interpOrient;
			//modelMatrix.SetTranslate(poc->interpPos);
			matrix4x4d modelMatrix = poc->orient;
			modelMatrix.SetTranslate(poc->pos);

			for (Uint32 i = 0; i < 16; i++)
				gc->graphic->modelTransform[i] = float(modelMatrix[i]);
		}
	}
};

Scene::Scene(Graphics::Renderer* r, ent_ptr<EntityManager> em, ent_ptr<EventManager> ev)
	: m_renderer(r)
	, m_entities(em)
	, m_events(ev)
{
	m_modelRenderSystem.reset(new ModelRenderSystem());

	ev->subscribe<entityx::ComponentAddedEvent<CameraComponent>>(*this);
	ev->subscribe<entityx::ComponentRemovedEvent<CameraComponent>>(*this);
	ev->subscribe<entityx::ComponentAddedEvent<GraphicComponent>>(*this);
	ev->subscribe<entityx::ComponentRemovedEvent<GraphicComponent>>(*this);
	ev->subscribe<entityx::EntityDestroyedEvent>(*this);
}

void Scene::Render()
{
	//update some transforms in Graphics
	m_modelRenderSystem->update(m_entities, m_events, 0);

	m_renderer->SetAmbientColor(Color(0, 0, 0, 0));

	const Uint32 w = m_renderer->GetWindow()->GetWidth();
	const Uint32 h = m_renderer->GetWindow()->GetHeight();

	std::sort(m_cameras.begin(), m_cameras.end(), [](Camera * a, Camera * b) {
		return a->zOrder < b->zOrder;
	});

	for (auto cam : m_cameras) {
		const float aspect = (cam->viewport.z * w) / (cam->viewport.w * h);
		m_renderer->SetViewport(cam->viewport.x * w, cam->viewport.y * h, cam->viewport.z * w, cam->viewport.w * h);
		m_renderer->SetScissor(true,
		                       vector2f(cam->viewport.x * w, cam->viewport.y * h),
		                       vector2f(cam->viewport.z * w, cam->viewport.w * h));
		m_renderer->SetClearColor(cam->clearColor);
		m_renderer->ClearScreen();
		m_renderer->SetPerspectiveProjection(cam->fovY, aspect, cam->nearZ, cam->farZ);

		m_renderer->SetViewMatrix(cam->viewMatrix);

		for (auto g : m_bgGraphics)
			g->Render();

		m_renderer->EnableFramebufferSRGB(true);

		Graphics::Light dl(Graphics::Light::LIGHT_DIRECTIONAL,
		                   vector3f(0.f, 1.f, 1.f).Normalized(), Color(255), Color(255));
		m_renderer->SetLights(1, &dl);

		for (auto g : m_graphics)
			g->Render();

		m_renderer->EnableFramebufferSRGB(false);
	}

	//restore expectations
	m_renderer->SetViewport(0, 0, w, h);
	m_renderer->SetScissor(true, vector2f(0.f), vector2f(w, h));
}

void Scene::AddCamera(Camera* c)
{
	m_cameras.push_back(c);
}

void Scene::RemoveCamera(Camera* c)
{
	auto it = m_cameras.begin();
	while (it != m_cameras.end()) {
		if (*it == c) {
			it = m_cameras.erase(it);
			continue;
		}
		++it;
	}
}

void Scene::AddLight(Graphics::Light* l)
{
	m_lights.insert(l);
}

void Scene::RemoveLight(Graphics::Light* l)
{
	m_lights.erase(l);
}

void Scene::AddGraphic(Graphic* g, RenderBin bin)
{
	auto gb = &m_graphics;
	if (bin == RenderBin::BACKGROUND)
		gb = &m_bgGraphics;

	gb->push_back(g);
}

void Scene::RemoveGraphic(Graphic* g, RenderBin bin)
{
	auto gb = &m_graphics;
	if (bin == RenderBin::BACKGROUND)
		gb = &m_bgGraphics;

	gb->erase(std::remove_if(gb->begin(), gb->end(), [g](Graphic * x) {
		return x == g;
	}), gb->end());
}

void Scene::receive(const entityx::ComponentAddedEvent<CameraComponent>& ev)
{
	AddCamera(ev.component->camera.get());
}

void Scene::receive(const entityx::ComponentRemovedEvent<CameraComponent>& ev)
{
	RemoveCamera(ev.component->camera.get());
}

void Scene::receive(const entityx::ComponentAddedEvent<GraphicComponent>& ev)
{
	AddGraphic(ev.component->graphic.Get());
}

void Scene::receive(const entityx::ComponentRemovedEvent<GraphicComponent>& ev)
{
	RemoveGraphic(ev.component->graphic.Get());
}

void Scene::receive(const entityx::EntityDestroyedEvent& ev)
{
	//may be faster if a Graphic unregisters itself
	Entity ent = ev.entity;
	ent_ptr<GraphicComponent> gc = ent.component<GraphicComponent>();
	if (gc)
		RemoveGraphic(gc->graphic.Get());
}

}
