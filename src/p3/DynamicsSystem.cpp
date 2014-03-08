#include "p3/DynamicsSystem.h"
#include "p3/CoreComponents.h"

namespace p3
{

void DynamicsSystem::update(ent_ptr<EntityManager> em, ent_ptr<EventManager> events, double dt)
{
	for (auto entity : em->entities_with_components<DynamicsComponent>()) {
        ent_ptr<DynamicsComponent> dc = entity.component<DynamicsComponent>();
        ent_ptr<PosOrientComponent> poc = entity.component<PosOrientComponent>();
        ent_ptr<MassComponent> mc = entity.component<MassComponent>();
        SDL_assert(poc);
        SDL_assert(mc);

		dc->vel += dt * dc->force * (1.0 / mc->mass);

		poc->pos += dc->vel * dt;
	}
}

}
