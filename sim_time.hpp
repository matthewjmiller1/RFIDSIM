
#ifndef SIM_TIME_H
#define SIM_TIME_H

#include <iostream>
using namespace std;

#include <boost/shared_ptr.hpp>

/**
 * Defines the class for managing time in the simulator.
 */
class SimTime {
public:

	/**
	 * A constructor.
	 * Time is set to zero by default.
	 */
	SimTime();
	/**
	 * A constructor.
	 * Sets time in seconds.
	 * @param simTime time (in seconds) to initialize object.
	 */
	SimTime(double simTime);

	/**
	 * Set the internal time by specifying microseconds.
	 * @param timeInMicroSeconds new time specified in microseconds.
	 */
	inline void setTimeInMicroSeconds(double timeInMicroSeconds);

	/**
	 * Set the internal time by specifying milliseconds.
	 * @param timeInMilliSeconds new time specified in milliseconds.
	 */
	inline void setTimeInMilliSeconds(double timeInMilliSeconds);

	/**
	 * Set the internal time by specifying seconds.
	 * @param timeInSeconds new time specified in seconds.
	 */
	inline void setTimeInSeconds(double timeInSeconds);

	/**
	 * Set the internal time by specifying minutes.
	 * Note: does not handle overflow.
	 * @param timeInMinutes new time specified in minutes.
	 */
	inline void setTimeInMinutes(double timeInMinutes);

	/**
	 * Get the internal time in microseconds.
	 * Note: does not handle overflow.
	 * @return time in microseconds.
	 */
	inline double getTimeInMicroSeconds() const;

	/**
	 * Get the internal time in milliseconds.
	 * Note: does not handle overflow.
	 * @return time in milliseconds.
	 */
	inline double getTimeInMilliSeconds() const;

	/**
	 * Get the internal time in seconds.
	 * @return time in seconds.
	 */
	inline double getTimeInSeconds() const;

	/**
	 * Get the internal time in minutes.
	 * @return time in minutes.
	 */
	inline double getTimeInMinutes() const;

	/**
	 * Set the internal time (by specifying seconds).
	 * @param timeInSeconds new time specified in seconds.
	 */
	inline void setTime(double timeInSeconds);

	/**
	 * Get the internal time (in seconds).
	 * @return time in seconds.
	 */
	inline double getTime() const;

	/**
	 * Check that the internal format of time is valid.
	 * Can be used in assert statements.
	 * @return whether or not the internal format is valid.
	 */
	inline bool isValid() const;

	//@{
	/// An operator for the class.
	inline SimTime& operator+= (const SimTime& rhs);
	inline SimTime& operator-= (const SimTime& rhs);
	inline bool operator< (const SimTime& rhs) const;
	inline bool operator<= (const SimTime& rhs) const;
	inline bool operator> (const SimTime& rhs) const;
	inline bool operator>= (const SimTime& rhs) const;
	//@}

private:
	double m_time;
};
typedef boost::shared_ptr<SimTime> SimTimePtr;

/////////////////////////////////////////////////
// Inline Functions
/////////////////////////////////////////////////

inline void SimTime::setTimeInMicroSeconds(double timeInMicroSeconds)
{
	assert(timeInMicroSeconds >= 0.0);
	m_time = (timeInMicroSeconds / 1000000.0);
}

inline void SimTime::setTimeInMilliSeconds(double timeInMilliSeconds)
{
	assert(timeInMilliSeconds >= 0.0);
	m_time = (timeInMilliSeconds / 1000.0);
}

inline void SimTime::setTimeInSeconds(double timeInSeconds)
{
	assert(timeInSeconds >= 0.0);
	m_time = timeInSeconds;
}

inline void SimTime::setTimeInMinutes(double timeInMinutes)
{
	assert(timeInMinutes >= 0.0);
	m_time = (60.0 * timeInMinutes);
}

inline double SimTime::getTimeInMicroSeconds() const
{
	return (1000000.0 * m_time);
}

inline double SimTime::getTimeInMilliSeconds() const
{
	return (1000.0 * m_time);
}

inline double SimTime::getTimeInSeconds() const
{
	return m_time;
}

inline double SimTime::getTimeInMinutes() const
{
	return (m_time / 60.0);
}

inline void SimTime::setTime(double timeInSeconds) 
{
	setTimeInSeconds(timeInSeconds);
}

inline double SimTime::getTime() const
{
	return getTimeInSeconds();
}

inline bool SimTime::isValid() const 
{
	// Right now, our only requirement is that
	// the time is not negative.
	return (m_time >= 0.0);
}

/////////////////////////////////////////////////
// Overloaded Operators
/////////////////////////////////////////////////

inline SimTime& SimTime::operator+= (const SimTime& rhs) 
{
	m_time += rhs.m_time;
	return *this;
}

inline SimTime& SimTime::operator-= (const SimTime& rhs) 
{
	m_time -= rhs.m_time;
	return *this;
}

inline bool SimTime::operator< (const SimTime& rhs) const 
{
	return m_time < rhs.m_time;
}

inline bool SimTime::operator<= (const SimTime& rhs) const 
{
	return m_time <= rhs.m_time;
}

inline bool SimTime::operator> (const SimTime& rhs) const 
{
	return m_time > rhs.m_time;
}

inline bool SimTime::operator>= (const SimTime& rhs) const 
{
	return m_time >= rhs.m_time;
}

// Note that this can't be included in the class definition, so we
// define it here so that files that include sim_time.hpp can 
// use it.
inline SimTime operator+ (const SimTime& lhs, const SimTime& rhs)
{
	SimTime returnVal = lhs;
	return returnVal += rhs;
}

inline SimTime operator- (const SimTime& lhs, const SimTime& rhs)
{
	SimTime returnVal = lhs;
	return returnVal -= rhs;
}

inline ostream& operator<< (ostream& s, const SimTime& st)
{
	return s<< st.getTimeInSeconds();
}

#endif // SIM_TIME_H

