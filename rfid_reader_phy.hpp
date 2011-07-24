
#ifndef RFID_READER_PHY_H
#define RFID_READER_PHY_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include "physical_layer.hpp"
#include "simulator.hpp"

/**
 * The physical layer class for a RFID reader.
 */
class RfidReaderPhy : public PhysicalLayer {
public:
	/// Smart pointer that clients should use.
	typedef boost::shared_ptr<RfidReaderPhy> RfidReaderPhyPtr;

	/**
	 * A factory method to ensure that all objects
	 * are created via \c new since we are using smart pointers. 
	 */
	static inline RfidReaderPhyPtr create(
		NodePtr node, WirelessChannelManagerPtr wirelessChannelManager);
	
	/// A destructor
	/// A virtual destructor is recommended since the class
	/// has virtual functions.
	virtual ~RfidReaderPhy();

	/**
	 * Gives \c this pointer as a \c RfidReaderPhy object.
	 * This is used to deal with \c shared_ptr's, \c this, and
	 * inheritance.
	 * @return a pointer to \c this object.
	 */
	inline RfidReaderPhyPtr thisRfidReaderPhy();

	/**
	 * Gives \c this pointer as a \c PhysicalLayer object.
	 * This is used to deal with \c shared_ptr's, \c this, and
	 * inheritance.
	 * @return a pointer to \c this object.
	 */
	inline PhysicalLayerPtr thisPhysicalLayer();

	/**
	 * Gives \c this pointer as a \c SimulationEndListener object.
	 * This is used to deal with \c shared_ptr's, \c this, and
	 * inheritance.
	 * @return a pointer to \c this object.
	 */
	inline SimulationEndListenerPtr thisSimulationEndListener();

	/**
	 * The function called when the simulation ends.
	 */
	virtual void simulationEndHandler();

	/**
	 * Set the listening channel for the RFID reader.
	 * This corresponds to setting the channel on which
	 * the reader is listening since we model the reader's
	 * transmission channel as being on which all readers send
	 * on and all tags listen on.
	 * @param channelId the ID of the channel on which the RFID
	 * reader will listen.
	 * @return true if the object is attached to the channel
	 * successfully.
	 * @see setAllSendersChannel()
	 * @see resetRegularChannel()
	 */
	bool setRegularChannel(t_uint channelId);

	/**
	 * Invalidate the current listening channel.
	 * After calling this function, the reader is no longer
	 * listening to any channels.
	 * @see setRegularChannel()
	 * @see setAllSendersChannel()
	 */
	void resetRegularChannel();

	/**
	 * Set the channel on which this (and, presumably, all) readers
	 * are transmitting on.
	 * All readers should be set to transmit on this channel,
	 * though they will listen on a different channel where
	 * not necessarily all readers are listening.
	 * @param channelId the ID of the channel on which the readers
	 * will transmit.
	 * @return true if the object is attached to the channel
	 * successfully.
	 * @see setRegularChannel()
	 * @see resetAllSendersChannel()
	 */
	bool setAllSendersChannel(t_uint channelId);

	/**
	 * Invalidate the current all senders channel.
	 * After calling this function, the reader is no longer
	 * sending to any channels.
	 * @see setAllSendersChannel()
	 * @see setRegularChannel()
	 */
	void resetAllSendersChannel();

protected:

	/// A constructor
	RfidReaderPhy(NodePtr node,
		WirelessChannelManagerPtr wirelessChannelManager);

private:

	/// An internal pointer to the object to allow it
	/// to return \c this.
	boost::weak_ptr<RfidReaderPhy> m_weakThis;

	/// The default reader receive threshold.
	static const double m_DEFAULT_READER_RX_THRESHOLD;

	/// The default reader carrier sensing threshold.
	static const double m_DEFAULT_READER_CS_THRESHOLD;

	/// Has the current channel has been set.
	/// @see setRegularChannel()
	/// @see resetRegularChannel()
	bool m_regularChannelIsValid;

	/// The ID of the current channel (if valid).
	/// @see setChannel()
	/// @see resetChannel()
	t_uint m_regularChannel;

	/// Has the all senders channel has been set.
	/// @see setAllSendersChannel()
	/// @see resetAllSendersChannel()
	bool m_allSendersChannelIsValid;

	/// The ID of the all senders channel (if valid).
	/// @see setAllSendersChannel()
	/// @see resetAllSendersChannel()
	t_uint m_allSendersChannel;

};
typedef boost::shared_ptr<RfidReaderPhy> RfidReaderPhyPtr;

/////////////////////////////////////////////////
// Inline Functions
/////////////////////////////////////////////////

inline RfidReaderPhyPtr RfidReaderPhy::create(
	NodePtr node, WirelessChannelManagerPtr wirelessChannelManager)
{
	RfidReaderPhyPtr p(new RfidReaderPhy(node, wirelessChannelManager));
	p->m_weakThis = p;
	// weakThis *must* be set before this* functions are called.
	Simulator::instance()->addSimulationEndListener(
		p->thisSimulationEndListener());
	return p;
}

inline RfidReaderPhyPtr RfidReaderPhy::thisRfidReaderPhy()
{
	RfidReaderPhyPtr p(m_weakThis);
	return p;
}

inline PhysicalLayerPtr RfidReaderPhy::thisPhysicalLayer()
{
	PhysicalLayerPtr p(m_weakThis);
	return p;
}

inline SimulationEndListenerPtr RfidReaderPhy::thisSimulationEndListener()
{
	SimulationEndListenerPtr p(m_weakThis);
	return p;
}

/////////////////////////////////////////////////
// Overloaded Operators
/////////////////////////////////////////////////

#endif // RFID_READER_PHY_H

