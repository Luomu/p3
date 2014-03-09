#pragma once
#include "graphics/Renderer.h"
#include "graphics/Material.h"
#include "graphics/VertexBuffer.h"
#include "graphics/RenderState.h"

namespace p3
{
/**
 * Basic drawable object, register to Scene
 * for drawing.
 */
class Graphic
{
public:
	virtual ~Graphic() { }
	virtual void Render() = 0;
};
}
