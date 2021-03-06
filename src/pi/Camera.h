// Copyright © 2008-2014 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#ifndef _CAMERA_H
#define _CAMERA_H

#include "graphics/Frustum.h"
#include "graphics/Light.h"
#include "RefCounted.h"
#include "vector3.h"
#include "matrix4x4.h"
#include "Body.h"
#include "graphics/Material.h"

class Frame;
namespace Graphics { class Renderer; }

class CameraContext : public RefCounted {
public:
	// camera for rendering to width x height with view frustum properties
	CameraContext(float width, float height, float fovAng, float zNear, float zFar);
	~CameraContext();

	float GetWidth()  const { return m_width;  }
	float GetHeight() const { return m_height; }
	float GetFovAng() const { return m_fovAng; }
	float GetZNear()  const { return m_zNear;  }
	float GetZFar()   const { return m_zFar;   }

	// frame to position the camera relative to
	void SetFrame(Frame *frame) { m_frame = frame; }

	// camera position relative to the frame origin
	void SetPosition(const vector3d &pos) { m_pos = pos; }

	// camera orientation relative to the frame origin
	void SetOrient(const matrix3x3d &orient) { m_orient = orient; }

	// get the frustum. use for projection
	const Graphics::Frustum &GetFrustum() const { return m_frustum; }

	// generate and destroy the camere frame, used mostly to transform things to camera space
	void BeginFrame();
	void EndFrame();

	// valid between BeginFrame and EndFrame
	Frame *GetCamFrame() const { assert(m_camFrame); return m_camFrame; }

	// apply projection and modelview transforms to the renderer
	void ApplyDrawTransforms(Graphics::Renderer *r);

private:
	float m_width;
	float m_height;
	float m_fovAng;
	float m_zNear;
	float m_zFar;

	Graphics::Frustum m_frustum;

	Frame *m_frame;
	vector3d m_pos;
	matrix3x3d m_orient;

	Frame *m_camFrame;
};


class Camera {
public:
	Camera(RefCountedPtr<CameraContext> context, Graphics::Renderer *renderer);

	const CameraContext *GetContext() const { return m_context.Get(); }

	void Update();
	void Draw(const Body *excludeBody = nullptr);

private:
	RefCountedPtr<CameraContext> m_context;
	Graphics::Renderer *m_renderer;

	std::unique_ptr<Graphics::Material> m_billboardMaterial;

	// temp attrs for sorting and drawing
	struct BodyAttrs {
		Body *body;

		// camera position and orientation relative to the body
		vector3d viewCoords;
		matrix4x4d viewTransform;

		// body distance from camera
		double camDist;

		// body flags. DRAW_LAST is the interesting one
		Uint32 bodyFlags;

		// if true, draw object as billboard of billboardSize at billboardPos
		bool billboard;
		vector3f billboardPos;
		float billboardSize;
		Color billboardColor;

		// for sorting. "should a be drawn before b?"
		friend bool operator<(const BodyAttrs &a, const BodyAttrs &b) {
			// both drawing last; distance order
			if (a.bodyFlags & Body::FLAG_DRAW_LAST && b.bodyFlags & Body::FLAG_DRAW_LAST)
				return a.camDist > b.camDist;

			// a drawing last; draw b first
			if (a.bodyFlags & Body::FLAG_DRAW_LAST)
				return false;

			// b drawing last; draw a first
			if (b.bodyFlags & Body::FLAG_DRAW_LAST)
				return true;

			// both in normal draw; distance order
			return a.camDist > b.camDist;
		}
	};

	std::list<BodyAttrs> m_sortedBodies;
};

#endif
