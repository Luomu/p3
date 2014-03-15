#pragma once
#include "p3/Common.h"
#include "p3/EntitySystem.h"
#include "p3/DynamicsSystem.h"
#include "p3/PlayerInputSystem.h"
#include "p3/StarfieldGraphic.h"
#include "p3/Camera.h"
#include "p3/CoreSystems.h"

namespace p3 {

class Scene;

/**
 * Main simulation (game instance)
 *  - has a Space instance (system player is in)
 *  - may run galaxy simulation
 */
class Sim {
public:
	Sim();
	~Sim();
	void Execute(double time);
	void End();
	void InterpolatePositions(double gameTickAlpha);

	bool IsRunning() const { return m_running; }
	double GetTimeAccelRate() const { return 1.0; }
	double GetTimeStep() const { return GetTimeAccelRate() * 1.0/60.0; }

	Scene* GetScene() const { return m_scene; } //ZZZ should not be here?

private:
	bool m_running;
	ent_ptr<entityx::EntityManager> m_entities;
	ent_ptr<entityx::EventManager> m_eventManager;

	Scene* m_scene;

	//systems
	ent_ptr<DynamicsSystem> m_dynamicsSystem;
	ent_ptr<PlayerInputSystem> m_inputSystem;
	ent_ptr<ThrusterSystem> m_thrusterSystem;
	ent_ptr<WeaponSystem> m_weaponSystem;

	//updated when?
	ent_ptr<CameraUpdateSystem> m_cameraUpdateSystem;

	//run between sim::execute and scene::render
	ent_ptr<TransInterpSystem> m_transInterpSystem;

	//some gfx
	StarfieldGraphic* m_starfield;
};
}
