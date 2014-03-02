#pragma once
#include "p3/common.h"
#include "pi/IniConfig.h"

namespace p3 {

class GameConfig : public IniConfig {
public:
	GameConfig(const std::map<std::string,std::string> &override_ = std::map<std::string,std::string>());

	void Load();
	bool Save();
};

}
