#include "p3/Game.h"
#include "p3/GameConfig.h"
#include "pi/OS.h"
#include "pi/FileSystem.h"
#include "pi/ModManager.h"

namespace p3
{
void Game::Init(const std::map<std::string, std::string>& options)
{
	OS::NotifyLoadBegin();
	FileSystem::Init();
	FileSystem::userFiles.MakeDirectory(""); // ensure the config directory exists
	m_config.reset(new GameConfig(/*pass option overrides here*/));
	if (m_config->Int("RedirectStdio"))
		OS::RedirectStdio();
	//KeyBindings::InitBindings();
	ModManager::Init();
	//Lang::Resource res(Lang::GetResource("core", config->String("Lang")));
	//Lang::MakeCore(res);
	Uint32 sdlInitFlags = SDL_INIT_VIDEO | SDL_INIT_JOYSTICK;
#if defined(DEBUG) || defined(_DEBUG)
	sdlInitFlags |= SDL_INIT_NOPARACHUTE;
#endif
	if (SDL_Init(sdlInitFlags) < 0)
		Error("SDL initialization failed: %s\n", SDL_GetError());

	Graphics::Settings vs = ReadVideoSettings(GetConfig());
	vs.title = "P3";
	m_renderer.reset(Graphics::Init(vs));

	OS::NotifyLoadEnd();
}

void Game::Uninit()
{
	SDL_Quit();
	Graphics::Uninit();
	FileSystem::Uninit();
}

void Game::Run()
{
	const Uint32 MAX_PHYSICS_TICKS = Clamp(GetConfig()->Int("MaxPhysicsCyclesPerRender"), 0, 4);

	m_sim = new Sim();

	double currentTime = 0.001 * double(SDL_GetTicks());
	double accumulator = m_sim->GetTimeStep();
	double gameTickAlpha = 0.0;
	double frameTime = 0.0;
	while (m_sim->IsRunning()) {
		double newTime = 0.001 * double(SDL_GetTicks());
		frameTime = newTime - currentTime;
		if (frameTime > 0.25)
			frameTime = 0.25;
		currentTime = newTime;
		accumulator += frameTime * m_sim->GetTimeAccelRate();

		const double step = m_sim->GetTimeStep();
		if (step > 0.0) { //pause check
			int physTicks = 0;
			while (accumulator >= step) {
				if (++physTicks >= MAX_PHYSICS_TICKS) {
					accumulator = 0.0;
					break;
				}

				//run physics and game simulation
				m_sim->Execute(step);
				accumulator -= step;
			}

			gameTickAlpha = accumulator / step;
		} else {
			//possible paused updates
		}

		SDL_Event event;
		while (SDL_PollEvent(&event)) {}

		//render
		m_renderer->BeginFrame();
		m_renderer->SetClearColor(Color(12, 12, 93, 255));
		m_renderer->ClearScreen();
		m_renderer->EndFrame();
		m_renderer->SwapBuffers();
	}

	delete m_sim;
}
}
