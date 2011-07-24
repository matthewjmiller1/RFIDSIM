
#ifndef PATH_LOSS_H
#define PATH_LOSS_H

#include <math.h>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

class WirelessCommSignal;
class PhysicalLayer;

/**
 * This computes the path loss for a given signal at a receiver.
 */
class PathLoss : boost::noncopyable {
public:
	/// Smart pointer that clients should use.
	typedef boost::shared_ptr<PathLoss> PathLossPtr;

	/// A destructor.
	virtual ~PathLoss();

	/**
	 * Compute the signal strength for the signal at the receiver.
	 * @param signal the signal being transmitted.
	 * @param receiver the object receiving the signal.
	 * @return the signal strength in Watts.
	 */
	virtual double getRecvdStrength(const WirelessCommSignal& signal,
		const PhysicalLayer& receiver) const = 0;
	
protected:

	/// Determine when debugging info gets printed.
	static const bool m_DEBUG_SIGNAL_STRENGTH = false;

	/// A constructor
	PathLoss();

private:

};
typedef boost::shared_ptr<PathLoss> PathLossPtr;

/**
 * This computes the received signal strength using the
 * Friis free space model.  
 * The equation for the model * is given by:
 * \f[
 * P_r = \frac{P_t G_t G_r \lambda^2}{(4\pi)^2 d^2 L}
 * \f]
 * where \f$P_r\f$ is the receiver power, \f$P_t\f$ is
 * the transmitted power, \f$G_t\f$ and \f$G_r\f$ are the
 * gains of the transmitter and receiver antennas, respectively,
 * \f$\lambda\f$ is the wavelength, \f$d\f$ is the distance
 * between the transmitter and receiver, and \f$L\f$ is the
 * loss factor.
 */
class FreeSpace : public PathLoss {
public:
	/// Smart pointer that clients should use.
	typedef boost::shared_ptr<FreeSpace> FreeSpacePtr;

	/**
	 * A factory method to ensure that all objects
	 * are created via \c new since we are using smart pointers. 
	 */
	static inline FreeSpacePtr create();

	/**
	 * A factory method to ensure that all objects
	 * are created via \c new since we are using smart pointers. 
	 * @param lossFactor the loss factor of the model (must be >= 1).
	 */
	static inline FreeSpacePtr create(double lossFactor);

	/**
	 * Compute the signal strength for the signal at the receiver.
	 * @param signal the signal being transmitted.
	 * @param receiver the object receiving the signal.
	 * @return the signal strength in Watts.
	 */
	virtual double getRecvdStrength(const WirelessCommSignal& signal,
		const PhysicalLayer& receiver) const;
	
protected:

	/// A constructor
	FreeSpace();

	/// A constructor.
	/// @param lossFactor the loss factor of the model (must be
	/// >= 1).
	FreeSpace(double lossFactor);

protected:

	/// The default loss factor.
	static const double m_DEFAULT_LOSS_FACTOR;

	/// The multiplier for how much signal loss a signal
	/// experiences.
	double m_lossFactor;

private:

};
typedef boost::shared_ptr<FreeSpace> FreeSpacePtr;

/**
 * This computes the received signal strength using the
 * TwoRay Ground Refelection model at longer distances
 * and FreeSpace for shorter distances.  The equation
 * for the Two-Ray model is given by:
 * \f[
 * P_r = \frac{P_t G_t G_r H_t H_r}{d^4 L}
 * \f]
 * where \f$P_r\f$ is the receiver power, \f$P_t\f$ is
 * the transmitted power, \f$G_t\f$ and \f$G_r\f$ are the
 * gains of the transmitter and receiver antennas, respectively,
 * \f$H_t\f$  and \f$H_r\f$ is the transmitter and receiver
 * antenna heights, \f$d\f$ is the distance
 * between the transmitter and receiver, and \f$L\f$ is the
 * loss factor.  By default, \f$H_t = H_r = 0.0\f$.
 */
class TwoRay : public FreeSpace {
public:
	/// Smart pointer that clients should use.
	typedef boost::shared_ptr<TwoRay> TwoRayPtr;

	/**
	 * A factory method to ensure that all objects
	 * are created via \c new since we are using smart pointers. 
	 */
	static inline TwoRayPtr create();

	/**
	 * A factory method to ensure that all objects
	 * are created via \c new since we are using smart pointers. 
	 * @param lossFactor the loss factor of the model (must be >= 1).
	 */
	static inline TwoRayPtr create(double lossFactor);

	/**
	 * Compute the signal strength for the signal at the receiver.
	 * @param signal the signal being transmitted.
	 * @param receiver the object receiving the signal.
	 * @return the signal strength in Watts.
	 */
	virtual double getRecvdStrength(const WirelessCommSignal& signal,
		const PhysicalLayer& receiver) const;
	
protected:

	/// A constructor
	TwoRay();

	/// A constructor.
	/// @param lossFactor the loss factor of the model (must be
	/// >= 1).
	TwoRay(double lossFactor);


private:

	static const double m_DEFAULT_ANTENNA_HEIGHT;

	/// The antenna height for the model in meters.
	double m_antennaHeight;

};
typedef boost::shared_ptr<TwoRay> TwoRayPtr;

/////////////////////////////////////////////////
// Inline Functions
/////////////////////////////////////////////////

inline FreeSpacePtr FreeSpace::create()
{
	FreeSpacePtr p(new FreeSpace());
	return p;
}

inline FreeSpacePtr FreeSpace::create(double lossFactor)
{
	FreeSpacePtr p(new FreeSpace(lossFactor));
	return p;
}

inline TwoRayPtr TwoRay::create()
{
	TwoRayPtr p(new TwoRay());
	return p;
}

inline TwoRayPtr TwoRay::create(double lossFactor)
{
	TwoRayPtr p(new TwoRay(lossFactor));
	return p;
}

/////////////////////////////////////////////////
// Overloaded Operators
/////////////////////////////////////////////////

#endif // PATH_LOSS_H

