// Copyright © 2008-2014 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "libs.h"
#include "ModelBody.h"
#include "Frame.h"
#include "Game.h"
#include "ModelCache.h"
#include "Pi.h"
#include "Serializer.h"
#include "Space.h"
#include "WorldView.h"
#include "Camera.h"
#include "Planet.h"
#include "collider/collider.h"
#include "graphics/Renderer.h"
#include "scenegraph/SceneGraph.h"
#include "scenegraph/NodeVisitor.h"
#include "scenegraph/CollisionGeometry.h"

class DynGeomFinder : public SceneGraph::NodeVisitor {
public:
	std::vector<SceneGraph::CollisionGeometry*> results;

	virtual void ApplyCollisionGeometry(SceneGraph::CollisionGeometry &cg)
	{
		if (cg.IsDynamic())
			results.push_back(&cg);
	}

	SceneGraph::CollisionGeometry *GetCgForTree(GeomTree *t)
	{
		for (auto it = results.begin(); it != results.end(); ++it)
			if ((*it)->GetGeomTree() == t) return (*it);
		return 0;
	}
};

class DynCollUpdateVisitor : public SceneGraph::NodeVisitor {
private:
	std::vector<matrix4x4f> m_matrixStack;

public:
	void Reset() { m_matrixStack.clear(); }

	virtual void ApplyMatrixTransform(SceneGraph::MatrixTransform &m)
	{
		matrix4x4f matrix = matrix4x4f::Identity();
		if (!m_matrixStack.empty()) matrix = m_matrixStack.back();

		m_matrixStack.push_back(matrix * m.GetTransform());
		m.Traverse(*this);
		m_matrixStack.pop_back();
	}

	virtual void ApplyCollisionGeometry(SceneGraph::CollisionGeometry &cg)
	{
		if (!cg.GetGeom()) return;

		matrix4x4ftod(m_matrixStack.back(), cg.GetGeom()->m_animTransform);
	}
};

ModelBody::ModelBody()
: m_isStatic(false)
, m_geom(0)
, m_model(0)
{
}

ModelBody::~ModelBody()
{
	SetFrame(0); // Will remove geom from frame if necessary.
	DeleteGeoms();

	//delete instanced model
	delete m_model;
}

void ModelBody::SetStatic(bool isStatic)
{
	if (isStatic == m_isStatic) return;
	m_isStatic = isStatic;
	if (!m_geom) return;

	if (m_isStatic) {
		GetFrame()->RemoveGeom(m_geom);
		GetFrame()->AddStaticGeom(m_geom);
	}
	else {
		GetFrame()->RemoveStaticGeom(m_geom);
		GetFrame()->AddGeom(m_geom);
	}
}

void ModelBody::SetColliding(bool colliding)
{
	if(colliding) m_geom->Enable();
	else m_geom->Disable();
}

void ModelBody::RebuildCollisionMesh()
{
	if (m_geom) {
		if (GetFrame()) RemoveGeomsFromFrame(GetFrame());
		DeleteGeoms();
	}

	m_collMesh = m_model->GetCollisionMesh();
	double maxRadius= m_collMesh->GetAabb().GetRadius();

	//static geom
	m_geom = new Geom(m_collMesh->GetGeomTree());
	m_geom->SetUserData(static_cast<void*>(this));
	m_geom->MoveTo(GetOrient(), GetPosition());

	SetPhysRadius(maxRadius);

	//have to figure out which collision geometries are responsible for which geomtrees
	DynGeomFinder dgf;
	m_model->GetRoot()->Accept(dgf);

	//dynamic geoms
	for (auto it = m_collMesh->GetDynGeomTrees().begin(); it != m_collMesh->GetDynGeomTrees().end(); ++it) {
		Geom *dynG = new Geom(*it);
		dynG->SetUserData(static_cast<void*>(this));
		dynG->MoveTo(GetOrient(), GetPosition());
		dynG->m_animTransform = matrix4x4d::Identity();
		SceneGraph::CollisionGeometry *cg = dgf.GetCgForTree(*it);
		if (cg)
			cg->SetGeom(dynG);
		m_dynGeoms.push_back(dynG);
	}

	if (GetFrame()) AddGeomsToFrame(GetFrame());
}

void ModelBody::SetModel(const char *modelName)
{
	//remove old instance
	delete m_model;
	m_model = 0;

	m_modelName = modelName;

	//create model instance (some modelbodies, like missiles could avoid this)
	m_model = Pi::FindModel(m_modelName)->MakeInstance();
	m_idleAnimation = m_model->FindAnimation("idle");

	SetClipRadius(m_model->GetDrawClipRadius());

	RebuildCollisionMesh();
}

void ModelBody::SetPosition(const vector3d &p)
{
	Body::SetPosition(p);
	MoveGeoms(GetOrient(), p);

	// for rebuild of static objects in collision space
	if (m_isStatic) SetFrame(GetFrame());
}

void ModelBody::SetOrient(const matrix3x3d &m)
{
	Body::SetOrient(m);
	const matrix4x4d m2 = m;
	MoveGeoms(m2, GetPosition());
}

void ModelBody::SetFrame(Frame *f)
{
	if (f == GetFrame()) return;

	//remove collision geoms from old frame
	if (GetFrame()) RemoveGeomsFromFrame(GetFrame());

	Body::SetFrame(f);

	//add collision geoms to new frame
	if (f) AddGeomsToFrame(f);
}

void ModelBody::DeleteGeoms()
{
	delete m_geom;
	m_geom = 0;
	for (auto it = m_dynGeoms.begin(); it != m_dynGeoms.end(); ++it)
		delete *it;
	m_dynGeoms.clear();
}

void ModelBody::AddGeomsToFrame(Frame *f)
{
	const int group = f->GetCollisionSpace()->GetGroupHandle();

	m_geom->SetGroup(group);

	if(m_isStatic) {
		f->AddStaticGeom(m_geom);
	} else {
		f->AddGeom(m_geom);
	}

	for (auto it = m_dynGeoms.begin(); it != m_dynGeoms.end(); ++it) {
		(*it)->SetGroup(group);
		f->AddGeom(*it);
	}
}

void ModelBody::RemoveGeomsFromFrame(Frame *f)
{
	if(m_isStatic) {
		GetFrame()->RemoveStaticGeom(m_geom);
	} else {
		GetFrame()->RemoveGeom(m_geom);
	}

	for (auto it = m_dynGeoms.begin(); it != m_dynGeoms.end(); ++it)
		GetFrame()->RemoveGeom(*it);
}

void ModelBody::MoveGeoms(const matrix4x4d &m, const vector3d &p)
{
	m_geom->MoveTo(m, p);

	//accumulate transforms to animated positions
	if (!m_dynGeoms.empty()) {
		DynCollUpdateVisitor dcv;
		m_model->GetRoot()->Accept(dcv);
	}

	for (auto it = m_dynGeoms.begin(); it != m_dynGeoms.end(); ++it) {
		//combine orient & pos
		static matrix4x4d s_tempMat;
		for (unsigned int i = 0; i < 12; i++)
			s_tempMat[i] = m[i];
		s_tempMat[12] = p.x;
		s_tempMat[13] = p.y;
		s_tempMat[14] = p.z;
		s_tempMat[15] = m[15];

		(*it)->MoveTo(s_tempMat * (*it)->m_animTransform);
	}
}

// setLighting: set renderer lights according to current position and sun
// positions. Original lighting is passed back in oldLights, oldAmbient, and
// should be reset after rendering with ModelBody::ResetLighting.
void ModelBody::SetLighting(Graphics::Renderer *r, const Camera *camera, std::vector<Graphics::Light> &oldLights, Color &oldAmbient) {
	std::vector<Graphics::Light> newLights;
	double ambient = 0.2;
	const std::vector<Camera::LightSource> &lightSources = camera->GetLightSources();
	newLights.reserve(lightSources.size());
	oldLights.reserve(lightSources.size());
	for(size_t i = 0; i < lightSources.size(); i++) {
		Graphics::Light light(lightSources[i].GetLight());

		oldLights.push_back(light);

		Color c = light.GetDiffuse();
		Color cs = light.GetSpecular();
		light.SetDiffuse(c);
		light.SetSpecular(cs);

		newLights.push_back(light);
	}

	oldAmbient = r->GetAmbientColor();
	r->SetAmbientColor(Color(ambient*255));
	r->SetLights(newLights.size(), &newLights[0]);
}

void ModelBody::RenderModel(Graphics::Renderer *r, const Camera *camera, const vector3d &viewCoords, const matrix4x4d &viewTransform, const bool setLighting)
{
	std::vector<Graphics::Light> oldLights;
	Color oldAmbient;
	if (setLighting)
		SetLighting(r, camera, oldLights, oldAmbient);

	matrix4x4d m2 = GetInterpOrient();
	m2.SetTranslate(GetInterpPosition());
	matrix4x4d t = viewTransform * m2;

	//double to float matrix
	matrix4x4f trans;
	for (int i=0; i<12; i++) trans[i] = float(t[i]);
	trans[12] = viewCoords.x;
	trans[13] = viewCoords.y;
	trans[14] = viewCoords.z;
	trans[15] = 1.0f;

	m_model->Render(trans);
}

void ModelBody::TimeStepUpdate(const float timestep)
{
	if (m_idleAnimation)
		// step animation by timestep/total length, loop to 0.0 if it goes >= 1.0
		m_idleAnimation->SetProgress(fmod(m_idleAnimation->GetProgress() + timestep / m_idleAnimation->GetDuration(), 1.0));

	m_model->UpdateAnimations();
}
