
#ifndef EVENT_H
#define EVENT_H

#include <iostream>
using namespace std;
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include "sim_time.hpp"
#include "utility.hpp"

/**
 * The interface for events which are scheduled in the simulator's
 * event queue.
 * Individual sublcasses will override the execute function
 * to do their scheduled action.
 */
class Event : boost::noncopyable {
	// Only Simulator can set the event time.
	friend class Simulator;
public:
	/// Smart pointer that clients should use.
	typedef boost::shared_ptr<Event> EventPtr;

	/// A destructor.
	/// A virtual destructor is recommended since the
	/// class has virtual functions.
	virtual ~Event();

	/**
	 * The code that gets executed for the event.
	 * Subclasses can override this to define their
	 * own actions.
	 */
	virtual void execute() = 0;

	/**
	 * Get the time at which the event will fire.
	 * @return The time at which the event will fire.
	 */
	inline SimTime getFireTime() const;

	/**
	 * True if the event is currently pending in an event queue.
	 * @return true if the event is in an event queue.
	 */
	inline bool inEventQueue() const;

	// Needed for insertion in a priority_queue
	/// One event is less than another event if its fire time
	/// is smaller.
	inline bool operator< (const Event& e) const;

protected:

	/// A constructor.
	Event();

private:

	/// The time at which the event will fire.
	/// @see getFireTime()
	/// @see setFireTime()
	SimTime m_timeToFire;

	/// True if the event is currently in an event queue.
	/// @see inEventQueue()
	/// @see setInEventQueue()
	bool m_inEventQueue;

	/**
	 * Set the time at which the event will fire.
	 * This can only be set by simulator, which is a
	 * friend for this function. 
	 * @param newFireTime the time at which the event will fire.
	 * @see setFireTime()
	 */
	inline void setFireTime(const SimTime& newFireTime);

	/**
	 * Set whether an event is currently in an event queue.
	 * Assumes that the event is only handled by one event queue
	 * at any given time.
	 * @param inEventQueue true if the event is in an event queue.
	 */
	inline void setInEventQueue(const bool inEventQueue);

};
typedef boost::shared_ptr<Event> EventPtr;
typedef boost::shared_ptr<Event const> ConstEventPtr;

/////////////////////////////////////////////////
// Inline Functions
/////////////////////////////////////////////////

inline void Event::setFireTime(const SimTime& newFireTime) 
{
	m_timeToFire = newFireTime;
}

inline SimTime Event::getFireTime() const 
{
	return m_timeToFire;
}

inline bool Event::inEventQueue() const
{
	return m_inEventQueue;
}

inline void Event::setInEventQueue(const bool inEventQueue)
{
	m_inEventQueue = inEventQueue;
}

/////////////////////////////////////////////////
// Overloaded Operators
/////////////////////////////////////////////////

inline bool Event::operator< (const Event& e) const {
	return m_timeToFire < e.m_timeToFire; 
}

inline ostream& operator<< (ostream& s, const Event& event)
{
	return s<< "Event state (pointer=" << &event << ", time=" <<
		event.getFireTime() << ")";
}

/////////////////////////////////////////////////
// Subclasses
/////////////////////////////////////////////////

/**
 * This class does nothing when the \c execute function is
 * called.
 * This can be used with timers that require no action
 * upon firing.
 */
class DummyEvent : public Event {
public:
	/// Smart pointer that clients should use.
	typedef boost::shared_ptr<DummyEvent> DummyEventPtr;

	/**
	 * A factory method to ensure that all objects
	 * are created via \c new since we are using smart pointers. 
	 */
	static inline DummyEventPtr create()
	{
		DummyEventPtr p(new DummyEvent());
		return p;
	}

	/**
	 * The code that gets executed for the event.
	 * Does nothing for this class.
	 */
	void execute()
	{
		/*
		cout << "Executing event " << this << " at time " << 
			getFireTime() << ".\n";
		*/
	}
protected:

	/// A constructor.
	DummyEvent() 
		: Event() 
	{
	
	}
};
typedef boost::shared_ptr<DummyEvent> DummyEventPtr;

#endif // EVENT_H

