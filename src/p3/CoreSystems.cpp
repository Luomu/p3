#include "p3/CoreSystems.h"
#include "p3/CoreComponents.h"

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
			laser.assign<ProjectileComponent>(ownerDc->vel, ownerPoc->orient * vector3d(0, 0, -100), 3.0);
			laser.assign<FrameComponent>(ownerFc->frame);
			wc->firing = false;
		}
	}
}

void ProjectileSystem::update(ent_ptr<EntityManager> em, ent_ptr<EventManager> events, double dt)
{
	for (auto entity : em->entities_with_components<ProjectileComponent>()) {
		ent_ptr<ProjectileComponent> pc = entity.component<ProjectileComponent>();
		pc->lifetime -= dt;
		if (pc->lifetime < 0)
			entity.destroy();
		else {
			ent_ptr<PosOrientComponent> poc = entity.component<PosOrientComponent>();

			poc->pos += (pc->baseVel + pc->dirVel) * dt;
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
		auto tgtPoc = att->target.component<PosOrientComponent>();
		SDL_assert(tgtPoc);
		poc->pos    = tgtPoc->pos + tgtPoc->orient * att->offset;
		poc->orient = tgtPoc->orient;
	}
}


}
