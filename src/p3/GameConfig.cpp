#include "p3/GameConfig.h"
#include "pi/FileSystem.h"

namespace p3
{

GameConfig::GameConfig(const std::map<std::string, std::string>& override_)
{
	// set defaults
	std::map<std::string, std::string>& map = m_map[""];
	map["Lang"] = "en";
	map["StartFullscreen"] = "0";
	map["ScrWidth"] = "800";
	map["ScrHeight"] = "600";
	map["FOVVertical"] = "65";
	map["MaxPhysicsCyclesPerRender"] = "4";
	map["AntiAliasingMode"] = "2";
	map["VSync"] = "1";
	map["UseTextureCompression"] = "0";

#ifdef _WIN32
	map["RedirectStdio"] = "1";
#else
	map["RedirectStdio"] = "0";
#endif
	map["EnableGLDebug"] = "1";

	Load();

	for (auto i = override_.begin(); i != override_.end(); ++i) {
		const std::string& key = (*i).first;
		const std::string& val = (*i).second;
		map[key] = val;
	}
}

void GameConfig::Load()
{
	Read(FileSystem::userFiles, "config.ini");
}

bool GameConfig::Save()
{
	return Write(FileSystem::userFiles, "config.ini");
}

}
