
#ifndef SIMULATOR_END_LISTENER_H
#define SIMULATOR_END_LISTENER_H

#include <boost/enable_shared_from_this.hpp>

/////////////////////////////////////////////////
// SimulationEndHandler Interface
/////////////////////////////////////////////////

/**
 * This is an interface for classes which can be notified
 * when the simulation ends.
 * Each object that implements this interface \e must add
 * itself to be notified by the simulator.
 * N.B.: This should be treated as an interface and neither
 * state nor function definitions should be added to it.
 * @see Simulator::addSimulationEndListener()
 */
class SimulationEndListener
{
public:

	/// A destructor.
	virtual ~SimulationEndListener() {}

	/**
	 * The function called when the simulation ends.
	 */
	virtual void simulationEndHandler() = 0;

protected:

	/// A constructor.
	SimulationEndListener() {}

private:
	// Make the class unable to be copied
	SimulationEndListener(const SimulationEndListener& rhs);
	SimulationEndListener& operator= (const SimulationEndListener& rhs);
};
typedef boost::shared_ptr<SimulationEndListener> 
	SimulationEndListenerPtr;

#endif // SIMULATOR_END_LISTENER_H

