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

			//laser bolt
			Entity laser = em->create();
			ref_ptr<LaserBoltGraphic> lc(new LaserBoltGraphic(m_renderer));
			laser.assign<GraphicComponent>(lc);
			laser.assign<PosOrientComponent>(ownerPoc->pos, ownerPoc->orient);
			wc->firing = false;
		}
	}
}
}
