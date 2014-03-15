// Copyright © 2008-2014 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "Camera.h"
#include "Frame.h"
#include "galaxy/StarSystem.h"
#include "Space.h"
#include "Player.h"
#include "Pi.h"
#include "Game.h"
#include "Planet.h"
#include "graphics/Graphics.h"
#include "graphics/Renderer.h"
#include "graphics/VertexArray.h"
#include "graphics/Material.h"
#include "graphics/TextureBuilder.h"

#include <SDL_stdinc.h>

using namespace Graphics;

// if a body would render smaller than this many pixels, just ignore it
static const float OBJECT_HIDDEN_PIXEL_THRESHOLD = 2.0f;

// if a terrain object would render smaller than this many pixels, draw a billboard instead
static const float BILLBOARD_PIXEL_THRESHOLD = 15.0f;

CameraContext::CameraContext(float width, float height, float fovAng, float zNear, float zFar) :
	m_width(width),
	m_height(height),
	m_fovAng(fovAng),
	m_zNear(zNear),
	m_zFar(zFar),
	m_frustum(m_width, m_height, m_fovAng, m_zNear, m_zFar),
	m_frame(nullptr),
	m_pos(0.0),
	m_orient(matrix3x3d::Identity()),
	m_camFrame(nullptr)
{
}

CameraContext::~CameraContext()
{
	if (m_camFrame)
		EndFrame();
}

void CameraContext::BeginFrame()
{
	assert(m_frame);
	assert(!m_camFrame);

	// make temporary camera frame
	m_camFrame = new Frame(m_frame, "camera", Frame::FLAG_ROTATING);

	// move and orient it to the camera position
	m_camFrame->SetOrient(m_orient, Pi::game ? Pi::game->GetTime() : 0.0);
	m_camFrame->SetPosition(m_pos);

	// make sure old orient and interpolated orient (rendering orient) are not rubbish
	m_camFrame->ClearMovement();
	m_camFrame->UpdateInterpTransform(1.0);			// update root-relative pos/orient
}

void CameraContext::EndFrame()
{
	assert(m_frame);
	assert(m_camFrame);

	m_frame->RemoveChild(m_camFrame);
	delete m_camFrame;
	m_camFrame = nullptr;
}

void CameraContext::ApplyDrawTransforms(Graphics::Renderer *r)
{
	r->SetPerspectiveProjection(m_fovAng, m_width/m_height, m_zNear, m_zFar);
	r->SetTransform(matrix4x4f::Identity());
}


Camera::Camera(RefCountedPtr<CameraContext> context, Graphics::Renderer *renderer) :
	m_context(context),
	m_renderer(renderer)
{
	Graphics::MaterialDescriptor desc;
	desc.textures = 1;

	m_billboardMaterial.reset(m_renderer->CreateMaterial(desc));
	m_billboardMaterial->texture0 = Graphics::TextureBuilder::Billboard("textures/planet_billboard.png").GetOrCreateTexture(m_renderer, "billboard");
}

void Camera::Update()
{
	Frame *camFrame = m_context->GetCamFrame();

	// evaluate each body and determine if/where/how to draw it
	m_sortedBodies.clear();
	for (Body* b : Pi::game->GetSpace()->GetBodies()) {
		BodyAttrs attrs;
		attrs.body = b;

		// determine position and transform for draw
		Frame::GetFrameTransform(b->GetFrame(), camFrame, attrs.viewTransform);
		attrs.viewCoords = attrs.viewTransform * b->GetInterpPosition();

		// cull off-screen objects
		double rad = b->GetClipRadius();
		if (!m_context->GetFrustum().TestPointInfinite(attrs.viewCoords, rad))
			continue;

		attrs.camDist = attrs.viewCoords.Length();
		attrs.bodyFlags = b->GetFlags();

		// approximate pixel width (disc diameter) of body on screen
		float pixSize = (Graphics::GetScreenWidth() * rad / attrs.camDist);
		if (pixSize < OBJECT_HIDDEN_PIXEL_THRESHOLD)
			continue;

		// terrain objects are visible from distance but might not have any discernable features
		attrs.billboard = false;
		if (b->IsType(Object::TERRAINBODY)) {
			if (pixSize < BILLBOARD_PIXEL_THRESHOLD) {
				attrs.billboard = true;
				vector3d pos;
				double size = rad * 2.0 * m_context->GetFrustum().TranslatePoint(attrs.viewCoords, pos);
				attrs.billboardPos = vector3f(&pos.x);
				attrs.billboardSize = float(size);
				if (b->IsType(Object::STAR)) {
					Uint8 *col = StarSystem::starRealColors[b->GetSystemBody()->GetType()];
					attrs.billboardColor = Color(col[0], col[1], col[2], 255);
				}
				else if (b->IsType(Object::PLANET)) {
					double surfaceDensity; // unused
					// XXX this is pretty crap because its not always right
					// (gas giants are always white) and because it should have
					// some star colour mixed in to simulate lighting
					b->GetSystemBody()->GetAtmosphereFlavor(&attrs.billboardColor, &surfaceDensity);
					attrs.billboardColor.a = 255; // no alpha, these things are hard enough to see as it is
				}
				else
					attrs.billboardColor = Color::WHITE;
			}
		}

		m_sortedBodies.push_back(attrs);
	}

	// depth sort
	m_sortedBodies.sort();
}

void Camera::Draw(const Body *excludeBody)
{
	PROFILE_SCOPED()

	Frame *camFrame = m_context->GetCamFrame();

	m_renderer->ClearScreen();

	for (std::list<BodyAttrs>::iterator i = m_sortedBodies.begin(); i != m_sortedBodies.end(); ++i) {
		BodyAttrs *attrs = &(*i);

		// explicitly exclude a single body if specified (eg player)
		if (attrs->body == excludeBody)
			continue;

		// draw something!
		if (attrs->billboard) {
			Graphics::Renderer::MatrixTicket mt(m_renderer, Graphics::MatrixMode::MODELVIEW);
			m_renderer->SetTransform(matrix4x4d::Identity());
			m_billboardMaterial->diffuse = attrs->billboardColor;
		}
		else
			attrs->body->Render(m_renderer, this, attrs->viewCoords, attrs->viewTransform);
	}
}
