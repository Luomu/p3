// Copyright © 2008-2014 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "Light.h"

namespace Graphics {

Light::Light() :
	m_type(LIGHT_POINT),
	m_position(0.f),
	m_diffuse(Color(255))
{

}

Light::Light(LightType t, const vector3f &p, const Color &d) :
	m_type(t),
	m_position(p),
	m_diffuse(d)
{
}

}
