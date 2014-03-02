#include "p3/Sim.h"

namespace p3 {

Sim::Sim()
	: m_running(true)
{

}

void Sim::Execute(double time)
{
	static double tim = 0;
	tim += time;
	if (tim > 5) {
		printf("%f\n", tim);
		m_running = false;
	}
}

void Sim::End()
{
	m_running = false;
}

}
