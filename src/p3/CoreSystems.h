#pragma once
#include "p3/Common.h"
#include "p3/EntitySystem.h"
#include "graphics/Renderer.h"
namespace p3
{
/**
 * Fires lazors
 */
class WeaponSystem : public entityx::System<WeaponSystem>
{
public:
	WeaponSystem(Graphics::Renderer* r) : m_renderer(r) {}
	virtual void update(ent_ptr<entityx::EntityManager> es, ent_ptr<entityx::EventManager> events, double alpha) override;

private:
	Graphics::Renderer* m_renderer;
};

/**
 * Move and collide projectiles
 */
class ProjectileSystem : public entityx::System<ProjectileSystem>
{
public:
	virtual void update(ent_ptr<entityx::EntityManager> es, ent_ptr<entityx::EventManager> events, double alpha) override;
};

/**
 * Moves geoms to entity positions
 */
class CollisionSystem : public entityx::System<CollisionSystem>
{
public:
	virtual void update(ent_ptr<entityx::EntityManager> es, ent_ptr<entityx::EventManager> events, double dt) override;
};

class AttachToSystem : public entityx::System<AttachToSystem>
{
public:
	virtual void update(ent_ptr<entityx::EntityManager> em, ent_ptr<entityx::EventManager> events, double dt) override;
};
}
