#pragma once
#include "p3/Common.h"
#include "p3/EntitySystem.h"
#include "p3/DynamicsSystem.h"
#include "p3/PlayerInputSystem.h"
#include "p3/StarfieldGraphic.h"
#include "p3/Camera.h"
#include "p3/CoreSystems.h"
#include "collider/CollisionSpace.h"
#include "p3/SpeedLines.h"
#include "p3/Space.h"
#include "p3/ShipAISystem.h"
#include "p3/Hud.h"

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
	double GetGameTime() const { return m_time; }

	void SetTimeAccelRate(double d) { m_timeAccelRate = d; }

	Scene* GetScene() const { return m_scene; } //ZZZ should not be here?

private:
	bool m_running;
	double m_time; //game time, seconds since epoch
	double m_timeAccelRate;
	ent_ptr<entityx::EntityManager> m_entities;
	ent_ptr<entityx::EventManager> m_eventManager;

	std::unique_ptr<Space> m_space;
	std::unique_ptr<Hud> m_hud;
	Scene* m_scene;

	//systems
	ent_ptr<DynamicsSystem> m_dynamicsSystem;
	ent_ptr<AttachToSystem> m_attachToSystem;
	ent_ptr<CollisionSystem> m_collisionSystem;
	ent_ptr<ProjectileSystem> m_projectileSystem;
	ent_ptr<PlayerInputSystem> m_inputSystem;
	ent_ptr<ThrusterSystem> m_thrusterSystem;
	ent_ptr<WeaponSystem> m_weaponSystem;
	ent_ptr<AICommandSystem> m_aiCommandSystem;
	ent_ptr<ShipAISystem> m_shipAISystem;

	//updated when?
	ent_ptr<CameraUpdateSystem> m_cameraUpdateSystem;

	//run between sim::execute and scene::render
	ent_ptr<TransInterpSystem> m_transInterpSystem;

	//some gfx
	StarfieldGraphic* m_starfield;
	//hud gfx, does not belong here
	std::unique_ptr<SpeedLines> m_speedLines;
};
}
