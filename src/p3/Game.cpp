#include "p3/Game.h"
#include "p3/GameConfig.h"
#include "p3/p3.h"
#include "pi/OS.h"
#include "pi/FileSystem.h"
#include "pi/ModManager.h"
#include "pi/Lang.h"
#include "pi/EnumStrings.h"
#include "p3/Scene.h"
#include "p3/KeyBindings.h"
#include "pi/Stringf.h"

//Lua API
#include "p3/LuaEngine.h"
#include "pi/LuaConstants.h"
#include "pi/LuaLang.h"
#include "pi/LuaFileSystem.h"
#include "pi/LuaEvent.h"

namespace p3
{
void Game::Init(const std::map<std::string, std::string>& options)
{
	//register filthy global (for lua api)
	p3::game = this;

	OS::NotifyLoadBegin();
	FileSystem::Init();
	FileSystem::userFiles.MakeDirectory(""); // ensure the config directory exists
	m_config.reset(new GameConfig(options));
	if (m_config->Int("RedirectStdio"))
		OS::RedirectStdio();
	KeyBindings::InitBindings();
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

	m_modelCache.reset(new ModelCache(GetRenderer()));

	m_console.reset(new LuaConsole(GetUI()));
	KeyBindings::toggleLuaConsole.onPress.connect(sigc::mem_fun(m_console.get(), &LuaConsole::Toggle));

	OS::NotifyLoadEnd();
}

void Game::Uninit()
{
	m_config->Save();
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
	LuaEvent::Queue("onGameStart");
	LuaEvent::Emit();

	GetUI()->DropAllLayers();
	m_fpsLabel = GetUI()->Label("16.67");
	GetUI()->GetTopLayer()->SetInnerWidget(m_fpsLabel);

	//http://gafferongames.com/game-physics/fix-your-timestep/
	double currentTime = 0.001 * double(SDL_GetTicks());
	double accumulator = m_sim->GetTimeStep();
	double gameTickAlpha = 0.0;
	double frameTime = 0.0;

	//frame timing
	double lastStatsTime = currentTime;
	Uint32 frameCount = 0;

	while (m_sim->IsRunning()) {
		double newTime = 0.001 * double(SDL_GetTicks());
		frameTime = newTime - currentTime;
		if (frameTime > 0.25)
			frameTime = 0.25;
		currentTime = newTime;
		accumulator += frameTime * m_sim->GetTimeAccelRate();

		frameCount++;

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

		m_sim->InterpolatePositions(gameTickAlpha);

		HandleEvents();
		m_ui->Update();

		//render
		m_renderer->BeginFrame();
		m_sim->GetScene()->Render();
		m_ui->Draw();
		m_renderer->EndFrame();
		m_renderer->SwapBuffers();

		//if (Pi::game->UpdateTimeAccel())
		//	accumulator = 0; // fix for huge pauses 10000x -> 1x

		if (newTime - lastStatsTime > 1.0) {
			m_fpsLabel->SetText(stringf("%0{f.2}", 1000.f / frameCount));
			frameCount = 0;
			if (newTime - lastStatsTime > 1.2)
				lastStatsTime = newTime;
			else
				lastStatsTime += 1.0;
		}
	}

	delete m_sim;
}

void Game::HandleEvents()
{
	SDL_Event event;

	// XXX for most keypresses SDL will generate KEYUP/KEYDOWN and TEXTINPUT
	// events. keybindings run off KEYUP/KEYDOWN. the console is opened/closed
	// via keybinding. the console TextInput widget uses TEXTINPUT events. thus
	// after switching the console, the stray TEXTINPUT event causes the
	// console key (backtick) to appear in the text entry field. we hack around
	// this by setting this flag if the console was switched. if its set, we
	// swallow the TEXTINPUT event this hack must remain until we have a
	// unified input system
	bool skipTextInput = false;

	Mouse::motion[0] = Mouse::motion[1] = 0;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			if (m_sim)
				m_sim->End();
		}

		if (skipTextInput && event.type == SDL_TEXTINPUT) {
			skipTextInput = false;
			continue;
		}
		if (GetUI()->DispatchSDLEvent(event))
			continue;

		bool consoleActive = m_console->IsActive();
		if (!consoleActive)
			KeyBindings::DispatchSDLEvent(&event);
		else
			KeyBindings::toggleLuaConsole.CheckSDLEventAndDispatch(&event);
		if (consoleActive != m_console->IsActive()) {
			skipTextInput = true;
			continue;
		}

		if (m_console->IsActive())
			continue;

		switch (event.type) {
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_ESCAPE) {
				if (m_sim)
					m_sim->End();
				break;
			}
			Keyboard::state[event.key.keysym.sym] = true;
			Keyboard::modState = event.key.keysym.mod;
			break;
		case SDL_KEYUP:
			Keyboard::state[event.key.keysym.sym] = false;
			Keyboard::modState = event.key.keysym.mod;
			break;
		case SDL_MOUSEBUTTONDOWN:
			if (event.button.button < COUNTOF(Mouse::button))
				Mouse::button[event.button.button] = 1;
			break;
		case SDL_MOUSEBUTTONUP:
			if (event.button.button < COUNTOF(Mouse::button))
				Mouse::button[event.button.button] = 0;
			break;
		case SDL_MOUSEWHEEL:
			Mouse::onWheel.emit(event.wheel.y > 0); // true = up
			break;
		case SDL_MOUSEMOTION:
			Mouse::motion[0] += event.motion.xrel;
			Mouse::motion[1] += event.motion.yrel;
			break;
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
	LuaConsole::Register();

	UI::Lua::Init();

	lua_State* l = Lua::manager->GetLuaState();
	pi_lua_dofile(l, "libs/autoload.lua");
	pi_lua_dofile_recursive(l, "ui");
	pi_lua_dofile_recursive(l, "modules");
}

}
