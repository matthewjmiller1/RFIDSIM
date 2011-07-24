
#ifndef MAC_PROTOCOL_H
#define MAC_PROTOCOL_H

#include <boost/shared_ptr.hpp>

#include "timer.hpp"
#include "simulation_end_listener.hpp"

class LinkLayer;
typedef boost::shared_ptr<LinkLayer> LinkLayerPtr;

/**
 * The class for handling medium access control (MAC)
 * on a channel.
 */
class MacProtocol : public SimulationEndListener {
friend class SendToLinkLayerEvent;
public:
	/// Smart pointer that clients should use.
	typedef boost::shared_ptr<MacProtocol> MacProtocolPtr;
	/// Smart pointer that clients should use.
	typedef boost::shared_ptr<SimulationEndListener>
		SimulationEndListenerPtr;

	/// A destructor.
	virtual ~MacProtocol();

	/**
	 * Gives \c this pointer as a \c MacProtocol object.
	 * This is used to deal with \c shared_ptr's, \c this, and
	 * inheritance.
	 * @return a pointer to \c this object.
	 */
	virtual MacProtocolPtr thisMacProtocol() = 0;

	/**
	 * Gives \c this pointer as a \c SimulationEndListenerPtr object.
	 * This is used to deal with \c shared_ptr's, \c this, and
	 * inheritance.
	 * @return a pointer to \c this object.
	 */
	virtual SimulationEndListenerPtr thisSimulationEndListener() = 0;

	/**
	 * Starts a timer to send the packet with the specified delay.
	 * @param direction the direction the packet will be sent.
	 * @param packet a pointer to the packet being sent.
	 * @param delay the delay with which the packet should be sent.
	 * @return true if the timer is set successfully.
	 */
	bool startSendTimer(
		CommunicationLayer::Directions direction, PacketPtr packet,
		const SimTime& delay);

	/**
	 * Receives a packet from a sending layer.
	 * @param direction the direction the packet was sent.
	 * @param packet a pointer to the received packet.
	 * @param sendingLayerIdx the index of the layer that sent the
	 * packet.
	 * @return true if the specified layer was able to receive
	 * the packet.
	 */
	virtual bool recvFromLinkLayer(
		CommunicationLayer::Directions direction, PacketPtr packet,
		t_uint sendingLayerIdx);

	/**
	 * Handle a MAC packet that is received.
	 * @param packet a pointer to the received packet.
	 * @param sendingLayerIdx the index of the layer that sent the
	 * packet.
	 * @return true if the packet was able to be handled.
	 */
	virtual bool handleRecvdMacPacket(PacketPtr packet, 
		t_uint sendingLayerIdx) = 0;

	/**
	 * Handle a packet received from upper layers.
	 * @param packet a pointer to the received packet.
	 * @param sendingLayerIdx the index of the layer that sent the
	 * packet.
	 * @return true if the packet was able to be handled.
	 */
	virtual bool handleRecvdUpperLayerPacket(PacketPtr packet, 
		t_uint sendingLayerIdx) = 0;

	/**
	 * This is called when the MAC is scheduled to transmit
	 * a packet on the channel but the channel is busy.
	 * @param packet the packet whose transmission was being
	 * attmpted.
	 * @see handlePacketSent()
	 */
	virtual void handleChannelBusy(PacketPtr packet) = 0;

	/**
	 * This is called when the MAC is scheduled to transmit
	 * a packet on the channel and it is sent on the channel.
	 * @param packet the packet whose transmission was being
	 * attmpted.
	 * @see handleChannelBusy()
	 */
	virtual void handlePacketSent(PacketPtr packet) = 0;

	/**
	 * Set the LinkLayer associated with this MAC.
	 * A weak pointer to the layer is maintained to break
	 * a cyclic reference.
	 * @param linkLayer a point to the LinkLayer object.
	 */
	void setLinkLayer(LinkLayerPtr linkLayer);

	/**
	 * Get a pointer to the node associated with this MAC layer.
	 * @return a pointer to the node.
	 */
	inline NodePtr getNode() const
	{
		return m_node;
	}

protected:

	/// A constructor.
	/// @param node the node that owns this MAC.
	MacProtocol(NodePtr node);

	/**
	 *	Send a packet to the default receiving layer.
	 * @param direction the direction the packet was sent.
	 * @param packet a pointer to the received packet.
	 * @return true if the specified layer was able to receive
	 * the packet.
	 */
	bool sendToLinkLayer(
		CommunicationLayer::Directions direction, PacketPtr packet);

	/**
	 * Block all incoming queues from upper layers.
	 */
	void blockUpperQueues();

	/**
	 * Unblock all incoming queues from upper layers.
	 */
	void unblockUpperQueues();

	/**
	 * Determine whether incoming queues are currently blocked.
	 * @return true if the are blocked.
	 */
	bool getQueueIsBlocked() const;

	/// A timer for when a packet is sent to the link layer.
	TimerPtr m_sendTimer;

	/// A pointer to the link layer associated with this MAC
	/// object.
	/// This must be a weak pointer because there is a cyclic
	/// reference since MAC points to Link and Link points
	/// to MAC.
	boost::weak_ptr<LinkLayer> m_linkLayer;

private:

	/// A pointer to the node that owns this MAC.
	NodePtr m_node;

};
typedef boost::shared_ptr<MacProtocol> MacProtocolPtr;

/////////////////////////////////////////////////
// SlottedMac Class
/////////////////////////////////////////////////

/**
 * A protocol which divides time into discrete slots during
 * which packets can be transmitted.
 */
class SlottedMac : public MacProtocol {
friend class SlottedMacSlotEvent;
public:
	/// Smart pointer that clients should use.
	typedef boost::shared_ptr<SlottedMac> SlottedMacPtr;

	/// A destructor.
	virtual ~SlottedMac() {};

protected:

	/// The default time per slot.
	static const double m_DEFAULT_SLOT_TIME;

	/// A timer for when each slot begins.
	TimerPtr m_slotTimer;

	/// The current slot number in the cycle.
	t_uint m_currentSlotNumber;

	/// This object's chosen transmission slot.
	t_uint m_txSlotNumber;
	
	/// The number of slots in the current contention cycle.
	/// If m_currentSlotNumber < m_numberOfSlots, then the
	/// node is not currently engaged in a contention cycle.
	t_uint m_numberOfSlots;

	/// A pointer to the packet to transmit in the chosen slot.
	PacketPtr m_packetToTransmit;

	/// A constructor.
	SlottedMac(NodePtr node) 
		: MacProtocol(node), m_currentSlotNumber(0),
		m_txSlotNumber(0), m_numberOfSlots(0)
	{ }

	/**
	 * This function is called whenever a slot begins.
	 */
	virtual void beginSlotEvent() = 0;

	/**
	 * Set the time per slot.
	 * @param slotTime the new time per slot.
	 */
	inline void setSlotTime(const SimTime& slotTime)
	{
		m_slotTime = slotTime;
	}

	/**
	 * Get the time per slot.
	 * @return the current time per slot.
	 */
	inline SimTime getSlotTime() const
	{
		return m_slotTime;
	}

	/**
	 * Determine whether the node is currently engaged in a
	 * contention cycle.
	 * @return true if the node is in a contention cycle.
	 */
	inline bool inContentionCycle() const
	{
		return (m_currentSlotNumber < m_numberOfSlots);
	}

	/**
	 * Stops a contention cycle by reseting the appropriate
	 * state variables to zero.
	 */
	inline void stopContentionCycle()
	{
		m_currentSlotNumber = 0;
		m_numberOfSlots = 0;
	}

private:

	/// The time per slot.
	/// @see setSlotTime()
	/// @see getSlotTime()
	SimTime m_slotTime;

};
typedef boost::shared_ptr<SlottedMac> SlottedMacPtr;

////////////////////////////////////////////////
// Event Subclasses
/////////////////////////////////////////////////

/**
 * The event for when a discrete slot begins.
 */
class SlottedMacSlotEvent : public Event {
public:
	/// Smart pointer that clients should use.
	typedef boost::shared_ptr<SlottedMacSlotEvent> SlottedMacSlotEventPtr;

	/**
	 * A factory method to ensure that all objects
	 * are created via \c new since we are using smart pointers. 
	 */
	static inline SlottedMacSlotEventPtr create(
		SlottedMacPtr slottedMac)
	{
		SlottedMacSlotEventPtr p(new SlottedMacSlotEvent(slottedMac));
		return p;
	}

	void execute()
	{
		m_slottedMac->beginSlotEvent();
	}

protected:

	/// A constructor.
	SlottedMacSlotEvent(SlottedMacPtr slottedMac) 
		: Event()
	{ 
		m_slottedMac = slottedMac;
	}
private:
	SlottedMacPtr m_slottedMac;
};
typedef boost::shared_ptr<SlottedMacSlotEvent> SlottedMacSlotEventPtr;

/**
 * The event for when a packet gets passed from a MAC
 * object to a link layer object.
 */
class SendToLinkLayerEvent : public Event {
public:
	/// Smart pointer that clients should use.
	typedef boost::shared_ptr<SendToLinkLayerEvent> 
		SendToLinkLayerEventPtr;

	/**
	 * A factory method to ensure that all objects
	 * are created via \c new since we are using smart pointers. 
	 */
	static inline SendToLinkLayerEventPtr create(
		MacProtocolPtr macProtocol, 
		CommunicationLayer::Directions direction, PacketPtr packet)
	{
		SendToLinkLayerEventPtr p(new SendToLinkLayerEvent(
			macProtocol, direction, packet));
		return p;
	}

	void execute()
	{
		m_macProtocol->sendToLinkLayer(m_direction, m_packet);
	}

protected:
	/// A constructor.
	SendToLinkLayerEvent(MacProtocolPtr macProtocol, 
		CommunicationLayer::Directions direction, PacketPtr packet) 
		: Event(), m_macProtocol(macProtocol), m_direction(direction),
		m_packet(packet)
	{ 

	}
private:
	MacProtocolPtr m_macProtocol;
	CommunicationLayer::Directions m_direction;
	PacketPtr m_packet;
};
typedef boost::shared_ptr<SendToLinkLayerEvent> SendToLinkLayerEventPtr;

#endif // MAC_PROTOCOL_H
