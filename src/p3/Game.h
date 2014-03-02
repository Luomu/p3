#pragma once
#include "p3/Application.h"
#include "p3/Sim.h"
#include "graphics/Renderer.h"
#include "ui/Context.h"

namespace p3
{
class Game : public Application
{
public:
	virtual void Init(const std::map<std::string, std::string>& options) override;
	virtual void Uninit() override;
	virtual void Run() override;

	Graphics::Renderer* GetRenderer() const { return m_renderer.get(); }
	Sim* GetSim() const { m_sim; }
	UI::Context* GetUI() const { return m_ui.Get(); }

private:
	void HandleEvents();
	void InitLua();

	std::unique_ptr<Graphics::Renderer> m_renderer;
	RefCountedPtr<UI::Context> m_ui;
	Sim* m_sim;
};
}
