#include "p3/CoreSystems.h"
#include "p3/CoreComponents.h"
#include "collider/CollisionSpace.h"

namespace p3
{
void WeaponSystem::update(ent_ptr<EntityManager> em, ent_ptr<EventManager> events, double dt)
{
	for (auto entity : em->entities_with_components<WeaponComponent>()) {
		ent_ptr<WeaponComponent> wc = entity.component<WeaponComponent>();
		if (wc->firing) {
			ent_ptr<PosOrientComponent> ownerPoc = entity.component<PosOrientComponent>();
			ent_ptr<DynamicsComponent> ownerDc = entity.component<DynamicsComponent>();
			auto ownerFc = entity.component<FrameComponent>();

			//laser bolt
			Entity laser = em->create();
			laser.assign<GraphicComponent>(new LaserBoltGraphic(m_renderer));
			laser.assign<PosOrientComponent>(ownerPoc->pos, ownerPoc->orient);
			laser.assign<ProjectileComponent>(ownerDc->vel, ownerPoc->orient * vector3d(0, 0, -500), 3.0, entity);
			laser.assign<FrameComponent>(ownerFc->frame);
			wc->firing = false;
		}
	}
}

void ProjectileSystem::update(ent_ptr<EntityManager> em, ent_ptr<EventManager> events, double dt)
{
	for (auto entity : em->entities_with_components<ProjectileComponent, FrameComponent>()) {
		ent_ptr<ProjectileComponent> pc = entity.component<ProjectileComponent>();
		pc->lifetime -= dt;

		if (pc->lifetime < 0) {
			entity.destroy();
			continue;
		}

		ent_ptr<PosOrientComponent> poc = entity.component<PosOrientComponent>();
		const vector3d vel = pc->baseVel + pc->dirVel;
		poc->pos += vel * dt;

		//Collide
		auto pfc = entity.component<FrameComponent>();
		CollisionContact c;
		pfc->frame->GetCollisionSpace()->TraceRay(poc->pos, vel.Normalized(), vel.Length(), &c, 0);

		Entity* collEnt = static_cast<Entity*>(c.userData1);
		if (collEnt && *collEnt != pc->owner) {
			Entity clonk = *collEnt;
			auto gc = clonk.component<CollisionMeshComponent>();
			auto fc = clonk.component<FrameComponent>();
			SDL_assert(gc);
			SDL_assert(fc);
			clonk.destroy();
			entity.destroy();
		}
	}
}

void CollisionSystem::update(ent_ptr<EntityManager> em, ent_ptr<EventManager> events, double dt)
{
	for (auto entity : em->entities_with_components<CollisionMeshComponent, PosOrientComponent>()) {
		auto cmc = entity.component<CollisionMeshComponent>();
		auto poc = entity.component<PosOrientComponent>();

		cmc->geom->MoveTo(poc->orient, poc->pos);
	}
}

void AttachToSystem::update(ent_ptr<entityx::EntityManager> em, ent_ptr<entityx::EventManager> events, double dt)
{
	for (auto entity : em->entities_with_components<AttachToEntityComponent, PosOrientComponent>()) {
		auto att = entity.component<AttachToEntityComponent>();
		auto poc = entity.component<PosOrientComponent>();

		if (!att->target.valid()) {
			entity.remove<AttachToEntityComponent>();
			continue;
		}
		auto tgtPoc = att->target.component<PosOrientComponent>();
		SDL_assert(tgtPoc);
		poc->pos    = tgtPoc->pos + tgtPoc->orient * att->offset;
		poc->orient = tgtPoc->orient;
	}
}


}
