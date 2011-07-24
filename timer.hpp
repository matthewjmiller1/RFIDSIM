
#ifndef TIMER_H
#define TIMER_H

#include <iostream>
using namespace std;
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include "node.hpp"
#include "event.hpp"
#include "sim_time.hpp"

/**
 * Provides an interface to control events based on timers.
 * An event is set for the timer and then the functions control
 * the timer's operation for when the event is executed.
 */
class Timer : boost::noncopyable {
public:
	/// Smart pointer that clients should use.
	typedef boost::shared_ptr<Timer> TimerPtr;

	/**
	 * A factory method to ensure that all objects
	 * are created via \c new since we are using smart pointers. 
	 * @param owner the node that owns this timer.
	 * @param eventOnFire the event that will be executed
	 * whenever this timer fires.
	 */
	static inline TimerPtr create(NodePtr owner, EventPtr eventOnFire);

	/**
	 * Starts the timer running.
	 * The event is not scheduled if the timer is already running.
	 * In this case, reschedule() should be used instead.
	 * @param delay the time in the future when the event will
	 * be executed.  If the current clock value for the node is
	 * currentTime(), the event will be executed at currentTime()
	 * + delay.
	 * @return true if the timer is not already running and the
	 * event was successfully scheduled.
	 * @see stop()
	 * @see reschedule()
	 * @see isRunning()
	 * @see setEvent()
	 */
	inline bool start(const SimTime& delay) const;

	/**
	 * Stops the timer.
	 * The timer is stopped if it is currently running.
	 * @return true if the timer is running and the event is
	 * successfully cancelled.
	 * @see start()
	 * @see reschedule()
	 * @see isRunning()
	 */
	inline bool stop() const;

	/**
	 * Reschedules the timer for the new delay.
	 * The new value is scheduled regardless of whether the
	 * timer is running.  If it is running, the timer is stopped
	 * and then started with the new value.
	 * @return true if a running timer was sucessfully stopped and
	 * and then started with the new delay or a non-running timer
	 * was started with the new delay.
	 * @see start()
	 * @see stop()
	 * @see isRunning()
	 */
	inline bool reschedule(const SimTime& delay) const;

	/**
	 * Check if the timer is currently running.
	 * The timer is considered to be running if the timer's
	 * event is currently in an event queue.
	 * @return true if the timer's event is in an event queue.
	 * @see start()
	 * @see stop()
	 * @see reschedule()
	 */
	inline bool isRunning() const;

	/**
	 * Get the time remaining on the timer.
	 * @return the time left until the event is executed or zero
	 * if the timer is not running.
	 * @see start()
	 * @see stop()
	 * @see reschedule()
	 * @see isRunning()
	 */
	inline SimTime timeRemaining() const;

	/**
	 * Change the event executed when the timer fires.
	 * If the timer is currently running, it will be stopped
	 * and then the new event added to the timer.  If the
	 * new event needs to be scheduled, start() must be called
	 * separately.
	 * @param eventOnFire the event that will be executed whenever
	 * the timer fires.
	 * @see start()
	 * @see stop()
	 * @see reschedule()
	 */
	void setEvent(const EventPtr eventOnFire);

protected:

	/// A constructor.
	/// @param owner the node that owns this timer.
	/// @param eventOnFire the event that will be executed
	/// whenever this timer fires.
	Timer(NodePtr owner, EventPtr eventOnFire);

private:

	/// The node to which this timer belongs.
	NodePtr m_owner;

	/// The event that will be executed when the timer fires.
	/// @see setEvent()
	EventPtr m_eventOnFire;

};
typedef boost::shared_ptr<Timer> TimerPtr;

/////////////////////////////////////////////////
// Inline Functions
/////////////////////////////////////////////////

inline TimerPtr Timer::create(NodePtr owner, EventPtr eventOnFire)
{
	TimerPtr p(new Timer(owner, eventOnFire));
	return p;
}

inline bool Timer::start(const SimTime& delay) const
{
	assert(m_owner != 0);
	assert(m_eventOnFire != 0);

	bool didScheduleEvent = false;
	if(!isRunning()) {
		didScheduleEvent = m_owner->scheduleEvent(m_eventOnFire, delay);
	}
	return didScheduleEvent;
}

inline bool Timer::stop() const
{
	assert(m_owner != 0);
	assert(m_eventOnFire != 0);

	bool didCancelEvent = false;
	if(isRunning()) {
		didCancelEvent = m_owner->cancelEvent(m_eventOnFire);
	}
	return didCancelEvent;
}

inline bool Timer::reschedule(const SimTime& delay) const
{
	bool wasSuccessful = true;
	if(isRunning()) {
		wasSuccessful &= stop();
	}
	wasSuccessful &= start(delay);

	return wasSuccessful;

}

inline bool Timer::isRunning() const
{
	assert(m_eventOnFire != 0);
	return m_eventOnFire->inEventQueue();
}

inline SimTime Timer::timeRemaining() const
{
	assert(m_owner != 0);
	assert(m_eventOnFire != 0);

	SimTime timeLeft(0.0);
	if(isRunning()) {
		timeLeft = m_eventOnFire->getFireTime() - m_owner->currentTime();
		assert(timeLeft.isValid());
	}

	return timeLeft;
}

/////////////////////////////////////////////////
// Overloaded Operators
/////////////////////////////////////////////////

inline ostream& operator<< (ostream& s, const Timer& timer)
{
	return s<< "Timer state (pointer=" << &timer << ") isRunning=" <<
		timer.isRunning() << " timeRemaining=" << timer.timeRemaining();
}

#endif // TIMER_H

