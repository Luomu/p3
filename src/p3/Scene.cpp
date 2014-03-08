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
		for (auto entity : em->entities_with_components<ModelComponent, PosOrientComponent>()) {
			ent_ptr<ModelComponent> mc = entity.component<ModelComponent>();
			ent_ptr<PosOrientComponent> poc = entity.component<PosOrientComponent>();

			matrix4x4f viewMatrix(1.f);
			viewMatrix.SetTranslate(vector3f(0.f, 0.f, -100.f));

			matrix4x4d modelMatrix = poc->orient;
			modelMatrix.SetTranslate(poc->pos);

			matrix4x4f modelMatrixf;
			for (Uint32 i = 0; i < 16; i++)
				modelMatrixf[i] = float(modelMatrix[i]);

			mc->model->Render(viewMatrix * modelMatrixf);
		}
	}
};

Scene::Scene(Graphics::Renderer* r, ent_ptr<EntityManager> em, ent_ptr<EventManager> ev)
	: m_renderer(r)
	, m_entities(em)
	, m_events(ev)
{
	m_modelRenderSystem.reset(new ModelRenderSystem());
}

void Scene::Render()
{
	const Uint32 w = m_renderer->GetWindow()->GetWidth();
	const Uint32 h = m_renderer->GetWindow()->GetHeight();
	m_renderer->SetViewport(0, 0, w, h);
	m_renderer->SetPerspectiveProjection(45.f, float(w)/h, 0.1f, 1000.f);
	m_renderer->SetAmbientColor(Color(0,0,0,0));
	m_renderer->SetClearColor(Color(12, 12, 93, 0));
	m_renderer->ClearScreen();

	Graphics::Light dl(Graphics::Light::LIGHT_DIRECTIONAL,
	                   vector3f(0.f, 1.f, 0.f).Normalized(), Color(255), Color(255));
	m_renderer->SetLights(1, &dl);

	m_modelRenderSystem->update(m_entities, m_events, 0);
}

}
