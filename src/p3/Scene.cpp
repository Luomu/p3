#include "p3/Scene.h"
#include "p3/CoreComponents.h"
#include "scenegraph/Model.h"
#include "graphics/Light.h"
#include "pi/Frame.h"
#include "p3/Game.h"
#include "p3/p3.h"

namespace p3
{

void FrameRenderSystem::update(ent_ptr<EntityManager> em, ent_ptr<EventManager> events, double dt)
{
	for (auto camEntity : em->entities_with_components<CameraComponent, FrameComponent>()) {
		auto camc = camEntity.component<CameraComponent>();
		auto cfc  = camEntity.component<FrameComponent>();
		auto cpoc = camEntity.component<PosOrientComponent>();
		SDL_assert(cpoc);

		//make a temporary camera frame
		std::unique_ptr<Frame> camFrame(new Frame(cfc->frame, "camera", Frame::FLAG_ROTATING));
		camFrame->SetOrient(cpoc->orient, p3::game->GetSim()->GetGameTime());
		camFrame->SetPosition(cpoc->pos);
		//update root relative pos & interpolate
		camFrame->ClearMovement();
		camFrame->UpdateInterpTransform(1.0);

		//set up star light source(s)
		for (auto lightEntity : em->entities_with_components<LightComponent, FrameComponent>()) {
			auto lc  = lightEntity.component<LightComponent>();
            auto lfc = lightEntity.component<FrameComponent>();
            vector3d lpos = lfc->frame->GetPositionRelTo(camFrame.get());
			const double dist = lpos.Length() / AU;
			lpos *= 1.0/dist; // normalize
            lc->light.SetPosition(vector3f(lpos.x, lpos.y, lpos.z));
            scene->AddLight(&lc->light);
		}

		//copy view transform to each graphic
		//they will be sorted later
		for (auto drawEntity : em->entities_with_components<GraphicComponent, FrameComponent>()) {
			auto efc  = drawEntity.component<FrameComponent>();
			auto egc  = drawEntity.component<GraphicComponent>();
			auto epoc = drawEntity.component<PosOrientComponent>();
			SDL_assert(epoc);
			matrix4x4d viewTransform;
			Frame::GetFrameTransform(efc->frame, camFrame.get(), egc->graphic->viewTransform);

			//vector3d viewCoords = viewTransform * epoc->pos; //should be interp pos
		}

		scene->Render(camc->camera.get());

		cfc->frame->RemoveChild(camFrame.get());
	}
}

class ModelRenderSystem : public entityx::System<ModelRenderSystem>
{
public:
	virtual void update(ent_ptr<EntityManager> em, ent_ptr<EventManager> events, double dt) override
	{
		//turn interpTransform into something more renderable
		for (auto entity : em->entities_with_components<GraphicComponent, PosOrientComponent>()) {
			ent_ptr<GraphicComponent> gc = entity.component<GraphicComponent>();
			ent_ptr<PosOrientComponent> poc = entity.component<PosOrientComponent>();

			gc->graphic->modelTransform = poc->orient;
			gc->graphic->modelTransform.SetTranslate(poc->pos);
		}
	}
};

Scene::Scene(Graphics::Renderer* r, ent_ptr<EntityManager> em, ent_ptr<EventManager> ev)
	: m_renderer(r)
	, m_entities(em)
	, m_events(ev)
{
	m_modelRenderSystem.reset(new ModelRenderSystem());
	//ZZZ I think this doesn't have to be here. Just refactor Scene::Render
	//to take one Camera at a time and call from outside.
	m_frameRenderSystem.reset(new FrameRenderSystem(this));

	ev->subscribe<entityx::ComponentAddedEvent<GraphicComponent>>(*this);
	ev->subscribe<entityx::ComponentRemovedEvent<GraphicComponent>>(*this);
	ev->subscribe<entityx::EntityDestroyedEvent>(*this);
}

void Scene::Render()
{
	const Uint32 w = m_renderer->GetWindow()->GetWidth();
	const Uint32 h = m_renderer->GetWindow()->GetHeight();

	//update some transforms in Graphics
	m_modelRenderSystem->update(m_entities, m_events, 0);

	m_renderer->SetAmbientColor(Color(0, 0, 0, 0));

	//this calls Scene::Render(camera)
	m_frameRenderSystem->update(m_entities, m_events, 0);

	//restore expectations
	m_renderer->SetViewport(0, 0, w, h);
	m_renderer->SetScissor(true, vector2f(0.f), vector2f(w, h));
}

void Scene::Render(Camera* cam)
{
	const Uint32 w = m_renderer->GetWindow()->GetWidth();
	const Uint32 h = m_renderer->GetWindow()->GetHeight();

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

	if (!m_lights.empty())
		m_renderer->SetLights(m_lights.size(), m_lights[0]);

	for (auto g : m_graphics)
		g->Render();

	m_lights.clear();
	m_renderer->EnableFramebufferSRGB(false);
}

void Scene::AddLight(Graphics::Light* l)
{
	//m_lights.insert(l);
	m_lights.push_back(l);
}

/*void Scene::RemoveLight(Graphics::Light* l)
{
	m_lights.erase(l);
}*/

void Scene::AddGraphic(Graphic* g, RenderBin bin)
{
	auto gb = &m_graphics;
	if (bin == RenderBin::BACKGROUND)
		gb = &m_bgGraphics;

	//don't add twice
	SDL_assert(gb->end() == std::find(gb->begin(), gb->end(), g));

	gb->push_back(g);
}

void Scene::RemoveGraphic(Graphic* g, RenderBin bin)
{
	auto gb = &m_graphics;
	if (bin == RenderBin::BACKGROUND)
		gb = &m_bgGraphics;

	gb->erase(std::remove_if(gb->begin(), gb->end(), [g](Graphic * x) { return x == g; }), gb->end());
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
