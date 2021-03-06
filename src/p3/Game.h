#pragma once
#include "p3/Application.h"
#include "p3/Sim.h"
#include "graphics/Renderer.h"
#include "ui/Context.h"
#include "pi/ModelCache.h"
#include "pi/LuaConsole.h"
#include "pi/Random.h"

namespace p3
{
class Game : public Application
{
public:
	virtual void Init(const std::map<std::string, std::string>& options) override;
	virtual void Uninit() override;
	virtual void Run() override;

	Graphics::Renderer* GetRenderer() const { return m_renderer.get(); }
	Sim* GetSim() const { return m_sim; }
	UI::Context* GetUI() const { return m_ui.Get(); }
	ModelCache* GetModelCache() const { return m_modelCache.get(); }
	Random& GetRNG() { return m_rng; }

private:
	void HandleEvents();
	void InitLua();

	std::unique_ptr<Graphics::Renderer> m_renderer;
	std::unique_ptr<LuaConsole> m_console;
	std::unique_ptr<ModelCache> m_modelCache;
	RefCountedPtr<UI::Context> m_ui;
	UI::Label* m_fpsLabel;
	Sim* m_sim;
	Random m_rng;
};
}
