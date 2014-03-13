#include "p3/Sim.h"
#include "p3/Game.h"
#include "p3/p3.h"
#include "p3/CoreComponents.h"
#include "p3/Camera.h"
#include "p3/Scene.h"

namespace p3 {

Sim::Sim()
	: m_running(true)
{
	m_eventManager.reset(new entityx::EventManager());
	m_entities.reset(new entityx::EntityManager(m_eventManager));

	//ZZZ this seems uncertain
	auto renderer = p3::game->GetRenderer();
	m_scene = new Scene(renderer, m_entities, m_eventManager);

	m_starfield = new StarfieldGraphic(renderer, p3::game->GetRNG());
	m_scene->AddGraphic(m_starfield, Scene::RenderBin::BACKGROUND);

	m_dynamicsSystem.reset(new DynamicsSystem());
	m_inputSystem.reset(new PlayerInputSystem());
	m_thrusterSystem.reset(new ThrusterSystem());
	m_transInterpSystem.reset(new TransInterpSystem());
	m_cameraUpdateSystem.reset(new CameraUpdateSystem());

	//init "player"
	auto model = p3::game->GetModelCache()->FindModel("natrix");
	SDL_assert(model);
	Entity player = m_entities->create();
	ref_ptr<ModelGraphic> mc(new ModelGraphic(renderer, model));
	player.assign<GraphicComponent>(mc);
	player.assign<PosOrientComponent>();
	player.assign<MassComponent>(10.0);
	player.assign<DynamicsComponent>();
	player.assign<ThrusterComponent>();
	player.assign<PlayerInputComponent>();

	//init camera
	//left camera
	{
		Entity camera = m_entities->create();
		ent_ptr<CameraComponent> camc(new CameraComponent());
		camc->camera.reset(new Camera());
		camc->camera->clearColor = Color(0,0,0,0);
		camc->camera->viewport = vector4f(0.f, 0.f, 0.5f, 1.f);
		camera.assign(camc);
		camera.assign<PosOrientComponent>(vector3d(0,50,100), matrix3x3d(1.0));
		camera.assign<CameraLookAtComponent>(player);
	}
	//right top camera
	{
		Entity camera = m_entities->create();
		ent_ptr<CameraComponent> camc(new CameraComponent());
		camc->camera.reset(new Camera());
		camc->camera->viewport = vector4f(0.5f, 0.5f, 0.5f, 0.5f);
		camera.assign(camc);
		camera.assign<PosOrientComponent>(vector3d(0,-10,50), matrix3x3d(1.0));
	}
	//right bottom camera
	{
		Entity camera = m_entities->create();
		ent_ptr<CameraComponent> camc(new CameraComponent());
		camc->camera.reset(new Camera());
		camc->camera->clearColor = Color(10,10,10,0);
		camc->camera->viewport = vector4f(0.5f, 0.f, 0.5f, 0.5f);
		camera.assign(camc);
		camera.assign<PosOrientComponent>(vector3d(0,0,0), matrix3x3d(1.0));
		camera.assign<AttachToEntityComponent>(player);
	}
}

Sim::~Sim()
{
	delete m_starfield;
	delete m_scene;
}

void Sim::Execute(double time)
{
	static double totalElapsed = 0;
	totalElapsed += time;

	//update systems
	m_inputSystem->update(m_entities, m_eventManager, time);
	m_thrusterSystem->update(m_entities, m_eventManager, time);
	m_dynamicsSystem->update(m_entities, m_eventManager, time);
	m_cameraUpdateSystem->update(m_entities, m_eventManager, time);

	if (totalElapsed > 30) {
		printf("Ran for %fs\n", totalElapsed);
		m_running = false;
	}
}

void Sim::End()
{
	m_running = false;
}

void Sim::InterpolatePositions(double gameTickAlpha)
{
	//update bodies
    m_transInterpSystem->update(m_entities, m_eventManager, gameTickAlpha);
    //update frames
}

}
