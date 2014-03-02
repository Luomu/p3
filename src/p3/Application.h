#pragma once
#include "Common.h"
#include "pi/IniConfig.h"
#include "graphics/Graphics.h"

namespace p3
{

class Application
{
public:
	Application();
	virtual ~Application();

	virtual void Init(const std::map<std::string, std::string>& options) = 0;
	virtual void Uninit() = 0;
	virtual void Run() = 0;

	IniConfig* GetConfig() const { return m_config.get(); }

protected:
	Graphics::Settings ReadVideoSettings(IniConfig*);

	std::unique_ptr<IniConfig> m_config;
};

}
