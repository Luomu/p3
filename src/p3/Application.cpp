#include "Application.h"
#include "pi/OS.h"
#include "pi/FileSystem.h"
#include "pi/ModManager.h"

namespace p3
{

Application::Application()
{

}

Application::~Application()
{

}

Graphics::Settings Application::ReadVideoSettings(IniConfig* config)
{
	Graphics::Settings s = {};
	s.width = config->Int("ScrWidth");
	s.height = config->Int("ScrHeight");
	s.fullscreen = (config->Int("StartFullscreen") != 0);
	s.requestedSamples = config->Int("AntiAliasingMode");
	s.vsync = (config->Int("VSync") != 0);
	s.useTextureCompression = (config->Int("UseTextureCompression") != 0);
	s.enableDebugMessages = (config->Int("EnableGLDebug") != 0);
	s.iconFile = OS::GetIconFilename();
	return s;
}

}
