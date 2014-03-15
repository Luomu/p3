#pragma once
#include "p3/Common.h"
#include "p3/EntitySystem.h"
#include "graphics/Renderer.h"
namespace p3
{
/**
 * Fires lazors
 */
class WeaponSystem : public entityx::System<WeaponSystem> {
public:
	WeaponSystem(Graphics::Renderer* r) : m_renderer(r) {}
	void update(ent_ptr<entityx::EntityManager> es, ent_ptr<entityx::EventManager> events, double alpha) override;

private:
	Graphics::Renderer* m_renderer;
};
}
