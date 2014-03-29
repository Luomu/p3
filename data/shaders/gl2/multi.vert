// Copyright © 2008-2014 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#ifdef TEXTURE0
varying vec2 texCoord0;
#endif
#ifdef VERTEXCOLOR
varying vec4 vertexColor;
#endif
#if (NUM_LIGHTS > 0)
varying vec3 eyePos;
varying vec3 normal;
#endif

void main(void)
{
	gl_Position = logarithmicTransform();
#ifdef VERTEXCOLOR
	vertexColor = gl_Color;
#endif
#ifdef TEXTURE0
	texCoord0 = gl_MultiTexCoord0.xy;
#endif
#if (NUM_LIGHTS > 0)
	eyePos = vec3(gl_ModelViewMatrix * gl_Vertex);
	normal = normalize(gl_NormalMatrix * gl_Normal);
#endif
}
