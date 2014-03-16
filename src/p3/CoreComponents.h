#pragma once
#include "p3/Common.h"
#include "p3/EntitySystem.h"
#include "scenegraph/Model.h"
#include "collider/Geom.h"
#include "p3/Graphic.h"

/**
 * Common components. Can be moved into separate files.
 */

namespace p3
{

//for entities with identifiable names
struct NameComponent : public entityx::Component<NameComponent> {
	NameComponent(std::string& n) : name(n) {}
	std::string name;
};

struct PosOrientComponent : public entityx::Component<PosOrientComponent> {
	PosOrientComponent()
		: pos(0.0), orient(1.0), interpPos(0.0), interpOrient(1.0)
		, oldPos(0.0), oldAngDisplacement(0.0) { }
	PosOrientComponent(vector3d initialPos, const matrix3x3d& initialOrient)
		: pos(initialPos), orient(initialOrient), interpPos(initialPos)
		, interpOrient(initialOrient), oldPos(initialPos), oldAngDisplacement(0.0) {}
	vector3d pos;
	matrix3x3d orient;

	//interpolated for drawing
	vector3d interpPos;
	matrix3x3d interpOrient;

	//previous state used for interpolation
	vector3d oldPos;
	vector3d oldAngDisplacement;
};

struct MassComponent : public entityx::Component<MassComponent> {
	MassComponent(double initialMass) : mass(initialMass) {}
	double mass;
};

//for things that move according to physics
struct DynamicsComponent : public entityx::Component<DynamicsComponent> {
	DynamicsComponent()
		: force(0.0)
		, torque(0.0)
		, vel(0.0)
		, angVel(0.0)
		, angInertia(1.0)
	{
	}
	vector3d force;
	vector3d torque;
	vector3d vel;
	vector3d angVel;
	double angInertia;
};

//for entities that can be damaged and destroyed
struct HealthComponent : public entityx::Component<HealthComponent> {
	HealthComponent(double initialHealth = 100.0)
		: health(initialHealth)
		, invulnerable(false)
	{
	}
	double health;
	bool invulnerable;
};

//has a triangle soup collision mesh
struct CollisionMeshComponent : public entityx::Component<CollisionMeshComponent> {
	CollisionMeshComponent(Entity ent, RefCountedPtr<CollMesh> m)
		: mesh(m) {
		geom.reset(new Geom(m->GetGeomTree()));
		owner = ent;
		geom->SetUserData(&owner);
	}
	RefCountedPtr<CollMesh> mesh;
	std::unique_ptr<Geom> geom;
	Entity owner; //needed forr collider
};

/**
 * Takes ownership of the graphic
 * Graphics should not be shared
 */
struct GraphicComponent : public entityx::Component<GraphicComponent> {
	GraphicComponent(Graphic* g)
	{
		graphic.Reset(g);
	}

	ref_ptr<Graphic> graphic;
};

struct PlayerInputComponent : public entityx::Component<PlayerInputComponent> {
};

struct ThrusterComponent : public entityx::Component<ThrusterComponent> {
	ThrusterComponent() : linear(0.0), angular(0.0) { }

	vector3d linear;
	vector3d angular;
};

//Simplified physics for a projectile. Dies after a timeout.
struct ProjectileComponent : public entityx::Component<ProjectileComponent> {
	ProjectileComponent(vector3d parentVel, vector3d projVel, double timeout) : baseVel(parentVel), dirVel(projVel), lifetime(timeout) {}
	vector3d baseVel;
	vector3d dirVel;
	double lifetime;
};

//prototype component indicating an entity can fire a laser from its center
//(requires pos, orient)
struct WeaponComponent : public entityx::Component<WeaponComponent> {
	WeaponComponent() : firing(false) {}

	bool firing;
};

/**
 * Copy position and orientation from another entity
 * Requires: PosOrientComponent for this and target
 */
struct AttachToEntityComponent : public entityx::Component<AttachToEntityComponent> {
	AttachToEntityComponent(Entity tgt, vector3d offset_)
		: target(tgt), offset(offset_) {}
	Entity target;
	vector3d offset;
};

//a body
//has position
//has orientation
//has clipping radius
//has physical radius
//has label
//resides in Frame

//a dynamic body
//has mass
//has velocity
//has torque
//has angular velocity

//A terrain body:
//has mass
//has a geosphere gfx
//has an associated system body
//is not subject to dynamics (pos/orient from frame)
//has a planet geom

//A star
//is a terrainbody a halo

}
