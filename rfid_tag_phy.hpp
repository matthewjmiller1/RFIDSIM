
#ifndef RFID_TAG_PHY_H
#define RFID_TAG_PHY_H

#include <boost/shared_ptr.hpp>

#include "physical_layer.hpp"
#include "simulator.hpp"

/**
 * The physical layer class for a RFID tag.
 */
class RfidTagPhy : public PhysicalLayer {
public:
	/// Smart pointer that clients should use.
	typedef boost::shared_ptr<RfidTagPhy> RfidTagPhyPtr;

	/**
	 * A factory method to ensure that all objects
	 * are created via \c new since we are using smart pointers. 
	 */
	static inline RfidTagPhyPtr create(
		NodePtr node, WirelessChannelManagerPtr wirelessChannelManager);
	
	/// A destructor
	/// A virtual destructor is recommended since the class
	/// has virtual functions.
	virtual ~RfidTagPhy();

	/**
	 * Gives \c this pointer as a \c RfidTagPhy object.
	 * This is used to deal with \c shared_ptr's, \c this, and
	 * inheritance.
	 * @return a pointer to \c this object.
	 */
	inline RfidTagPhyPtr thisRfidTagPhy();

	/**
	 * Gives \c this pointer as a \c PhysicalLayer object.
	 * This is used to deal with \c shared_ptr's, \c this, and
	 * inheritance.
	 * @return a pointer to \c this object.
	 */
	virtual inline PhysicalLayerPtr thisPhysicalLayer();

	/**
	 * Gives \c this pointer as a \c SimulationEndListener object.
	 * This is used to deal with \c shared_ptr's, \c this, and
	 * inheritance.
	 * @return a pointer to \c this object.
	 */
	virtual inline SimulationEndListenerPtr thisSimulationEndListener();

	/**
	 * The function called when the simulation ends.
	 */
	virtual void simulationEndHandler();

	/**
	 * Set the sending for the RFID tag.
	 * This corresponds to setting the channel on which
	 * the tag is sending since we model the tags's
	 * listening channel as being on which all tags listeng
	 * and all readers transmit on.
	 * @param channelId the ID of the channel on which the RFID
	 * tag will transmit.
	 * @return true if the object is attached to the channel
	 * successfully.
	 * @see setAllListenersChannel()
	 * @see resetSendingChannel()
	 */
	bool setSendingChannel(t_uint channelId);

	/**
	 * Invalidate the current sending channel.
	 * After calling this function, the tag is no longer
	 * sending to any channels.
	 * @see setSendingChannel()
	 * @see setAllListenersChannel()
	 */
	void resetSendingChannel();

	/**
	 * Set the channel on which this (and, presumably, all) tags
	 * are listening on.
	 * All tags should be set to listen on this channel,
	 * though they will send on a different channel where
	 * not necessarily all tags are sending.
	 * @param channelId the ID of the channel on which the tags
	 * will listen.
	 * @return true if the object is attached to the channel
	 * successfully.
	 * @see setSendingChannel()
	 * @see resetAllListenersChannel()
	 */
	bool setAllListenersChannel(t_uint channelId);

	/**
	 * Invalidate the current all listeners channel.
	 * After calling this function, the tag is no longer
	 * listening to any channels.
	 * @see setAllListenersChannel()
	 * @see setSendingChannel()
	 */
	void resetAllListenersChannel();

	/**
	 * This function is called when an error free signal is
	 * received, just before its packet is passed to the upper layer.
	 * The transmit power is set to the received power of the signal
	 * since the tag reflects the power back to the reader.
	 * @param signal the signal that was successfully received.
	 * @param recvdSignalStrength the strength of the signal at
	 * the receiver.
	 */
	virtual void recvdErrorFreeSignal(
		WirelessCommSignalPtr signal, double recvdSignalStrength);

protected:

	/// A constructor
	RfidTagPhy(NodePtr node, 
		WirelessChannelManagerPtr wirelessChannelManager);

private:

	/// An internal pointer to the object to all it
	/// to return \c this.
	boost::weak_ptr<RfidTagPhy> m_weakThis;

	/// Has the current channel has been set.
	/// @see setSendingChannel()
	/// @see resetSendingChannel()
	bool m_sendingChannelIsValid;

	/// The ID of the current channel (if valid).
	/// @see setChannel()
	/// @see resetChannel()
	t_uint m_sendingChannel;

	/// Has the all senders channel has been set.
	/// @see setAllListenersChannel()
	/// @see resetAllListenersChannel()
	bool m_allListenersChannelIsValid;

	/// The ID of the all senders channel (if valid).
	/// @see setAllListenersChannel()
	/// @see resetAllListenersChannel()
	t_uint m_allListenersChannel;

};
typedef boost::shared_ptr<RfidTagPhy> RfidTagPhyPtr;
typedef boost::shared_ptr<RfidTagPhy const> ConstRfidTagPhyPtr;

/////////////////////////////////////////////////
// Inline Functions
/////////////////////////////////////////////////

inline RfidTagPhyPtr RfidTagPhy::create(
	NodePtr node, WirelessChannelManagerPtr wirelessChannelManager)
{
	RfidTagPhyPtr p(new RfidTagPhy(node, wirelessChannelManager));
	p->m_weakThis = p;
	// weakThis *must* be set before this* functions are called.
	Simulator::instance()->addSimulationEndListener(
		p->thisSimulationEndListener());
	return p;
}

inline RfidTagPhyPtr RfidTagPhy::thisRfidTagPhy()
{
	RfidTagPhyPtr p(m_weakThis);
	return p;
}

inline PhysicalLayerPtr RfidTagPhy::thisPhysicalLayer()
{
	PhysicalLayerPtr p(m_weakThis);
	return p;
}

inline SimulationEndListenerPtr RfidTagPhy::thisSimulationEndListener()
{
	SimulationEndListenerPtr p(m_weakThis);
	return p;
}

/////////////////////////////////////////////////
// Overloaded Operators
/////////////////////////////////////////////////

#endif // RFID_TAG_PHY_H

