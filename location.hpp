
#ifndef LOCATION_H
#define LOCATION_H

#include <iostream>
using namespace std;
#include <math.h>
#include <boost/shared_ptr.hpp>

/**
 * Represents the geographic location of an object in
 * the simulator.
 */
class Location {
public:
	/// Smart pointer that clients should use.
	typedef boost::shared_ptr<Location> LocationPtr;
	
	/// A constructor
	/// All coordinates are set to 0.0.
	Location();

	/**
	 * A constructor.
	 * @param x x coordinate in meters.
	 * @param y y coordinate in meters
	 * @param z z coordinate in meters
	 */
	Location(float x, float y, float z);

	/**
	 * Get the distance in meters between two locations.
	 * @param loc1 one of the locations.
	 * @param loc2 the other location.
	 * @return the Euclidean distance in meters between the two locations.
	 */
	static inline float distance(const Location& loc1, 
		const Location& loc2);

	/**
	 * Reset the coordinates.
	 * @param x the new x coordinate in meters.
	 * @param y the new y coordinate in meters.
	 * @param z the new z coordinate in meters.
	 */
	inline void setCoordinates(float x, float y, float z);

	/**
	 * Get the x coordinate.
	 * @return the x coordinate in meters.
	 */
	inline float getX() const;

	/**
	 * Get the y coordinate.
	 * @return the y coordinate in meters.
	 */
	inline float getY() const;

	/**
	 * Get the z coordinate.
	 * @return the z coordinate in meters.
	 */
	inline float getZ() const;

private:

	/// The x coordinate in meters.
	/// @see setCoordinates()
	/// @see getX()
	float m_xCoordinate;

	/// The y coordinate in meters.
	/// @see setCoordinates()
	/// @see getY()
	float m_yCoordinate;

	/// The z coordinate in meters.
	/// @see setCoordinates()
	/// @see getZ()
	float m_zCoordinate;

};
typedef boost::shared_ptr<Location> LocationPtr;

/////////////////////////////////////////////////
// Inline Functions
/////////////////////////////////////////////////

inline float Location::distance(const Location& loc1, const Location& loc2)
{
	float xDiff = loc1.getX() - loc2.getX();
	float yDiff = loc1.getY() - loc2.getY();
	float zDiff = loc1.getZ() - loc2.getZ();
	return sqrt(pow(xDiff, 2) + pow(yDiff, 2) + pow(zDiff, 2));
}

inline void Location::setCoordinates(float x, float y, float z)
{
	m_xCoordinate = x;
	m_yCoordinate = y;
	m_zCoordinate = z;
}

inline float Location::getX() const
{
	return m_xCoordinate;
}

inline float Location::getY() const
{
	return m_yCoordinate;
}

inline float Location::getZ() const
{
	return m_zCoordinate;
}

/////////////////////////////////////////////////
// Overloaded Operators
/////////////////////////////////////////////////

inline ostream& operator<< (ostream& s, const Location& location)
{
	return s << "(x=" << location.getX() << ", y=" << 
		location.getY() << ", z=" << location.getZ() << ")";
}

#endif // LOCATION_H

