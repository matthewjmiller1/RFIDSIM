
#ifndef RFID_TAG_MAC_H
#define RFID_TAG_MAC_H

#include <boost/shared_ptr.hpp>

#include "simulator.hpp"
#include "mac_protocol.hpp"
#include "packet.hpp"

class RfidReaderMacData;
typedef boost::shared_ptr<RfidReaderMacData> RfidReaderMacDataPtr;
class RfidTagApp;
typedef boost::shared_ptr<RfidTagApp> RfidTagAppPtr;

////////////////////////////////////////////////
// PacketData Subclass
/////////////////////////////////////////////////

/**
 * The representation of the MAC data in a RfidTagMac's
 * packet.
 */
class RfidTagMacData : public PacketData {
public:
	/// Smart pointer that clients should use.
	typedef boost::shared_ptr<RfidTagMacData> RfidTagMacDataPtr;

	/**
	 * Types enum.
	 * Specifies the types of packets that are sent.
	 */
	enum Types {
		Types_Generic, /**< enum value Types_Generic. */
		Types_Reply /**< enum value Types_Reply. */
	};

	/**
	 * A factory method to ensure that all objects
	 * are created via \c new since we are using smart pointers. 
	 */
	static inline RfidTagMacDataPtr create();

	/**
	 * A factory method to ensure that all objects
	 * are created via \c new since we are using smart pointers. 
	 */
	static inline RfidTagMacDataPtr create(
		const RfidTagMacData& rhs);

	/**
	 * Get the size of this data.
	 * @return the size of the data in bytes.
	 */
	virtual inline t_uint getSizeInBytes() const;

	/**
	 * Write the sender ID to the packet data.
	 * @param nodeId the ID to be written.
	 */
	void setSenderId(const NodeId& nodeId);

	/**
	 * Read the sender ID from the packet data.
	 * @return the ID of the node in the packet.
	 */
	NodeId getSenderId() const;

	/**
	 * Write the receiver ID to the packet data.
	 * @param nodeId the ID to be written.
	 */
	void setReceiverId(const NodeId& nodeId);

	/**
	 * Read the receiver ID from the packet data.
	 * @return the ID of the node in the packet.
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
	 * Return the stream representation of the object.
	 */
	inline ostream& print(ostream& s) const;

protected:

	/// A constructor.
	RfidTagMacData();

	/// A copy constructor.
	RfidTagMacData(const RfidTagMacData& rhs);

	/**
	 * Returns a pointer to a deep copy of this object.
	 * This is addresses the slicing problem with
	 * copy construction.
	 * @return a pointer to a deep copy of this object.
	 */
	virtual PacketDataPtr clone() const;

private:

	/// The default number of bytes for a tag ID.
	/// Most of the cheap tags seem to use 96 bits right now.
	static const t_uint m_senderIdBytes = 12;

	/// The number of bytes for the receiver ID.
	static const t_uint m_receiverIdBytes = 4;

	/// The default number of bytes for a packet type.
	static const t_uint m_typeBytes = 1;

	/// The data field for the sender ID.
	t_uchar m_senderId[m_senderIdBytes];

	/// The data field for the receiver ID.
	t_uchar m_receiverId[m_receiverIdBytes];

	/// The type field for the packet.
	Types m_type;

};
typedef boost::shared_ptr<RfidTagMacData> RfidTagMacDataPtr;

/////////////////////////////////////////////////
// RfidTagMac Class
/////////////////////////////////////////////////

/**
 * A MAC protocol for RFID tags that uses slotted ALOHA.
 */
class RfidTagMac : public SlottedMac {
public:
	/// Smart pointer that clients should use.
	typedef boost::shared_ptr<RfidTagMac> RfidTagMacPtr;

	/// A destructor.
	virtual ~RfidTagMac();

	/**
	 * A factory method to ensure that all objects
	 * are created via \c new since we are using smart pointers. 
	 * @param node the node that owns this object.
	 * @param tagApp a pointer to the application using this MAC.
	 */
	static inline RfidTagMacPtr create(NodePtr node, 
		RfidTagAppPtr tagApp);

	/**
	 * Gives \c this pointer as a \c RfidTagMac object.
	 * This is used to deal with \c shared_ptr's, \c this, and
	 * inheritance.
	 * @return a pointer to \c this object.
	 */
	virtual inline RfidTagMacPtr thisRfidTagMac();

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
	 * Handle a MAC packet the is received.
	 * @param packet a pointer to the received packet.
	 * @param sendingLayerIdx the index of the layer that sent the
	 * packet.
	 * @return true if the packet was able to be handled.
	 */
	bool handleRecvdMacPacket(PacketPtr packet, 
		t_uint sendingLayerIdx);

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
	 * @param packet the packet whose transmission was
	 * being attempted.
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

	/// The time which the node defers before transmitting
	/// an upper layer packet in a slot.
	static const double m_TAG_GENERIC_IFS;

	/// The time which the node defers before transmitting
	/// an reply packet in a slot.
	static const double m_TAG_REPLY_IFS;

	/// A pointer to the application that uses this MAC.
	RfidTagAppPtr m_tagApp;

	/// A constructor.
	RfidTagMac(NodePtr node, RfidTagAppPtr tagApp);

	/**
	 * The function called when a slot begins.
	 */
	virtual void beginSlotEvent();

	/**
	 * Handle the request packet by generating a reply packet
	 * and choosing a packet in which to send it.
	 * @param macData the MAC data from the reader's packet.
	 * @param sendingLayerIdx the layer from which the packet
	 * was received.
	 * @return true if the packet was handled successfully.
	 */
	bool handleRequestPacket(RfidReaderMacDataPtr macData, 
		t_uint sendingLayerIdx);

	/**
	 * Create a reply packet for the node.
	 * @param receiverId the ID of the destination for
	 * the packet.
	 * @return a pointer to the reply packet created by the function.
	 */
	PacketPtr createReplyPacket(NodeId receiverId) const;

	/**
	 * Add a generic header to the packet.
	 * @param packet a pointer to the packet to which the header
	 * will be added.
	 * @param receiverId the ID of the packet's destination.
	 */
	void addGenericHeader(PacketPtr packet, NodeId receiverId) const;

	/**
	 * Determine if the packet is for me based on the receiver
	 * address (i.e., has my address or the broadcast address).
	 * @param macData the MAC header of the packet.
	 */
	bool packetIsForMe(RfidReaderMacDataPtr macData) const;

	/**
	 * Determine whether the packet is a packet of the
	 * given type by looking at it's header.
	 * @param packet a pointer to the packet under consideration.
	 * @param type the type for which we are checking.
	 * @return true if it is a request packet.
	 */
	bool isPacketType(PacketPtr packet, 
		RfidTagMacData::Types type) const;

private:

	static const bool m_DEBUG = true;

	/// An internal pointer to the object to allow it
	/// to return \c this.
	boost::weak_ptr<RfidTagMac> m_weakThis;

};
typedef boost::shared_ptr<RfidTagMac> RfidTagMacPtr;

/////////////////////////////////////////////////
// Inline Functions
/////////////////////////////////////////////////

inline RfidTagMacPtr RfidTagMac::create(NodePtr node, 
	RfidTagAppPtr tagApp)
{
	RfidTagMacPtr p(new RfidTagMac(node, tagApp));
	// weakThis *must* be set before the this* functions are called.
	p->m_weakThis = p;

	SlottedMacSlotEventPtr slotEvent = 
		SlottedMacSlotEvent::create(p->thisSlottedMac());
	p->m_slotTimer = Timer::create(p->getNode(), slotEvent);
	p->m_slotTimer->start(SimTime(0.0));

	Simulator::instance()->addSimulationEndListener(
		p->thisSimulationEndListener());

	return p;
}

inline RfidTagMacPtr RfidTagMac::thisRfidTagMac()
{
	RfidTagMacPtr p(m_weakThis);
	return p;
}

inline SlottedMacPtr RfidTagMac::thisSlottedMac()
{
	SlottedMacPtr p(m_weakThis);
	return p;
}

inline MacProtocolPtr RfidTagMac::thisMacProtocol()
{
	MacProtocolPtr p(m_weakThis);
	return p;
}

inline SimulationEndListenerPtr RfidTagMac::thisSimulationEndListener()
{
	SimulationEndListenerPtr p(m_weakThis);
	return p;
}

////////////////////////////////////////////////
// Overloaded Operators
/////////////////////////////////////////////////

/////////////////////////////////////////////////
// RfidTagMacData Inline Functions
/////////////////////////////////////////////////

inline RfidTagMacDataPtr RfidTagMacData::create()
{
	RfidTagMacDataPtr p(new RfidTagMacData());
	return p;
}

inline RfidTagMacDataPtr RfidTagMacData::create(const RfidTagMacData& rhs)
{
	RfidTagMacDataPtr p = 
		boost::dynamic_pointer_cast<RfidTagMacData>(rhs.clone());
	// If the shared_ptr is empty, then the cast failed.
	assert(p.get() != 0);
	return p;
}

inline void RfidTagMacData::setType(RfidTagMacData::Types type)
{
	m_type = type;
}

inline RfidTagMacData::Types RfidTagMacData::getType() const
{
	return m_type;
}


inline t_uint RfidTagMacData::getSizeInBytes() const
{
	return (m_senderIdBytes + m_receiverIdBytes + m_typeBytes);
}

/////////////////////////////////////////////////
// Overloaded Operators
/////////////////////////////////////////////////

inline ostream& operator<< (ostream& s, 
		const RfidTagMacData::Types& dataType)
{
	switch(dataType) {
	case RfidTagMacData::Types_Generic:
		s << "GENERIC";
		break;
	case RfidTagMacData::Types_Reply:
		s << "REPLY";
		break;
	}
	return s;
}

inline ostream& RfidTagMacData::print(ostream& s) const
{
	s << "type=" << m_type << ", " <<
		"senderId=" << 
		NodeId(m_senderId, m_senderIdBytes) << ", " <<
		"receiverId=" << 
		NodeId(m_receiverId, m_receiverIdBytes);
	return s;
}


#endif // RFID_TAG_MAC_H

