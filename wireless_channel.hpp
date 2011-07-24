
#ifndef WIRELESS_CHANNEL_H
#define WIRELESS_CHANNEL_H

#include <boost/shared_ptr.hpp>

#include "channel.hpp"

class PathLoss;
typedef boost::shared_ptr<PathLoss> PathLossPtr;
class Fading;
typedef boost::shared_ptr<Fading> FadingPtr;
class WirelessCommSignal;
typedef boost::shared_ptr<WirelessCommSignal> WirelessCommSignalPtr;

/**
 * Defines the channel used for radio transmissions over
 * a wireless channel.
 */
class WirelessChannel : public Channel {
public:
	/// Smart pointer that clients should use.
	typedef boost::shared_ptr<WirelessChannel> WirelessChannelPtr;

	/**
	 * A factory method to ensure that all objects
	 * are created via \c new since we are using smart pointers. 
	 * @param pathLossModel the path loss model that the
	 * channel will use.
	 */
	static inline WirelessChannelPtr create(PathLossPtr pathLossModel);

	/**
	 * A factory method to ensure that all objects
	 * are created via \c new since we are using smart pointers. 
	 * @param pathLossModel the path loss model that the
	 * channel will use.
	 * @param fadingModel the fading model that the channel will use.
	 */
	static inline WirelessChannelPtr create(PathLossPtr pathLossModel, 
		FadingPtr fadingModel);

	/**
	 * Compute the signal strength at the receiver for the given
	 * signal.
	 * @param signal the signal being transmitted.
	 * @param receiver the physical layer object for which
	 * we will compute the received signal strength.
	 * @return the received signal strength value.
	 */
	virtual double getRecvdStrength(const WirelessCommSignal& signal, 
		const PhysicalLayer& receiver) const;

	/**
	 * Computes whether or not the signal has an error each time it
	 * is called based on some channel error model.
	 * In general, a signal's error rate is based on its SINR,
	 * length, and data rate.  The SINR is given as a parameter
	 * and the other two factors are included in the signal's
	 * packet.
	 * @param signalSinr the current SINR of the signal.
	 * @param signal the signal being tested, which includes the
	 * length and data rate of the signal's packet.
	 * @return true if an error occurs in the signal.
	 */
	virtual bool signalHasError(double signalSinr, 
		const WirelessCommSignal& signal) const;

protected:

	/**
	 * A constructor.
	 * @param pathLossModel the path loss model that the
	 * channel will use.
	 */
	WirelessChannel(PathLossPtr pathLossModel);

	/**
	 * A constructor.
	 * @param pathLossModel the path loss model that the
	 * channel will use.
	 * @param fadingModel the fading model that the channel will use.
	 */
	WirelessChannel(PathLossPtr pathLossModel, FadingPtr fadingModel);

private:

	/// Determine when debugging info gets printed.
	static const bool m_DEBUG_SIGNAL_STRENGTH = false;

	PathLossPtr m_pathLossModel;
	FadingPtr m_fadingModel;

};
typedef boost::shared_ptr<WirelessChannel> WirelessChannelPtr;

/////////////////////////////////////////////////
// Inline Functions
/////////////////////////////////////////////////

inline WirelessChannelPtr WirelessChannel::create(
	PathLossPtr pathLossModel)
{
	WirelessChannelPtr p(new WirelessChannel(pathLossModel));
	return p;
}

inline WirelessChannelPtr WirelessChannel::create(
	PathLossPtr pathLossModel, FadingPtr fadingModel)
{
	WirelessChannelPtr p(new WirelessChannel(pathLossModel, fadingModel));
	return p;
}

/////////////////////////////////////////////////
// Overloaded Operators
/////////////////////////////////////////////////

#endif // WIRELESS_CHANNEL_H

