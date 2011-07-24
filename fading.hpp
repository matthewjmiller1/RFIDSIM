
#ifndef FADING_H
#define FADING_H

#include <map>
using namespace std;
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include "utility.hpp"

class WirelessCommSignal;
class PhysicalLayer;
class NodeId;

/**
 * This computes the fading for a given signal at a receiver.
 */
class Fading : boost::noncopyable {
public:
	/// Smart pointer that clients should use.
	typedef boost::shared_ptr<Fading> FadingPtr;

	/// A destructor.
	virtual ~Fading();

	/**
	 * Compute the fading factor for the signal.
	 * @param signal the signal being transmitted.
	 * @param nodeId the ID of the receiver for which fading
	 * is being calculated.
	 * @return the fading factor.
	 */
	virtual double fadingFactor(const WirelessCommSignal& signal,
		const NodeId& nodeId) = 0;

protected:

	/// To avoid a global correlation in fading, we need
	/// a randomly choosen offset for each receiver.
	map<NodeId,int> m_nodeOffset;

	/// A constructor
	Fading();

	/// A copy constructor
	Fading(const Fading& rhs);

private:

	/// Declare as private to restrict use.
	Fading& operator= (const Fading& rhs);

};
typedef boost::shared_ptr<Fading> FadingPtr;

/**
 * The Ricean fading model.
 * This is based on the ns-2 implementation from the paper
 * <em>Efficient Simulation of %Ricean %Fading within a %Packet
 * %Simulator</em> by Punnoose et al.  See the paper for details
 * about the model.
 */
class Ricean : public Fading {
public:
	/// Smart pointer that clients should use.
	typedef boost::shared_ptr<Ricean> RiceanPtr;

	/**
	 * A factory method to ensure that all objects
	 * are created via \c new since we are using smart pointers. 
	 */
	static inline RiceanPtr create();

	/**
	 * A factory method to ensure that all objects
	 * are created via \c new since we are using smart pointers. 
	 */
	static inline RiceanPtr create(const Ricean& rhs);

	/**
	 * A factory method to ensure that all objects
	 * are created via \c new since we are using smart pointers. 
	 * @param maxVelocity the maximum velocity for objects in
	 * the environment in m/s (used for computing doppler).
	 * @param k a Ricean parameter.
	 */
	static inline RiceanPtr create(double maxVelocity, double k);

	/**
	 * Compute the fading factor for the signal.
	 * @param signal the signal being transmitted.
	 * @param nodeId the ID of the receiver for which fading
	 * is being calculated.
	 * @return the fading factor.
	 */
	virtual double fadingFactor(const WirelessCommSignal& signal, 
		const NodeId& nodeId);

protected:

	/// A constructor
	Ricean();

	/// A constructor.
	/// @param maxVelocity the maximum velocity for objects in
	/// the environment in m/s (used for computing doppler).
	/// @param k a Ricean parameter.
	Ricean(double maxVelocity, double k);

	/// A copy constructor
	Ricean(const Ricean& rhs);

	/// The default maximum velocity of objects in the
	/// environment.
	static const double m_DEFAULT_MAX_VELOCITY;

private:

	/// The default \e k value for the Ricean model.
	static const double m_DEFAULT_K;

	/// The number of points in the Gaussian component
	/// table we will use.
	static const t_uint m_NUMBER_OF_POINTS;

	/// The maximum Doppler frequency from the Gaussian component
	/// table.
	static const double m_MAX_DOPPLER_FREQUENCY;

	/// The sampling rate from the Gaussian component table.
	static const double m_MAX_SAMPLE_RATE;

	/// The table where the Gaussian component data is stored.
	static const double m_GAUSSIAN_DATA1[];

	/// The table where the Gaussian component data is stored.
	static const double m_GAUSSIAN_DATA2[];

	/// The maximum velocity for objects in
	/// the environment in m/s (used for computing doppler).
	double m_maxVelocity;

	/// The \e k parameter for the Ricean model.
	double m_kParameter;

	/// Declare as private to restrict use.
	Ricean& operator= (const Ricean& rhs);

};
typedef boost::shared_ptr<Ricean> RiceanPtr;

/**
 * Computes fading according to the Rayleigh model.
 */
class Rayleigh : public Ricean {
public:
	/// Smart pointer that clients should use.
	typedef boost::shared_ptr<Rayleigh> RayleighPtr;

	/**
	 * A factory method to ensure that all objects
	 * are created via \c new since we are using smart pointers. 
	 */
	static inline RayleighPtr create();

	/**
	 * A factory method to ensure that all objects
	 * are created via \c new since we are using smart pointers. 
	 */
	static inline RayleighPtr create(const Rayleigh& rhs);

	/**
	 * A factory method to ensure that all objects
	 * are created via \c new since we are using smart pointers. 
	 * @param maxVelocity the maximum velocity for objects in
	 * the environment in m/s (used for computing doppler).
	 */
	static inline RayleighPtr create(double maxVelocity);

protected:

	/// A constructor
	Rayleigh();

	/// A constructor.
	/// @param maxVelocity the maximum velocity for objects in
	/// the environment in m/s (used for computing doppler).
	Rayleigh(double maxVelocity);

	/// A copy constructor
	Rayleigh(const Rayleigh& rhs);

private:

	/// Declare as private to restrict use.
	Rayleigh& operator= (const Rayleigh& rhs);

};
typedef boost::shared_ptr<Rayleigh> RayleighPtr;

/////////////////////////////////////////////////
// Inline Functions
/////////////////////////////////////////////////

inline RiceanPtr Ricean::create()
{
	RiceanPtr p(new Ricean());
	return p;
}

inline RiceanPtr Ricean::create(double maxVelocity, double k)
{
	RiceanPtr p(new Ricean(maxVelocity, k));
	return p;
}

inline RiceanPtr Ricean::create(const Ricean& rhs)
{
	RiceanPtr p(new Ricean(rhs));
	return p;
}

inline RayleighPtr Rayleigh::create()
{
	RayleighPtr p(new Rayleigh());
	return p;
}

inline RayleighPtr Rayleigh::create(double maxVelocity)
{
	RayleighPtr p(new Rayleigh(maxVelocity));
	return p;
}

inline RayleighPtr Rayleigh::create(const Rayleigh& rhs)
{
	RayleighPtr p(new Rayleigh(rhs));
	return p;
}

/////////////////////////////////////////////////
// Overloaded Operators
/////////////////////////////////////////////////

#endif // FADING_H

