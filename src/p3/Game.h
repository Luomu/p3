#pragma once
#include "p3/Application.h"
#include "p3/Sim.h"
#include "graphics/Renderer.h"

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

private:
	std::unique_ptr<Graphics::Renderer> m_renderer;
	Sim* m_sim;
};
}
