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

	m_collSpace.reset(new CollisionSpace()); //ZZZ temp!

	m_dynamicsSystem.reset(new DynamicsSystem());
	m_attachToSystem.reset(new AttachToSystem());
	m_collisionSystem.reset(new CollisionSystem());
	m_projectileSystem.reset(new ProjectileSystem());
	m_inputSystem.reset(new PlayerInputSystem());
	m_thrusterSystem.reset(new ThrusterSystem());
	m_transInterpSystem.reset(new TransInterpSystem());
	m_weaponSystem.reset(new WeaponSystem(renderer));
	m_cameraUpdateSystem.reset(new CameraUpdateSystem());

	//init "player"
	Entity player = m_entities->create();
	{
		auto model = p3::game->GetModelCache()->FindModel("natrix");
		SDL_assert(model);
		player.assign<GraphicComponent>(new ModelGraphic(renderer, model));
		player.assign<PosOrientComponent>();
		player.assign<MassComponent>(10.0);
		player.assign<DynamicsComponent>();
		player.assign<ThrusterComponent>();
		player.assign<WeaponComponent>();
		player.assign<PlayerInputComponent>();
		player.assign<CollisionMeshComponent>(player, model->GetCollisionMesh());

		m_collSpace->AddGeom(player.component<CollisionMeshComponent>()->geom.get());
	}

	//some scenery
	Entity obstacle = m_entities->create();
	{
		auto model = p3::game->GetModelCache()->FindModel("kbuilding02");
		obstacle.assign<PosOrientComponent>(vector3d(0, 0, -200), matrix3x3d(1.0));
		obstacle.assign<GraphicComponent>(new ModelGraphic(renderer, model));
		obstacle.assign<MassComponent>(100.0);
		obstacle.assign<DynamicsComponent>();
		obstacle.assign<CollisionMeshComponent>(obstacle, model->GetCollisionMesh());

		m_collSpace->AddGeom(obstacle.component<CollisionMeshComponent>()->geom.get());

		Entity hangAroundMember = m_entities->create();
		hangAroundMember.assign<PosOrientComponent>(vector3d(0.0), matrix3x3d(1.0));
		hangAroundMember.assign<AttachToEntityComponent>(obstacle, vector3d(0, 50, 0));
		hangAroundMember.assign<GraphicComponent>(new ModelGraphic(renderer, model));
	}

	//init camera
	//left camera
	{
		Entity camera = m_entities->create();
		ent_ptr<CameraComponent> camc(new CameraComponent());
		camc->camera.reset(new Camera());
		camc->camera->clearColor = Color(0, 0, 0, 0);
		camc->camera->viewport = vector4f(0.f, 0.f, 0.5f, 1.f);
		camera.assign(camc);
		camera.assign<PosOrientComponent>(vector3d(0, 50, 100), matrix3x3d(1.0));
		camera.assign<CameraLookAtComponent>(player);
	}
	//right top camera
	{
		Entity camera = m_entities->create();
		ent_ptr<CameraComponent> camc(new CameraComponent());
		camc->camera.reset(new Camera());
		camc->camera->viewport = vector4f(0.5f, 0.5f, 0.5f, 0.5f);
		camera.assign(camc);
		camera.assign<PosOrientComponent>(vector3d(0, -10, 50), matrix3x3d(1.0));
	}
	//right bottom camera
	{
		Entity camera = m_entities->create();
		ent_ptr<CameraComponent> camc(new CameraComponent());
		camc->camera.reset(new Camera());
		camc->camera->clearColor = Color(10, 10, 10, 0);
		camc->camera->viewport = vector4f(0.5f, 0.f, 0.5f, 0.5f);
		camera.assign(camc);
		camera.assign<PosOrientComponent>(vector3d(0, 0, 0), matrix3x3d(1.0));
		camera.assign<ViewFromEntityComponent>(player);
	}
}

Sim::~Sim()
{
	delete m_starfield;
	delete m_scene;
}

//ZZZ unpleasant
static void hitCallback(CollisionContact* c)
{
	auto& ent1 = *static_cast<Entity*>(c->userData1);
	auto& ent2 = *static_cast<Entity*>(c->userData2);

	auto dc1 = ent1.component<DynamicsComponent>();
	auto dc2 = ent2.component<DynamicsComponent>();
	auto poc1 = ent1.component<PosOrientComponent>();
	auto poc2 = ent2.component<PosOrientComponent>();
	auto mc1 = ent1.component<MassComponent>();
	auto mc2 = ent2.component<MassComponent>();

	const double coeff_rest = 0.5;

	const double invMass1 = 1.0 / mc1->mass;
	const double invMass2 = 1.0 / mc2->mass;
	const vector3d hitPos1 = c->pos - poc1->pos;
	const vector3d hitPos2 = c->pos - poc2->pos;
	const vector3d hitVel1 = dc1->vel + dc1->angVel.Cross(hitPos1);
	const vector3d hitVel2 = dc2->vel + dc2->angVel.Cross(hitPos2);
	const double relVel = (hitVel1 - hitVel2).Dot(c->normal);

	// moving away so no collision
	if (relVel > 0) return;

	const double invAngInert1 = 1.0 / dc1->angInertia;
	const double invAngInert2 = 1.0 / dc2->angInertia;
	const double numerator = -(1.0 + coeff_rest) * relVel;
	const double term1 = invMass1;
	const double term2 = invMass2;
	const double term3 = c->normal.Dot((hitPos1.Cross(c->normal) * invAngInert1).Cross(hitPos1));
	const double term4 = c->normal.Dot((hitPos2.Cross(c->normal) * invAngInert2).Cross(hitPos2));

	const double j = numerator / (term1 + term2 + term3 + term4);
	const vector3d force = j * c->normal;

	dc1->vel += force * invMass1;
	dc1->angVel += hitPos1.Cross(force) * invAngInert1;
	dc2->vel -= force * invMass2;
	dc2->angVel -= hitPos2.Cross(force) * invAngInert2;
}

void Sim::Execute(double time)
{
	static double totalElapsed = 0;
	totalElapsed += time;

	//update systems
	m_inputSystem->update(m_entities, m_eventManager, time);
	m_thrusterSystem->update(m_entities, m_eventManager, time);
	m_weaponSystem->update(m_entities, m_eventManager, time);
	m_dynamicsSystem->update(m_entities, m_eventManager, time);
	m_attachToSystem->update(m_entities, m_eventManager, time);
	m_projectileSystem->update(m_entities, m_eventManager, time);
	m_collisionSystem->update(m_entities, m_eventManager, time);
	m_collSpace->Collide(&hitCallback);
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
