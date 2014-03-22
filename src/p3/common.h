#pragma once

//baseline stuff
#include <cstdio>
#include <string>
#include <vector>
#include <unordered_set>

#include "pi/vector2.h"
#include "pi/vector3.h"
#include "pi/vector4.h"
#include "pi/matrix3x3.h"
#include "pi/matrix4x4.h"
#include "pi/RefCounted.h"
#include "pi/StringF.h"

namespace p3
{
//this fits our style better
template <class T>
using ref_ptr = RefCountedPtr<T>;
}
