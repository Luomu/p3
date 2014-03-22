#pragma once
#include "p3/Game.h"

//acceptable globals:
//p3::game

namespace p3 {
	class Game;

	extern Game* game;

/*
	class Game : public Application { //Game application AKA Pi.pp, class-fied
		//has mainloop
		//has entity manager
		//has access to settings
		//has scene
	};

	class GameInstance {
		//current game
		//has space
		//has systems
	};

	//current system 3d space
	class Space {
		//has systems
	};

	class Scene {
		//has graphics
		//no systems?
	};

	class Galaxy {
		//has sectors
	};

	class Sector {
		//has star systems
		//has XYZ address
	};

	class System { //star system
		//has system bodies
	};
*/
}
