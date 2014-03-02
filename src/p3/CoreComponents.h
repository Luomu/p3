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

//for things that move according to physics
struct DynamicsComponent : public entityx::Component<DynamicsComponent> {
	double mass;
	vector3d force;
	vector3d torque;
	vector3d vel;
	vector3d angVel;
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
	Geom *geom;
};

//has a model for rendering (this is a placeholder)
struct ModelComponent : public entityx::Component<ModelComponent> {
	ModelComponent(SceneGraph::Model* m)
		: model(m)
	{
	}
	SceneGraph::Model* model;
};

}
