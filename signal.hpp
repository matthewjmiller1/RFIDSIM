
#ifndef SIGNAL_H
#define SIGNAL_H

#include <iostream>
using namespace std;
#include <boost/shared_ptr.hpp>

#include "location.hpp"
#include "utility.hpp"

class WirelessCommSignal;

/**
 * A signal that can be transmitted over a channel.
 */
class Signal {
	friend ostream& operator<< (ostream& s, const Signal& rhs);
	friend ostream& operator<< (ostream& s, const WirelessCommSignal& rhs);
public:
	/// Smart pointer that clients should use.
	typedef boost::shared_ptr<Signal> SignalPtr;

	/// A destructor.
	virtual ~Signal();

	/**
	 * A factory method to ensure that all objects
	 * are created via \c new since we are using smart pointers. 
	 */
	static inline SignalPtr create(const Location& location, 
		double dbStrength);
	
	/**
	 * A factory method to ensure that all objects
	 * are created via \c new since we are using smart pointers. 
	 */
	static inline SignalPtr create(const Signal& rhs);

	/**
	 * Get the geographic location of the signal source.
	 * @return a pointer to the location object.
	 */
	inline Location getLocation() const;

	/**
	 * Get the strength in decibels of this signal.
	 * @return the strength in decibels.
	 */
	inline double getDbStrength() const;

protected:

	/// A constructor
	Signal(const Location& location, double dbStrength);

	/// A copy constructor.
	Signal(const Signal& rhs);

	/**
	 * Returns a pointer to a deep copy of this object.
	 * This is addresses the slicing problem with
	 * copy construction.
	 * @return a pointer to a deep copy of this object.
	 */
	virtual SignalPtr clone() const;

private:

	/// A pointer to the location of the signal.
	/// @see getLocation()
	Location m_location;

	/// The signal strength in decibels of the signal.
	/// @see getDbStrength()
	double m_dbStrength;

	/// Declare private to restrict use.
	Signal& operator= (const Signal& rhs);

};
typedef boost::shared_ptr<Signal> SignalPtr;

/////////////////////////////////////////////////
// Inline Functions
/////////////////////////////////////////////////

inline SignalPtr Signal::create(const Location& location, 
	double dbStrength)
{
	SignalPtr p(new Signal(location, dbStrength));
	return p;
}

inline SignalPtr Signal::create(const Signal& rhs)
{
	return rhs.clone();
}

inline Location Signal::getLocation() const
{
	return m_location;
}

inline double Signal::getDbStrength() const
{
	return m_dbStrength;
}

/////////////////////////////////////////////////
// Overloaded Operators
/////////////////////////////////////////////////

inline ostream& operator<< (ostream& s, const Signal& rhs)
{
	return s << "Signal state (pointer= " << &rhs <<
		", location=(" << rhs.m_location << 
		"), db strength= " << rhs.m_dbStrength << ")";
}

#endif // SIGNAL_H

