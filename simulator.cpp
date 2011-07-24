
#include "simulator.hpp"
#include "rand_num_generator.hpp"

SimulatorPtr Simulator::m_instance;
const double Simulator::m_SIM_START_TIME = 0.0;

Simulator::Simulator() 
{
	m_clock.setTime(m_SIM_START_TIME);
	m_randNumGeneratorPtr = RandNumGenerator::create();
}

Simulator::~Simulator()
{

}

void Simulator::runSimulation(const SimTime& stopTime)
{
	while(!m_eventPtrQueue.empty()) {
		EventPtr nextEvent = getNextEvent();
		if(nextEvent->getFireTime() > stopTime) {
			break;
		}
		dispatchEvent(nextEvent);
	}
	m_clock = stopTime;
	// Notifiy all listeners that the simulation has ended
	for(t_uint i = 0; i < m_simulationEndListeners.size(); i++)
		m_simulationEndListeners[i]->simulationEndHandler();
}

void Simulator::seedRandNumGenerator(const t_uint seed) const
{
	assert(m_randNumGeneratorPtr != 0);
	m_randNumGeneratorPtr->setSeed(seed);
}

void Simulator::reset()
{
	// Because we're using smart pointers,
	// we don't have to worry about deleting
	// all the objects pointed to.
	m_eventPtrQueue.clear();
	m_clock.setTime(m_SIM_START_TIME);
}

