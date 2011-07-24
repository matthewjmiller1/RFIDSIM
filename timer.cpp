
#include "timer.hpp"

Timer::Timer(NodePtr owner, EventPtr eventOnFire) 
{
	assert(owner.get() != 0);
	assert(eventOnFire.get() != 0);

	m_eventOnFire = eventOnFire;
	m_owner = owner;

}

void Timer::setEvent(EventPtr eventOnFire)
{
	assert(eventOnFire != 0);
	// If the timer is currently running,
	// we need to stop it so that the old event
	// is removed from the event queue.
	if(isRunning()) {
		stop();
	}
	m_eventOnFire = eventOnFire;
}

