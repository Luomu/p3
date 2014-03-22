#include "p3/Sim.h"
#include "p3/Game.h"
#include "p3/p3.h"
#include "p3/CoreComponents.h"
#include "p3/Camera.h"
#include "p3/Scene.h"

namespace p3
{

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
	m_attachToSystem.reset(new AttachToSystem());
	m_collisionSystem.reset(new CollisionSystem());
	m_projectileSystem.reset(new ProjectileSystem());
	m_inputSystem.reset(new PlayerInputSystem());
	m_thrusterSystem.reset(new ThrusterSystem());
	m_transInterpSystem.reset(new TransInterpSystem());
	m_weaponSystem.reset(new WeaponSystem(renderer));
	m_cameraUpdateSystem.reset(new CameraUpdateSystem());
	m_shipAISystem.reset(new ShipAISystem(this));

	m_space.reset(new Space(m_entities, m_eventManager));

	Entity player = m_entities->create();
	m_space->CreateTestScene(player, GetGameTime());

	m_hud.reset(new Hud(p3::game->GetUI(), player));
	//init HUD
	m_speedLines.reset(new SpeedLines(renderer, player));
	GetScene()->AddGraphic(m_speedLines.get());
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
	m_shipAISystem->update(m_entities, m_eventManager, time);
	m_thrusterSystem->update(m_entities, m_eventManager, time);
	m_weaponSystem->update(m_entities, m_eventManager, time);
	m_dynamicsSystem->update(m_entities, m_eventManager, time);
	m_attachToSystem->update(m_entities, m_eventManager, time);
	m_projectileSystem->update(m_entities, m_eventManager, time);
	m_collisionSystem->update(m_entities, m_eventManager, time);
	m_space->Update(GetGameTime(), time);
	m_speedLines->Update(time);
	m_hud->Update(time);
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
