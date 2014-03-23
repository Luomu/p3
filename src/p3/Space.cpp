#include "p3/Space.h"
#include "pi/Lang.h"
#include "p3/p3.h"
#include "p3/Game.h"
#include "p3/CoreComponents.h"
#include "galaxy/Sector.h"

namespace p3
{

Space::Space(ent_ptr<EntityManager> em, ent_ptr<EventManager> ev)
	: m_entities(em)
{
	ev->subscribe<entityx::EntityDestroyedEvent>(*this);

	m_rootFrame.reset(new Frame(0, Lang::SYSTEM));
	m_rootFrame->SetRadius(FLT_MAX);
}

void Space::Update(double gameTime, double deltaTime)
{
	//update frame collision
	CollideFrame(m_rootFrame.get());

	//update frames
	//run static update
	m_rootFrame->UpdateOrbitRails(gameTime, deltaTime);
	//run time step updates
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

void Space::CollideFrame(Frame* f)
{
	f->GetCollisionSpace()->Collide(&hitCallback);
	for (auto child : f->GetChildren())
		CollideFrame(child);
}

void Space::receive(const entityx::EntityDestroyedEvent& ev)
{
	Entity ent = ev.entity;
	auto fc = ent.component<FrameComponent>();
	auto cc = ent.component<CollisionMeshComponent>();
	if (fc && cc) {
		fc->frame->GetCollisionSpace()->RemoveGeom(cc->geom.get());
	}
}

void Space::GenBody(double time, SystemBody* sbody, Frame* f)
{
	Entity e = m_entities->create();

	if (sbody->GetType() != SystemBody::TYPE_GRAVPOINT) {
		if (sbody->GetSuperType() == SystemBody::SUPERTYPE_STAR) {
			CreateStar(e, sbody);
		} else if ((sbody->GetType() == SystemBody::TYPE_STARPORT_ORBITAL) ||
		           (sbody->GetType() == SystemBody::TYPE_STARPORT_SURFACE)) {
			//SpaceStation *ss = new SpaceStation(sbody);
			//b = ss;
			return;
		} else {
			CreatePlanet(e, sbody);
		}
		e.assign<NameComponent>(sbody->GetName());
		e.assign<PosOrientComponent>(vector3d(0.0), matrix3x3d(1.0));
	}
	f = MakeFrameFor(time, sbody, e, f);

	for (SystemBody* child : sbody->GetChildren())
		GenBody(time, child, f);
}

Frame* Space::MakeFrameFor(double time, SystemBody* sbody, Entity e, Frame* f)
{
	if (!sbody->GetParent()) {
		e.assign<FrameComponent>(f);
		f->SetBodies(sbody, 0);
		return f;
	}

	if (sbody->GetType() == SystemBody::TYPE_GRAVPOINT) {
		Frame* orbFrame = new Frame(f, sbody->GetName().c_str());
		orbFrame->SetBodies(sbody, 0);
		orbFrame->SetRadius(sbody->GetMaxChildOrbitalDistance() * 1.1);
		return orbFrame;
	}

	const SystemBody::BodySuperType supertype = sbody->GetSuperType();

	if ((supertype == SystemBody::SUPERTYPE_GAS_GIANT) ||
	        (supertype == SystemBody::SUPERTYPE_ROCKY_PLANET)) {
		// for planets we want an non-rotating frame for a few radii
		// and a rotating frame with no radius to contain attached objects
		double frameRadius = std::max(4.0 * sbody->GetRadius(), sbody->GetMaxChildOrbitalDistance() * 1.05);
		Frame* orbFrame = new Frame(f, sbody->GetName().c_str(), Frame::FLAG_HAS_ROT);

		orbFrame->SetBodies(sbody, 0);
		orbFrame->SetRadius(frameRadius);

		assert(sbody->IsRotating() != 0);
		Frame* rotFrame = new Frame(orbFrame, sbody->GetName().c_str(), Frame::FLAG_ROTATING);
		rotFrame->SetBodies(sbody, 0);

		// rotating frame has atmosphere radius or feature height, whichever is larger
		rotFrame->SetRadius(frameRadius * 0.5); //ZZZ should be body->physRadius

		matrix3x3d rotMatrix = matrix3x3d::RotateX(sbody->GetAxialTilt());
		double angSpeed = 2.0 * M_PI / sbody->GetRotationPeriod();
		rotFrame->SetAngSpeed(angSpeed);

		if (sbody->HasRotationPhase())
			rotMatrix = rotMatrix * matrix3x3d::RotateY(sbody->GetRotationPhaseAtStart());
		rotFrame->SetInitialOrient(rotMatrix, time);

		e.assign<FrameComponent>(rotFrame);
		return orbFrame;
	} else if (supertype == SystemBody::SUPERTYPE_STAR) {
		// stars want a single small non-rotating frame
		// bigger than it's furtherest orbiting body.
		// if there are no orbiting bodies use a frame of several radii.
		Frame* orbFrame = new Frame(f, sbody->GetName().c_str());
		orbFrame->SetBodies(sbody, 0);
		double frameRadius = std::max(10.0 * sbody->GetRadius(), sbody->GetMaxChildOrbitalDistance() * 1.1);
		// Respect the frame of other stars in the multi-star system. We still make sure that the frame ends outside
		// the body. For a minimum separation of 1.236 radii, nothing will overlap (see StarSystem::StarSystem()).
		if (sbody->GetParent() && frameRadius > AU * 0.11 * sbody->GetOrbMin())
			frameRadius = std::max(1.1 * sbody->GetRadius(), AU * 0.11 * sbody->GetOrbMin());
		orbFrame->SetRadius(frameRadius);
		e.assign<FrameComponent>(orbFrame);
		return orbFrame;
	} else {
		SDL_assert(false);
	}
}

void Space::CreateStar(Entity e, SystemBody* sbody)
{
	e.assign<GraphicComponent>(new LaserBoltGraphic(m_renderer));
	e.assign<ColorComponent>(Color(255,255,0,255));
}

void Space::CreatePlanet(Entity e, SystemBody* sbody)
{
	e.assign<GraphicComponent>(new LaserBoltGraphic(m_renderer));
	e.assign<ColorComponent>(Color(0,255,0,255));
}

void Space::CreateTestScene(Entity player, double time)
{
	auto renderer = p3::game->GetRenderer();
	m_renderer = renderer;

	StarSystemCache::ShrinkCache(SystemPath(), true);
	Sector::cache.ClearCache();
	SystemPath path(10,0,0,0);
	m_starSystem = StarSystemCache::GetCached(path);

	GenBody(time, m_starSystem->m_rootBody.Get(), m_rootFrame.get());
	m_rootFrame->UpdateOrbitRails(time, 1.0);

	//init "player"
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
		player.assign<FrameComponent>(GetRootFrame());
		player.assign<ColorComponent>(Color(0,255,255,255));
		player.assign<ShipAIComponent>();

		GetRootFrame()->GetCollisionSpace()->AddGeom(player.component<CollisionMeshComponent>()->geom.get());
	}

	//some scenery
	//Entity obstacle = m_entities->create();
	{
		Entity obstacle = m_entities->create();
		auto model = p3::game->GetModelCache()->FindModel("kbuilding02");
		obstacle.assign<PosOrientComponent>(vector3d(0, 0, -200), matrix3x3d(1.0));
		obstacle.assign<GraphicComponent>(new ModelGraphic(renderer, model));
		obstacle.assign<MassComponent>(100.0);
		obstacle.assign<DynamicsComponent>();
		obstacle.assign<CollisionMeshComponent>(obstacle, model->GetCollisionMesh());
		obstacle.assign<FrameComponent>(GetRootFrame());

		GetRootFrame()->GetCollisionSpace()->AddGeom(obstacle.component<CollisionMeshComponent>()->geom.get());

		Entity hangAroundMember = m_entities->create();
		hangAroundMember.assign<PosOrientComponent>(vector3d(0.0), matrix3x3d(1.0));
		hangAroundMember.assign<AttachToEntityComponent>(obstacle, vector3d(0, 50, 0));
		hangAroundMember.assign<GraphicComponent>(new ModelGraphic(renderer, model));
		hangAroundMember.assign<FrameComponent>(GetRootFrame());
	}

	//init camera
	//left camera
	{
		Entity camera = m_entities->create();
		ent_ptr<CameraComponent> camc(new CameraComponent());
		camc->camera.reset(new Camera());
		camc->camera->viewport = vector4f(0.f, 0.f, 0.5f, 1.f);
		camc->camera->clearColor = Color(0, 40, 0, 0);
		camera.assign(camc);
		camera.assign<PosOrientComponent>(vector3d(0, 50, 100), matrix3x3d(1.0));
		camera.assign<CameraLookAtComponent>(player);
		camera.assign<FrameComponent>(GetRootFrame());
		//camera.assign<AttachToEntityComponent>(player, vector3d(0, 5, 50));
	}

	//right top camera
	{
		Entity camera = m_entities->create();
		ent_ptr<CameraComponent> camc(new CameraComponent());
		camc->camera.reset(new Camera());
		camc->camera->viewport = vector4f(0.5f, 0.5f, 0.5f, 0.5f);
		camera.assign(camc);
		camera.assign<PosOrientComponent>(vector3d(100, -10, -10), matrix3x3d(1.0));
		//camera.assign<CameraLookAtComponent>(obstacle);
		camera.assign<FrameComponent>(GetRootFrame());
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
		camera.assign<AttachToEntityComponent>(player, vector3d(0, 5, 10));
		camera.assign<FrameComponent>(GetRootFrame());
	}
}

vector3d Space::GetInterpPosRelTo(Entity e, Frame* relTo)
{
	auto fc  = e.component<FrameComponent>();
	auto poc = e.component<PosOrientComponent>();
	vector3d fpos = fc->frame->GetInterpPositionRelTo(relTo);
	matrix3x3d forient = fc->frame->GetInterpOrientRelTo(relTo);
	return forient * poc->pos + fpos; //ZZZ not actually interp
}

}
