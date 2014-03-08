#pragma once
#include "p3/EntitySystem.h"
#include "scenegraph/Model.h"
#include "collider/Geom.h"

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
		: pos(0.0), orient(1.0), interpPos(0.0), interpOrient(1.0) { }
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
	RefCountedPtr<CollMesh> mesh;
	Geom* geom;
};

//has a model for rendering (this is a placeholder)
struct ModelComponent : public entityx::Component<ModelComponent> {
	ModelComponent(SceneGraph::Model* m)
		: model(m)
	{
	}
	SceneGraph::Model* model;
};

struct PlayerInputComponent : public entityx::Component<PlayerInputComponent> {
};

struct ThrusterComponent : public entityx::Component<ThrusterComponent> {
	ThrusterComponent() : linear(0.0), angular(0.0) { }

	vector3d linear;
	vector3d angular;
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
