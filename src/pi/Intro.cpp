// Copyright © 2008-2014 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "Intro.h"
#include "Pi.h"
#include "Lang.h"
#include "Easing.h"
#include "graphics/Renderer.h"
#include "graphics/TextureBuilder.h"
#include "graphics/Graphics.h"
#include <algorithm>

struct PiRngWrapper {
	unsigned int operator()(unsigned int n) {
		return Pi::rng.Int32(n);
	}
};

Intro::Intro(Graphics::Renderer *r, int width, int height)
: Cutscene(r, width, height)
{
	using Graphics::Light;

	m_ambientColor = Color(0);

	const Color one = Color::WHITE;
	const Color two = Color(77, 77, 204, 0);
	m_lights.push_back(Light(Graphics::Light::LIGHT_DIRECTIONAL, vector3f(0.f, 0.3f, 1.f), one));
	m_lights.push_back(Light(Graphics::Light::LIGHT_DIRECTIONAL, vector3f(0.f, -1.f, 0.f), two));
}

Intro::~Intro()
{
}
// stage end times
static const float ZOOM_IN_END  = 2.0f;
static const float WAIT_END     = 12.0f;
static const float ZOOM_OUT_END = 14.0f;

void Intro::Draw(float _time)
{
	m_renderer->SetPerspectiveProjection(55, m_aspectRatio, 1.f, 10000.f);
	m_renderer->SetTransform(matrix4x4f::Identity());

	m_renderer->SetAmbientColor(m_ambientColor);
	m_renderer->SetLights(m_lights.size(), &m_lights[0]);

	// XXX all this stuff will be gone when intro uses a Camera
	// rotate background by time, and a bit extra Z so it's not so flat
	matrix4x4d brot = matrix4x4d::RotateXMatrix(-0.05*_time) * matrix4x4d::RotateZMatrix(0.6);
	m_renderer->ClearDepthBuffer();
}
