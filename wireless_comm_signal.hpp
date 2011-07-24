
#ifndef WIRELESS_COMM_SIGNAL_H
#define WIRELESS_COMM_SIGNAL_H

#include <boost/shared_ptr.hpp>

#include "signal.hpp"
#include "packet.hpp"

class SimTime;

/**
 * A radio signal to transmitted over a WirelessChannel.
 */
class WirelessCommSignal : public Signal {
	friend ostream& operator<< (ostream& s, const WirelessCommSignal& rhs);
public:
	/// Smart pointer that clients should use.
	typedef boost::shared_ptr<WirelessCommSignal> WirelessCommSignalPtr;

	/**
	 * A factory method to ensure that all objects
	 * are created via \c new since we are using smart pointers. 
	 */
	static inline WirelessCommSignalPtr create(
		const Location& location, double dbStrength, 
		double wavelength, double transmitterGain,
		PacketPtr packet);
	
	/**
	 * A factory method to ensure that all objects
	 * are created via \c new since we are using smart pointers. 
	 */
	static inline WirelessCommSignalPtr create(
		const WirelessCommSignal& rhs);

	/**
	 * Get the packet encapsulated in this signal.
	 * @return pointer to the packet.
	 */
	inline PacketPtr getPacketPtr() const;

	/**
	 * Get the wavelength of this signal.
	 * @return the wavelength of the signal in meters.
	 */
	inline double getWavelength() const;

	/**
	 * Get the gain of the transmitter of the signal.
	 * @return the gain of the signal's transmitter (unit-less).
	 */
	inline double getTransmitterGain() const;

	/**
	 * Get the time duration of the signal.
	 * @return the time duration of the signal.
	 */
	SimTime getDuration() const;

	/**
	 * Set the channel ID of the signal.
	 * @param channelId the channel ID.
	 */
	inline void setChannelId(t_uint channelId);

	/**
	 * Get the channel ID of the signal.
	 * @return the channel ID.
	 */
	inline t_uint getChannelId() const;

protected:

	/// A constructor
	WirelessCommSignal(const Location& location, double dbStrength, 
		double wavelength, double transmitterGain, 
		PacketPtr packet);

	/// A copy constructor
	WirelessCommSignal(const WirelessCommSignal& rhs);

	virtual SignalPtr clone() const;

private:

	/// The default channel ID of the signal.
	static const t_uint m_DEFAULT_CHANNEL_ID = 0;

	/// A pointer to this object's packet.
	/// @see getPacketPtr()
	PacketPtr m_packetPtr;

	/// The signal's wavelength.
	/// @see getWavelength()
	double m_wavelength;

	/// The antenna gain of the signal's transmitter.
	/// @see getTransmitterGain()
	double m_transmitterGain;

	/// The channel on which the signal was received.
	/// @see getChannelId()
	t_uint m_channelId;

	/// Declare private to restrict use.
	WirelessCommSignal& operator= (const WirelessCommSignal& rhs);

};
typedef boost::shared_ptr<WirelessCommSignal> WirelessCommSignalPtr;
typedef boost::shared_ptr<WirelessCommSignal const> 
	ConstWirelessCommSignalPtr;

/////////////////////////////////////////////////
// Inline Functions
/////////////////////////////////////////////////

inline WirelessCommSignalPtr WirelessCommSignal::create(
	const Location& location, double dbStrength,
	double wavelength, double transmitterGain,
	PacketPtr packet)
{
	WirelessCommSignalPtr p(new WirelessCommSignal(location, 
		dbStrength, wavelength, transmitterGain, packet));
	return p;
}

inline WirelessCommSignalPtr WirelessCommSignal::create(
	const WirelessCommSignal& rhs)
{
	WirelessCommSignalPtr p = 
		boost::dynamic_pointer_cast<WirelessCommSignal>(rhs.clone());
	// If the shared_ptr is empty, then the cast failed.
	assert(p.get() != 0);
	return p;
}

inline PacketPtr WirelessCommSignal::getPacketPtr() const
{
	return m_packetPtr;
}

inline double WirelessCommSignal::getWavelength() const
{
	return m_wavelength;
}

inline double WirelessCommSignal::getTransmitterGain() const
{
	return m_transmitterGain;
}

inline void WirelessCommSignal::setChannelId(t_uint channelId)
{
	m_channelId = channelId;
}

inline t_uint WirelessCommSignal::getChannelId() const
{
	return m_channelId;
}

/////////////////////////////////////////////////
// Overloaded Operators
/////////////////////////////////////////////////

inline ostream& operator<< (ostream& s, const WirelessCommSignal& rhs)
{
	return s << "Wireless signal state (" << 
		static_cast<Signal>(rhs) << "), packet=(" << 
		*(rhs.m_packetPtr) << ")";
}

// The assignment operator is now disabled via a private modifier.
/*
inline WirelessCommSignal& WirelessCommSignal::operator= 
	(const WirelessCommSignal& rhs)
{
	if(this == &rhs)
		return *this;

	Signal::operator=(rhs);
	m_packetPtr = Packet::create(*rhs.m_packetPtr);

	return *this;
}
*/

#endif // WIRELESS_COMM_SIGNAL_H

