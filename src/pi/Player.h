// Copyright © 2008-2014 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#ifndef _PLAYER_H
#define _PLAYER_H

#include "libs.h"
#include <list>
#include "HyperspaceCloud.h"
#include "Ship.h"
#include "ShipController.h"
#include "galaxy/StarSystem.h"

namespace Graphics { class Renderer; }

class Player: public Ship {
public:
	OBJDEF(Player, Ship, PLAYER);
	Player(ShipType::Id shipId);
	Player() {}; //default constructor used before Load
	virtual bool SetWheelState(bool down); // returns success of state change, NOT state itself
	virtual void NotifyRemoved(const Body* const removedBody);

	PlayerShipController *GetPlayerController() const;
	//XXX temporary things to avoid causing too many changes right now
	Body *GetCombatTarget() const;
	Body *GetNavTarget() const;
	Body *GetSetSpeedTarget() const;
	void SetCombatTarget(Body* const target, bool setSpeedTo = false);
	void SetNavTarget(Body* const target, bool setSpeedTo = false);

	virtual Ship::HyperjumpStatus InitiateHyperjumpTo(const SystemPath &dest, int warmup_time, double duration, LuaRef checks);
	virtual Ship::HyperjumpStatus StartHyperspaceCountdown(const SystemPath &dest);

protected:
	virtual void OnEnterSystem();
	virtual void OnEnterHyperspace();
};

#endif /* _PLAYER_H */
