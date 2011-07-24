
#ifndef RFID_READER_MAC_H
#define RFID_READER_MAC_H

#include <vector>
using namespace std;
#include <boost/shared_ptr.hpp>

#include "simulator.hpp"
#include "mac_protocol.hpp"
#include "packet.hpp"

class RfidTagMacData;
typedef boost::shared_ptr<RfidTagMacData> RfidTagMacDataPtr;
class RfidReaderApp;
typedef boost::shared_ptr<RfidReaderApp> RfidReaderAppPtr;

////////////////////////////////////////////////
// PacketData Subclass
/////////////////////////////////////////////////

/**
 * The representation of the MAC data in a RfidReaderMac's
 * packet.
 */
class RfidReaderMacData : public PacketData {
friend ostream& operator<< (ostream& s, const RfidReaderMacData& data);
public:
	/// Smart pointer that clients should use.
	typedef boost::shared_ptr<RfidReaderMacData> RfidReaderMacDataPtr;

	/**
	 * Types enum.
	 * Specifies the types of packets that are sent.
	 */
	enum Types {
		Types_Generic, /**< enum value Types_Generic. */
		Types_Request, /**< enum value Types_Request. */
		Types_Ack, /**< enum value Types_Ack. */
		Types_Select /**< enum value Types_Select. */
	};

	/**
	 * A factory method to ensure that all objects
	 * are created via \c new since we are using smart pointers. 
	 */
	static inline RfidReaderMacDataPtr create();

	/**
	 * A factory method to ensure that all objects
	 * are created via \c new since we are using smart pointers. 
	 */
	static inline RfidReaderMacDataPtr create(
		const RfidReaderMacData& rhs);

	/**
	 * Get the size of this data.
	 * @return the size of the data in bytes.
	 */
	virtual inline t_uint getSizeInBytes() const;

	/**
	 * Write the sender node ID to the packet data.
	 * @param nodeId the ID of the sender.
	 */
	void setSenderId(const NodeId& nodeId);

	/**
	 * Read the sender node ID from the packet data.
	 * @return the ID of the sender.
	 */
	NodeId getSenderId() const;

	/**
	 * Write the receiver node ID to the packet data.
	 * @param nodeId the ID of the receiver.
	 */
	void setReceiverId(const NodeId& nodeId);

	/**
	 * Read the receiver node ID from the packet data.
	 * @return the ID of the receiver.
	 */
	NodeId getReceiverId() const;

	/**
	 * Write the type field to this packet data.
	 * @param type the value for the type field.
	 */
	inline void setType(Types type);

	/**
	 * Read the type field from this packet data.
	 * @return the type field of the packet.
	 */
	inline Types getType() const;

	/**
	 * Write the number of slots field to this packet data.
	 * @param numberOfSlots the value of the number of slots field.
	 */
	inline void setNumberOfSlots(t_uchar numberOfSlots);

	/**
	 * Read the number of slots field from this packet data.
	 * @return the number of slots field of the packet.
	 */
	inline t_uchar getNumberOfSlots() const;

	/**
	 * Return the stream representation of the object.
	 */
	inline ostream& print(ostream& s) const;

protected:

	/// A constructor.
	RfidReaderMacData();

	/// A copy constructor.
	RfidReaderMacData(const RfidReaderMacData& rhs);

	/**
	 * Returns a pointer to a deep copy of this object.
	 * This is addresses the slicing problem with
	 * copy construction.
	 * @return a pointer to a deep copy of this object.
	 */
	virtual PacketDataPtr clone() const;

private:

	/// The number of bytes for a sender ID.
	static const t_uint m_senderIdBytes = 4;

	/// The number of bytes for a receiver ID.
	static const t_uint m_receiverIdBytes = 12;

	/// The number of bytes for a packet type.
	static const t_uint m_typeBytes = 1;

	/// The number of bytes for the number of slots.
	static const t_uint m_numberOfSlotsBytes = 1;

	/// The data field for the sender ID.
	t_uchar m_senderId[m_senderIdBytes];

	/// The data field for the receiver ID.
	t_uchar m_receiverId[m_receiverIdBytes];

	/// The data field for the number of slots.
	t_uchar m_numberOfSlots;

	/// The type field for the packet.
	Types m_type;

};
typedef boost::shared_ptr<RfidReaderMacData> RfidReaderMacDataPtr;

/////////////////////////////////////////////////
// RfidReaderMac Class
/////////////////////////////////////////////////

/**
 * A MAC protocol for RFID readers that uses slotted ALOHA.
 */
class RfidReaderMac : public SlottedMac {
friend class RfidMacCycleEvent;
public:
	/// Smart pointer that clients should use.
	typedef boost::shared_ptr<RfidReaderMac> RfidReaderMacPtr;

	/// A destructor.
	virtual ~RfidReaderMac();

	/**
	 * A factory method to ensure that all objects
	 * are created via \c new since we are using smart pointers. 
	 * @param node the node that owns this object.
	 * @param readerApp the application using this MAC.
	 */
	static inline RfidReaderMacPtr create(NodePtr node, 
		RfidReaderAppPtr readerApp);

	/**
	 * Gives \c this pointer as a \c RfidReaderMac object.
	 * This is used to deal with \c shared_ptr's, \c this, and
	 * inheritance.
	 * @return a pointer to \c this object.
	 */
	virtual inline RfidReaderMacPtr thisRfidReaderMac();

	/**
	 * Gives \c this pointer as a \c SlottedMac object.
	 * This is used to deal with \c shared_ptr's, \c this, and
	 * inheritance.
	 * @return a pointer to \c this object.
	 */
	virtual inline SlottedMacPtr thisSlottedMac();

	/**
	 * Gives \c this pointer as a \c MacProtocol object.
	 * This is used to deal with \c shared_ptr's, \c this, and
	 * inheritance.
	 * @return a pointer to \c this object.
	 */
	virtual inline MacProtocolPtr thisMacProtocol();

	/**
	 * Gives \c this pointer as a \c SimulationEndListenerPtr object.
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
	 * Handle a MAC packet that is received.
	 * @param packet a pointer to the received packet.
	 * @param sendingLayerIdx the index of the layer that sent the
	 * packet.
	 * @return true if the packet was able to be handled.
	 */
	bool handleRecvdMacPacket(PacketPtr packet, t_uint sendingLayerIdx);

	/**
	 * Handle a packet received from an upper layer.
	 * @param packet a pointer to the received packet.
	 * @param sendingLayerIdx the index of the layer that sent the
	 * packet.
	 * @return true if the packet was able to be handled.
	 */
	bool handleRecvdUpperLayerPacket(PacketPtr packet, 
		t_uint sendingLayerIdx);

	/**
	 * This is called when the MAC is scheduled to transmit
	 * a packet on the channel but the channel is busy.
	 * @param packet the packet whose transmission was being
	 * attmpted.
	 * @see handlePacketSent()
	 */
	virtual void handleChannelBusy(PacketPtr packet);

	/**
	 * This is called when the MAC is scheduled to transmit
	 * a packet on the channel and it is sent on the channel.
	 * @param packet the packet whose transmission was being
	 * attmpted.
	 * @see handleChannelBusy()
	 */
	virtual void handlePacketSent(PacketPtr packet);

protected:

	/// The interframe spacing of reader packets.
	/// The time which the node defers before transmitting
	/// a packet in a slot.
	static const double m_READER_IFS;

	/// The default number of slots the reader will
	/// use in contention intervals.
	static const t_uint m_DEFAULT_NUMBER_OF_SLOTS;

	/// The default time for how long a request cycle
	/// lasts.
	static const double m_DEFAULT_CYCLE_TIME;

	/// The threshold for how many consecutive tag reads
	/// we can miss before we manually end a cycle.
	static const t_uint m_MISSED_READ_THRESHOLD;

	//@{
	/// String for the stats file output.
	static const string m_MISSED_READ_TOTAL_STRING;
	static const string m_MISSED_READ_SLOT_AVG_STRING;
	static const string m_WINNING_SLOT_AVG_STRING;
	//@}

	/// Determines whether m_resetSlotNumber should be considered
	/// valid.
	bool m_doResetSlot;

	/// If this slot number is reached, then the request cycle
	/// should be reset if m_doResetSlot is true.
	t_uint m_resetSlotNumber;

	/// Determines whether we should do the entire
	/// read cycle or stop early after several
	/// consecutive contention cycles are unsuccessful.
	bool m_doEntireReadCycle;

	/// Counts the number of consecutive times that we
	/// are unable to successfully read a packet in a
	/// contention cycle.
	t_uint m_missedReadCount;

	/// A timer to keep track of when request cycles end.
	TimerPtr m_cycleTimer;

	/// The application packet for which we are currently
	/// doing the MAC.
	PacketPtr m_currentAppReadPacket;

	/// The number of slots we are currently using in contention
	/// intervals.
	t_uint m_nextCycleNumberOfSlots;

	/// How long the next cycle lasts.
	SimTime m_nextCycleTime;

	/// A pointer to the application layer associated with this MAC.
	RfidReaderAppPtr m_readerApp;

	/// Keep track of the winning slot number for each
	/// received payload for statistics.
	vector<pair<NodeId,t_uint> > m_winningSlotNumbers;

	/// Keep track of the slot number for when a missed
	/// read occurs for statistics.
	vector<t_uint> m_missedReads;

	/// A constructor.
	RfidReaderMac(NodePtr node, RfidReaderAppPtr readerApp);

	/**
	 * The function called when a slot begins.
	 */
	virtual void beginSlotEvent();

	/**
	 * The function called when a request cycle ends.
	 */
	virtual void endRequestCycleEvent();

	/**
	 * Create a request packet for the node.
	 * @return a pointer to the read packet created by the function.
	 */
	PacketPtr createRequestPacket() const;

	/**
	 * Create a acknowledgement packet for the node.
	 * @param destination the destination of the node to which
	 * we are sending the ACK.
	 * @return a pointer to the read packet created by the function.
	 */
	PacketPtr createAckPacket(NodeId destination) const;

	/**
	 * Add a generic header to the packet.
	 * @param packet a pointer to the packet to which the header
	 * will be added.
	 * @param receiverId the ID of the packet's destination.
	 */
	void addGenericHeader(PacketPtr packet, NodeId receiverId) const;

	/**
	 * Add a \c SELECT header to the packet.
	 * @param packet a pointer to the packet to which the header
	 * will be added.
	 * @param receiverId the ID of the packet's destination.
	 */
	void addSelectHeader(PacketPtr packet, NodeId receiverId) const;

	/**
	 * Determine if the packet is for me based on the receiver
	 * address (i.e., has my address or the broadcast address).
	 * @param macData the MAC header of the packet.
	 */
	bool packetIsForMe(RfidTagMacDataPtr macData) const;

	/**
	 * Determine if there is enough time left in the current
	 * request cycle to start a new contention cycle.
	 * @return true if there is enough time remaning.
	 */
	bool isEnoughTimeForContentionCycle() const;

	/**
	 * Send out a request packet to start a new contention
	 * cycle.
	 */
	void startNextContentionCycle();

	/**
	 * Determine whether the packet is a packet of the
	 * given type by looking at it's header.
	 * @param packet a pointer to the packet under consideration.
	 * @param type the type for which we are checking.
	 * @return true if it is a request packet.
	 */
	bool isPacketType(PacketPtr packet, 
		RfidReaderMacData::Types type) const;

private:

	/// Determines whether debugging info is printed.
	static const bool m_DEBUG = true;

	/// Determines whether we print debugging info.
	static const bool m_DEBUG_CONTENTION_CYCLE_TIME = false;

	/// An internal pointer to the object to allow it
	/// to return \c this.
	boost::weak_ptr<RfidReaderMac> m_weakThis;

};
typedef boost::shared_ptr<RfidReaderMac> RfidReaderMacPtr;

////////////////////////////////////////////////
// Event Subclasses
/////////////////////////////////////////////////

/**
 * The event for when the timer fires signaling
 * a timeout for an application layer's read request.
 */
class RfidMacCycleEvent : public Event {
public:
	/// Smart pointer that clients should use.
	typedef boost::shared_ptr<RfidMacCycleEvent> 
		RfidMacCycleEventPtr;

	/**
	 * A factory method to ensure that all objects
	 * are created via \c new since we are using smart pointers. 
	 */
	static inline RfidMacCycleEventPtr create(
		RfidReaderMacPtr rfidReaderMac)
	{
		RfidMacCycleEventPtr p(
			new RfidMacCycleEvent(rfidReaderMac));
		return p;
	}

	void execute()
	{
		m_rfidReaderMac->endRequestCycleEvent();
	}

protected:
	/// A constructor.
	RfidMacCycleEvent(RfidReaderMacPtr rfidReaderMac) 
		: Event()
	{ 
		m_rfidReaderMac = rfidReaderMac;
	}
private:
	RfidReaderMacPtr m_rfidReaderMac;
};
typedef boost::shared_ptr<RfidMacCycleEvent> 
	RfidMacCycleEventPtr;

////////////////////////////////////////////////
// Back to RfidReaderMac
/////////////////////////////////////////////////

/////////////////////////////////////////////////
// Inline Functions
/////////////////////////////////////////////////

inline RfidReaderMacPtr RfidReaderMac::create(NodePtr node, 
	RfidReaderAppPtr readerApp)
{
	RfidReaderMacPtr p(new RfidReaderMac(node, readerApp));
	// weakThis *must* be set before the this* functions are called.
	p->m_weakThis = p;

	SlottedMacSlotEventPtr slotEvent = 
		SlottedMacSlotEvent::create(p->thisSlottedMac());
	p->m_slotTimer = Timer::create(p->getNode(), slotEvent);
	p->m_slotTimer->start(SimTime(0.0));

	RfidMacCycleEventPtr cycleEvent =
		RfidMacCycleEvent::create(p->thisRfidReaderMac());
	p->m_cycleTimer = Timer::create(p->getNode(), cycleEvent);

	Simulator::instance()->addSimulationEndListener(
		p->thisSimulationEndListener());

	return p;
}

inline RfidReaderMacPtr RfidReaderMac::thisRfidReaderMac()
{
	RfidReaderMacPtr p(m_weakThis);
	return p;
}

inline SlottedMacPtr RfidReaderMac::thisSlottedMac()
{
	SlottedMacPtr p(m_weakThis);
	return p;
}

inline MacProtocolPtr RfidReaderMac::thisMacProtocol()
{
	MacProtocolPtr p(m_weakThis);
	return p;
}

inline SimulationEndListenerPtr RfidReaderMac::thisSimulationEndListener()
{
	SimulationEndListenerPtr p(m_weakThis);
	return p;
}

/////////////////////////////////////////////////
// Overloaded Operators
/////////////////////////////////////////////////

/////////////////////////////////////////////////
// RfidReaderMacData Inline Functions
/////////////////////////////////////////////////

inline RfidReaderMacDataPtr RfidReaderMacData::create()
{
	RfidReaderMacDataPtr p(new RfidReaderMacData());
	return p;
}

inline RfidReaderMacDataPtr RfidReaderMacData::create(
	const RfidReaderMacData& rhs)
{
	RfidReaderMacDataPtr p = 
		boost::dynamic_pointer_cast<RfidReaderMacData>(rhs.clone());
	// If the shared_ptr is empty, then the cast failed.
	assert(p.get() != 0);
	return p;
}

inline t_uint RfidReaderMacData::getSizeInBytes() const
{
	t_uint byteCount = (m_senderIdBytes + m_receiverIdBytes + m_typeBytes);

	switch(getType()) {
	case Types_Request:
		byteCount += m_numberOfSlotsBytes;
		break;
	default:
		break;
	}

	return byteCount;
}

inline void RfidReaderMacData::setType(RfidReaderMacData::Types type)
{
	m_type = type;
}

inline RfidReaderMacData::Types RfidReaderMacData::getType() const
{
	return m_type;
}

inline void RfidReaderMacData::setNumberOfSlots(t_uchar numberOfSlots)
{
	m_numberOfSlots = numberOfSlots;
}

inline t_uchar RfidReaderMacData::getNumberOfSlots() const
{
	return m_numberOfSlots;
}

/////////////////////////////////////////////////
// Overloaded Operators
/////////////////////////////////////////////////

inline ostream& operator<< (ostream& s, 
		const RfidReaderMacData::Types& dataType)
{
	switch(dataType) {
	case RfidReaderMacData::Types_Generic:
		s << "GENERIC";
		break;
	case RfidReaderMacData::Types_Request:
		s << "REQUEST";
		break;
	case RfidReaderMacData::Types_Ack:
		s << "ACK";
		break;
	case RfidReaderMacData::Types_Select:
		s << "SELECT";
		break;
	}
	return s;
}

inline ostream& RfidReaderMacData::print(ostream& s) const
{
	s << "type=" << m_type << ", " <<
		"senderId=" << 
		NodeId(m_senderId, m_senderIdBytes) << ", " <<
		"receiverId=" << 
		NodeId(m_receiverId, m_receiverIdBytes) << ", " <<
		"numberOfSlots=" << static_cast<t_uint>(m_numberOfSlots);
	return s;
}

#endif // RFID_READER_MAC_H

