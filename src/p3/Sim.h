#pragma once
#include "p3/Common.h"

namespace p3 {

/**
 * Main simulation (game instance)
 *  - has a Space instance (system player is in)
 *  - may run galaxy simulation
 */
class Sim {
public:
	Sim();
	void Execute(double time);
	void End();

	bool IsRunning() const { return m_running; }
	double GetTimeAccelRate() const { return 1.0; }
	double GetTimeStep() const { return GetTimeAccelRate() * 1.0/60.0; }

private:
	bool m_running;
};
}
