
#ifndef SIMULATOR_H
#define SIMULATOR_H

/**
 * \mainpage RFIDSIM -- A Simulator for RFID Networks
 * \author Matthew J. Miller (matt@matthewjmiller.net)
 * \date 2006
 * \version 0.1
 *
 * \section sec_intro Introduction
 *
 * RFIDSIM is intended to be an extensible simulator for RFID readers
 * and tags as well as other types of wireless networks.
 *
 * Right now, it is in the very early stages and has not been
 * well-tested, but hopefully it can serve as a foundation for
 * someone looking to develop a more complete and robust RFID 
 * simulator.  It is released under the GNU General Public License
 * (http://www.gnu.org/licenses/gpl.html).
 *
 * \section sec_install Installation
 *
 * \subsection sec_install_step1 Step 1
 * Install the Boost C++ libraries on your system 
 * (http://www.boost.org/).  I used Boost version 1.33.1
 * in development.  In Debian, there are packages available
 * that can be installed using \c aptitude.
 *
 * \subsection sec_install_step2 Step 2
 * Run \c make.
 * 
 * \subsection sec_install_step3 Step 3
 * Modify main.cpp as necessary to create new scenarios.
 *
 * \section sec_channel RFID Channel Model
 *
 * Each PhysicalLayer can select on which channel it transmits
 * and receives independently.  We use this to model RFID networks
 * as follows.  There is one master channel on which all readers
 * transmit (but not receive) and all tags receive (but not transmit).
 * Then, each reader chooses a regular channel on which packets
 * are both send and received.
 * Whenever a tag successfully receives a packet on the master
 * channel, it will transmit a packet on the regular channel
 * of the reader that sent the packet.
 *
 * Thus, tags are essentially listening on all channels concurrently
 * and can experience collisions if multiple readers transmit
 * simultaneously (even if the readers' regular channels are
 * different).  Once the tag receives a reader's packet, it locks
 * on to that channel to transmit back the the reader.  The tag's
 * tranmission is only overheard by devices listening on the regular
 * channel of the reader to which the packet is transmitted.
 *
 * As we understand it, this is the correct channel model for RFIDs.
 * It has the important implication that readers cannot use different
 * channels for their transmissions in order to avoid interference
 * at tags.
 *
 * With respect to the transmit power, the tag is modelled as
 * reflecting the reader's tranmit power.  This is done by
 * setting the transmit power of a tag to the received signal
 * strength of the last packet that it received.
 *
 * One thing that we are \e not currently modelling is the fact
 * that tags must have a sufficiently strong culmulative
 * received energy or else they "turn off".
 *
 * \section sec_advantages Advantages Over ns-2
 *
 * Most of my previous development was in \c ns-2 
 * (http://nsnam.isi.edu/nsnam/), but developed this
 * to give several advantages over \c ns-2 (of course, there
 * are many disadvantages as well, see \ref sec_future_work).
 *
 * \li <b>More realistic physical layer</b>: I used the Qualnet model
 * of culmulative interference rather than \c ns-2's model of
 * only considering the interference between two packets.  Also,
 * a Ricean fading model is included by default.  This is based
 * on a \c ns-2 extension 
 * (http://www.ece.cmu.edu/wireless/downloads.html), but improves
 * on it by calculating the fading factor at a receiver as a function
 * of \e both the current time \e and the specific receiver for which
 * it is being calculated (by contrast, the \c ns-2 extension only
 * calculates the fading factor as a function of the current time).
 *
 * \li <b>Multi-interface and multi-channel support</b>: From
 * the beginning, I tried to make it easy to connect a network layer
 * to multiple upper or lower layers in the stack.  This allows,
 * for example, multiple physical layers (i.e., radios) per device.
 * Also, there is a WirelessChannelManager class to handle
 * multiple channels easier and more with more flexibility than
 * \c ns-2.
 *
 * \li <b>Smart pointers</b>: This is my primarly motivation for
 * using Boost because the entire project uses smart pointers
 * (with the exception of singleton classes).  This, by in large,
 * frees the developer from worrying about memory management.
 * Each pointer is reference counted and automatially \c delete'd
 * whenever the count reaches zero.  Thus, you'll notice there
 * are no explicit \c delete or \c free function calls in the code.
 * Additionally, there is no overhead for garbage collection.
 * The disadvantage of smart pointers is that cyclic references
 * can arise and must be dealt with using \c weak_ptr's or else
 * a memory leak may occur.  Right now, the code is simple enough
 * that such cycles rarely occur.  Another difficult is handling
 * the \c this pointer to an object, particularly when inheritance
 * is involved.  You can peruse the code to see the approach
 * I've taken to this problem.
 * 
 * \li <b>Better use of the C++ STL</b>: I tried to use the STL
 * wherever possible whereas \c ns-2 seems to use "homegrown" data
 * structures for the most part.  For example, my event scheduler
 * is built on the STL's \c multimap, which is very efficient, whereas
 * \c ns-2 creates its own heap for this purpose which can be more
 * bug-prone and exposes more code to the user.
 *
 * \li <b>Better object-oriented design</b>: Of course this is
 * subjective, but I tried to incorporate more object-oriented
 * design techniques than I have seen in the \c ns-2 wireless
 * classes.  I have used some design patterns, such as the singleton
 * and factory method, in the project.
 *
 * \li <b>Improved random number generator</b>:  I used the random
 * number generation library from Boost, which claims industrial
 * strength RNG.  By contrast, the RNG of \c ns-2 is based on the
 * default C++ RNG, which is not as strong.
 *
 * \section sec_future_work Future Work
 *
 * This project is far from complete and should only be used if you
 * are interested in developing it further.  Some work that would
 * benefit this project include:
 *
 * \li <b>Scripting front-end</b>: Right now, every separate scenario
 * must be recompiled which obviously is not feasible.  Some
 * possibilities for this include: (1) using the Python/C++ interface
 * in Boost or (2) designing a relatively simple scenario specification
 * file that can be input and parsed by the simulator (perhaps using
 * Boost's regex library).  I do not think that SWIG 
 * (http://www.swig.org/) would work since it has issues with C++ and
 * probably would not work with smart pointers.  IMHO, the best option
 * is probably (2) since it would give more portability (Boost's
 * Python library seems quite difficult to install) and easier to
 * debug.
 *
 * \li <b>Bit error model</b>: Right now there is no bit
 * error model.  Packets are received if their SINR is larger
 * than a statically specified capture threshold.
 *
 * \li <b> Queueing model</b>: Right now, there is a minimal
 * queueing model, but I feel that this needs improved for
 * experiments where queueing is a factor.
 *
 * \li <b>More protocols</b>: For RFID, a tree-walking
 * MAC protocol would be useful.  Beyond that, 802.11 and
 * some ad hoc routing protocols would be useful for ad hoc
 * network and wireless LAN simulations.
 *
 * \li <b>Energy model</b>: A model should be added to track
 * the energy consumption of various commands.
 *
 * \li <b>More examples</b>: Right now, the only scenario example
 * is that of main.cpp.  After a scripting front-end is added,
 * creating more examples would be useful.
 *
 * \section sec_license GNU General Public License
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
 * 02110-1301, USA.
 *
 */

#include <vector>
#include <set>
using namespace std;
#include <boost/utility.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "utility.hpp"
#include "sim_time.hpp"
#include "event.hpp"
#include "node.hpp"
#include "simulation_end_listener.hpp"

class RandNumGenerator;
typedef boost::shared_ptr<RandNumGenerator> RandNumGeneratorPtr;
class LogStreamManager;
typedef LogStreamManager* LogStreamManagerPtr;

/////////////////////////////////////////////////
// EventPtrComparator Class
/////////////////////////////////////////////////

/**
 * Helper class to define how Event objects are compared for use
 * with the Simulator's Event queue.
 * This is necessary since the queue contains pointers that
 * must be dereferenced before they can be compared.
 */
class EventPtrComparator {
public:
	/**
	 * Operator to prioritize event pointers since they
	 * must be dereference before applying the less than
	 * operator.
	 */
	inline bool operator() (ConstEventPtr event1, 
		ConstEventPtr event2) const;
};

inline bool EventPtrComparator::operator() (ConstEventPtr event1, 
	ConstEventPtr event2) const
{
	return *event1 < *event2;
}

/////////////////////////////////////////////////
// Simulator Class
/////////////////////////////////////////////////

typedef multiset<EventPtr,EventPtrComparator> EventPtrQueue;
typedef EventPtrQueue::iterator EventPtrQueueIterator;

/**
 * The main Simulator which class contains the event queues, nodes,
 * and channels.
 */
// Declare as noncopyable so the copy constructor and copy 
// assignment cannot be used.
class Simulator : boost::noncopyable, 
		public boost::enable_shared_from_this<Simulator> {
public:
	/// Pointer that clients should use.
	typedef Simulator* SimulatorPtr;

	/**
	 * The class uses the Singleton design pattern.
	 * @return Pointer to single instance of Simulator
	 */
	static inline SimulatorPtr instance();

	/**
	 * Associate a node with this simulator object.
	 * @param nodeToAdd the node that will be associated.
	 */
	inline void addNode(NodePtr nodeToAdd);

	/**
	 * Execute events on the event queue.
	 * While the event queue is not empty, this will continue
	 * to pop and execute events.
	 * After the simulation object is set up, this is called
	 * to start executing the events.  The event queue is
	 * in ascending order by the execution time of events.
	 * @param stopTime the time at which the simulation should
	 * stop.
	 * @see scheduleEvent()
	 */
	void runSimulation(const SimTime& stopTime);

	/**
	 * Get the current time in the simulator.
	 * @return The current time in the simulator.
	 */
	inline SimTime currentTime() const;

	/**
	 * Add an event to the event queue.
	 * The event is pushed onto the event queue and scheduled
	 * to fire after eventDelay in the future.
	 * @param eventToSchedule a pointer to the event being scheduled.
	 * @param eventDelay how far in the future the event should be
	 * scheduled.  eventDelay is added to the currentTime to schedule
	 * the time at which the event will fire.
	 * @return true if event was scheduled succesfully.
	 * @see cancelEvent()
	 * @see currentTime()
	 * @see runSimulation()
	 */
	inline bool scheduleEvent(EventPtr eventToSchedule, 
		const SimTime& eventDelay);

	/**
	 * Cancel an event from the event queue.
	 * If the event pointer exists in the queue, it is removed.
	 * @param eventToCancel a pointer to the event being cancelled.
	 * @return true if the event was found and erased.
	 * @see scheduleEvent()
	 * @see currentTime()
	 * @see runSimulation()
	 */
	inline bool cancelEvent(EventPtr eventToCancel);

	/**
	 * Set the simulator's log stream manager.
	 * @param logStreamManager a pointer to the simulator's 
	 * log stream manager.
	 */
	inline void setLogStreamManager(
			LogStreamManagerPtr logStreamManager) const;

	/**
	 * Get a pointer to the simulator's log stream manager.
	 * @return pointer to the simulator's log stream manager.
	 */
	inline LogStreamManagerPtr getLogStreamManager() const;

	/**
	 * Seed the random number generator.
	 * @param seed the new seed.
	 * @see getRandNumGenerator()
	 */
	void seedRandNumGenerator(const t_uint seed) const;

	/**
	 * Get a pointer to the simulator's random number generator.
	 * @return pointer to the simulator's random number generator.
	 */
	inline RandNumGeneratorPtr getRandNumGenerator() const;

	/**
	 * Reset the state of the simulator to its initial state.
	 * This clears the event queue and resets the clock to
	 * the original start time.
	 */
	void reset();

	/**
	 * Add a listener that will be notified when the simulation
	 * ends.
	 * @param listener a pointer to the object to be notified.
	 */
	inline void addSimulationEndListener(
		SimulationEndListenerPtr listener);

private:

	/// The default start time for the simulator.
	static const double m_SIM_START_TIME;

	/// The lone instance of the simulator class.
	/// @see instance()
	static SimulatorPtr m_instance;

	/// The master clock of the simulator.
	/// @see currentTime()
	SimTime m_clock;

	/// The event queue.
	/// @see runSimulation()
	/// @see scheduleEvent()
	/// @see dispatchEvent()
	EventPtrQueue m_eventPtrQueue;

	/// The global interface to output to log files.
	/// @see setLogStreamManager()
	/// @see getLogStreamManager()
	LogStreamManagerPtr m_logStreamManagerPtr;

	/// The global random number generator for the simulator.
	/// @see getRandNumGenerator()
	/// @see seedRandNumGenerator()
	RandNumGeneratorPtr m_randNumGeneratorPtr;

	/// The set of objects to be notified when the simulation
	/// ends.
	/// @see addSimulationEndListener()
	vector<SimulationEndListenerPtr> m_simulationEndListeners;

	/// A constructor.
	Simulator();

	/// A destructor.
	/// This is private to avoid smart pointers to this
	/// class since it is a Singleton.
	~Simulator();

	/**
	 * Execute an event.
	 * Moves the clock forward to the event's execution time and
	 * calls the event's execute() function.
	 * @param event event to be executed.
	 * @see getNextEvent()
	 * @see runSimulation()
	 * @see scheduleEvent()
	 */
	inline void dispatchEvent(EventPtr event);

	/**
	 * Remove and return the next event on the queue.
	 * @return pointer to next event on the queue.
	 * @see dispatchEvent()
	 */
	inline EventPtr getNextEvent();

};
typedef Simulator* SimulatorPtr;

/////////////////////////////////////////////////
// Inline Functions
/////////////////////////////////////////////////

inline SimTime Simulator::currentTime() const
{
	return m_clock;
}

inline void Simulator::addNode(NodePtr nodeToAdd)
{
	// May keep track of the nodes globally in the future.
}

inline bool Simulator::scheduleEvent(EventPtr eventToSchedule, 
	const SimTime& eventDelay)
{
	assert(eventToSchedule != 0);
	assert(!eventToSchedule->inEventQueue());
	assert(eventDelay >= 0.0);

	eventToSchedule->setFireTime(currentTime() + eventDelay);
	EventPtrQueue::const_iterator insertIterator = 
		m_eventPtrQueue.insert(eventToSchedule);
	bool didInsert = (insertIterator != m_eventPtrQueue.end());
	if(didInsert) {
		eventToSchedule->setInEventQueue(true);
	}
	
	return didInsert;

}

inline bool Simulator::cancelEvent(EventPtr eventToCancel)
{
	assert(eventToCancel != 0);

	// Get the range of elements that match this key (i.e.,
	// have the same fire time).
	pair<EventPtrQueueIterator,EventPtrQueueIterator> keyIteratorPair =
		m_eventPtrQueue.equal_range(eventToCancel);
		
	bool didErase = false;
	// If the key was not found, pair.first == pair.second
	for(EventPtrQueueIterator keyIterator = keyIteratorPair.first;
			keyIterator != keyIteratorPair.second; 
			++keyIterator) {

		// If we iterator finds the pointer we want to delete,
		// then erase the element and return.
		if(*keyIterator == eventToCancel) {
			m_eventPtrQueue.erase(keyIterator);
			eventToCancel->setInEventQueue(false);
			didErase = true;
			break;
		}
	}

	return didErase;
}

inline RandNumGeneratorPtr Simulator::getRandNumGenerator() const
{
	assert(m_randNumGeneratorPtr != 0);
	return m_randNumGeneratorPtr;
}

inline LogStreamManagerPtr Simulator::getLogStreamManager() const
{
	assert(m_logStreamManagerPtr != 0);
	return m_logStreamManagerPtr;
}

inline SimulatorPtr Simulator::instance() 
{
	// See the Singleton design pattern for an explanation.
	if(m_instance == 0) {
		m_instance = new Simulator();
	}
	return m_instance;
}

inline void Simulator::dispatchEvent(EventPtr event)
{
	assert(event.get() != 0);
	SimTime fireTime = event->getFireTime();
	assert(m_clock <= fireTime);

	m_clock = fireTime;
	event->execute();
}

inline EventPtr Simulator::getNextEvent()
{
	EventPtr nextEvent (*m_eventPtrQueue.begin());
	m_eventPtrQueue.erase(m_eventPtrQueue.begin());
	nextEvent->setInEventQueue(false);
	return nextEvent;
}

inline void Simulator::addSimulationEndListener(
	SimulationEndListenerPtr listener)
{
	m_simulationEndListeners.push_back(listener);
}

#endif // SIMULATOR_H

