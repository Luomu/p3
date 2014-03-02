#include "p3/Game.h"
#include "p3/GameConfig.h"
#include "p3/p3.h"
#include "pi/OS.h"
#include "pi/FileSystem.h"
#include "pi/ModManager.h"
#include "pi/Lang.h"
#include "pi/EnumStrings.h"

#include "p3/LuaEngine.h"
#include "pi/LuaConstants.h"
#include "pi/LuaLang.h"
#include "pi/LuaFileSystem.h"

namespace p3
{
void Game::Init(const std::map<std::string, std::string>& options)
{
	//register filthy global (for lua api)
	p3::game = this;

	OS::NotifyLoadBegin();
	FileSystem::Init();
	FileSystem::userFiles.MakeDirectory(""); // ensure the config directory exists
	m_config.reset(new GameConfig(/*pass option overrides here*/));
	if (m_config->Int("RedirectStdio"))
		OS::RedirectStdio();
	//KeyBindings::InitBindings();
	ModManager::Init();
	Lang::Resource res(Lang::GetResource("core", GetConfig()->String("Lang")));
	Lang::MakeCore(res);
	Uint32 sdlInitFlags = SDL_INIT_VIDEO | SDL_INIT_JOYSTICK;
#if defined(DEBUG) || defined(_DEBUG)
	sdlInitFlags |= SDL_INIT_NOPARACHUTE;
#endif
	if (SDL_Init(sdlInitFlags) < 0)
		Error("SDL initialization failed: %s\n", SDL_GetError());

	Graphics::Settings vs = ReadVideoSettings(GetConfig());
	vs.title = "P3";
	m_renderer.reset(Graphics::Init(vs));

	EnumStrings::Init();

	Lua::Init();

	m_ui.Reset(new UI::Context(Lua::manager,
	                           GetRenderer(),
	                           GetRenderer()->GetWindow()->GetWidth(),
	                           GetRenderer()->GetWindow()->GetHeight(),
	                           Lang::GetCore().GetLangCode()));

	InitLua();

	OS::NotifyLoadEnd();
}

void Game::Uninit()
{
	static_cast<p3::GameConfig*>(m_config.get())->Save();
	m_ui.Reset();
	Lua::Uninit();
	Graphics::Uninit();
	SDL_Quit();
	FileSystem::Uninit();
}

void Game::Run()
{
	const Uint32 MAX_PHYSICS_TICKS = Clamp(GetConfig()->Int("MaxPhysicsCyclesPerRender"), 0, 4);

	m_sim = new Sim();

	GetUI()->DropAllLayers();
	auto w = GetUI()->CallTemplate("Test");
	GetUI()->GetTopLayer()->SetInnerWidget(w);

	//http://gafferongames.com/game-physics/fix-your-timestep/
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

		HandleEvents();
		m_ui->Update();

		//render
		m_renderer->BeginFrame();
		m_renderer->SetClearColor(Color(12, 12, 93, 255));
		m_renderer->ClearScreen();
		m_ui->Draw();
		m_renderer->EndFrame();
		m_renderer->SwapBuffers();

		//if (Pi::game->UpdateTimeAccel())
		//	accumulator = 0; // fix for huge pauses 10000x -> 1x
	}

	delete m_sim;
}

void Game::HandleEvents()
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			if (m_sim)
				m_sim->End();
		}

		//gui may swallow event
		if (GetUI()->DispatchSDLEvent(event))
			continue;

		//extra check for active console

		switch (event.type) {
		case SDL_KEYDOWN: {
			if (event.key.keysym.sym == SDLK_ESCAPE) {
				if (m_sim)
					m_sim->End();
			}
		}
		}
	}
}

void Game::InitLua()
{
	LuaObject<PropertiedObject>::RegisterClass();

	LuaConstants::Register(Lua::manager->GetLuaState());
	LuaLang::Register();
	p3::LuaEngine::Register();
	LuaFileSystem::Register();

	UI::Lua::Init();

	lua_State* l = Lua::manager->GetLuaState();
	pi_lua_dofile(l, "libs/autoload.lua");
	pi_lua_dofile_recursive(l, "ui");
	pi_lua_dofile_recursive(l, "modules");
}

}
