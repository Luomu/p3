#include "p3/Sim.h"
#include "p3/Game.h"
#include "p3/p3.h"
#include "p3/CoreComponents.h"
#include "p3/Scene.h"

namespace p3 {

Sim::Sim()
	: m_running(true)
{
	m_eventManager.reset(new entityx::EventManager());
	m_entities.reset(new entityx::EntityManager(m_eventManager));

	//ZZZ this seems uncertain
	m_scene = new Scene(p3::game->GetRenderer(), m_entities, m_eventManager);

	m_dynamicsSystem.reset(new DynamicsSystem());

	//init "player"
	auto model = p3::game->GetModelCache()->FindModel("natrix");
	SDL_assert(model);
	Entity player = m_entities->create();
	player.assign<ModelComponent>(model);
}

Sim::~Sim()
{
	delete m_scene;
}

void Sim::Execute(double time)
{
	static double totalElapsed = 0;
	totalElapsed += time;

	//update systems
	m_dynamicsSystem->update(m_entities, m_eventManager, time);

	if (totalElapsed > 30) {
		printf("Ran for %fs\n", totalElapsed);
		m_running = false;
	}
}

void Sim::End()
{
	m_running = false;
}

}
