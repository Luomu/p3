// Copyright © 2008-2014 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#ifndef _MODELBODY_H
#define _MODELBODY_H

#include "libs.h"
#include "Body.h"
#include "CollMesh.h"

class Geom;
class Camera;
namespace Graphics { class Renderer; class Light; }
namespace SceneGraph { class Model; class Animation; }

class ModelBody: public Body {
public:
	OBJDEF(ModelBody, Body, MODELBODY);
	ModelBody();
	virtual ~ModelBody();
	void SetPosition(const vector3d &p);
	void SetOrient(const matrix3x3d &r);
	void TransformToModelCoords(const Frame *camFrame);
	virtual void SetFrame(Frame *f);
	// Colliding: geoms are checked against collision space
	void SetColliding(bool colliding);
	// Static: geoms are static relative to frame
	void SetStatic(bool isStatic);
	bool IsStatic() const { return m_isStatic; }
	const Aabb &GetAabb() const { return m_collMesh->GetAabb(); }
	SceneGraph::Model *GetModel() const { return m_model; }
	CollMesh *GetCollMesh() { return m_collMesh.Get(); }

	void SetModel(const char *modelName);

	void RenderModel(Graphics::Renderer *r, const Camera *camera, const vector3d &viewCoords, const matrix4x4d &viewTransform);

	virtual void TimeStepUpdate(const float timeStep);

private:
	void RebuildCollisionMesh();
	void DeleteGeoms();
	void AddGeomsToFrame(Frame*);
	void RemoveGeomsFromFrame(Frame*);
	void MoveGeoms(const matrix4x4d&, const vector3d&);

	bool m_isStatic;
	RefCountedPtr<CollMesh> m_collMesh;
	Geom *m_geom; //static geom
	std::string m_modelName;
	SceneGraph::Model *m_model;
	std::vector<Geom*> m_dynGeoms;
	SceneGraph::Animation *m_idleAnimation;
};

#endif /* _MODELBODY_H */
