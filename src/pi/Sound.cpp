// Copyright © 2008-2014 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt
#include "Sound.h"
bool Sound::Init()
{
	return false;
}

void Sound::Uninit()
{

}

void Sound::DestroyAllEvents()
{

}

void Sound::Pause(int on)
{

}

void Sound::PlaySfx(const char* fx, const float volume_left, const float volume_right, const Op op)
{

}

void Sound::PlayMusic(const char* fx, const float volume_left, const float volume_right, const Op op)
{

}

void Sound::BodyMakeNoise(const Body* b, const char* fx, float vol)
{

}

void Sound::SetMasterVolume(const float vol)
{

}

float Sound::GetMasterVolume()
{
	return 0.f;
}

void Sound::SetSfxVolume(const float vol)
{
}

float Sound::GetSfxVolume()
{
	return 0.f;
}
