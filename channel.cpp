
#include "channel.hpp"
#include "signal.hpp"
#include "physical_layer.hpp"

Channel::Channel()
{

}

Channel::~Channel()
{

}

SimTime Channel::propagationDelay(const PhysicalLayer& sender, 
	const PhysicalLayer& receiver) const
{
	float distanceInMeters = Location::distance(
		sender.getLocation(), receiver.getLocation());
	SimTime delay(distanceInMeters / SPEED_OF_LIGHT);
	return delay;
}


