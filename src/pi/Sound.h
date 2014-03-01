// Copyright © 2008-2014 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt
#pragma once
#include "libs.h"

class Body;

namespace Sound {

typedef Uint32 Op;

bool Init ();
void Uninit ();
void DestroyAllEvents();
void Pause (int on);
void PlaySfx (const char *fx, const float volume_left, const float volume_right, const Op op);
void PlayMusic (const char *fx, const float volume_left, const float volume_right, const Op op);
inline static void PlaySfx (const char *fx) { return PlaySfx(fx, 1.0f, 1.0f, 0); }
void BodyMakeNoise(const Body *b, const char *fx, float vol);
void SetMasterVolume(const float vol);
float GetMasterVolume();
void SetSfxVolume(const float vol);
float GetSfxVolume();

} /* namespace Sound */
