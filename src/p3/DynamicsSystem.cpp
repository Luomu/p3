#include "p3/DynamicsSystem.h"
#include "p3/DynamicsComponent.h"

namespace p3
{

void DynamicsSystem::update(ent_ptr<EntityManager> em, ent_ptr<EventManager> events, double dt)
{
	for (auto entity : em->entities_with_components<DynamicsComponent>()) {
        ent_ptr<DynamicsComponent> dc = entity.component<DynamicsComponent>();
	}
}

}
