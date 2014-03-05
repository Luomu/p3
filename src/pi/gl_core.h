#pragma once
#include "gl_core_2_1.h"
#undef near
#undef far
#undef RegisterClass
#undef GetObject

/*
generated with:
LoadGen.lua -style=pointer_c -spec=gl -version=2.1 -stdext=piexts.txt -profile=core core_2_1
where piexts contains:
EXT_framebuffer_object
EXT_texture_compression_s3tc
KHR_debug
ARB_debug_output
EXT_texture_sRGB
EXT_texture_filter_anisotropic
ARB_framebuffer_sRGB
*/
