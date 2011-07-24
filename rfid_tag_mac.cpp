
#include "rfid_tag_mac.hpp"
#include "rfid_tag_app.hpp"
#include "rfid_reader_mac.hpp"
#include "rand_num_generator.hpp"
#include "simulator.hpp"
#include "log_stream_manager.hpp"

const double RfidTagMac::m_TAG_GENERIC_IFS = 15e-6;
const double RfidTagMac::m_TAG_REPLY_IFS = 20e-6;

RfidTagMac::RfidTagMac(NodePtr node, RfidTagAppPtr tagApp)
	: SlottedMac(node), m_tagApp(tagApp)
{
	setSlotTime(SimTime(m_DEFAULT_SLOT_TIME));
}

RfidTagMac::~RfidTagMac()
{

}

void RfidTagMac::simulationEndHandler()
{

}

void RfidTagMac::beginSlotEvent()
{
	if(m_currentSlotNumber == m_txSlotNumber) {
		if(m_packetToTransmit.get() != 0) {

			// Set the IFS delay based on the packet type.
			double ifsDelay = m_TAG_GENERIC_IFS;
			if(isPacketType(m_packetToTransmit, RfidTagMacData::Types_Reply))
				ifsDelay = m_TAG_REPLY_IFS;

			startSendTimer(CommunicationLayer::Directions_Lower, 
				m_packetToTransmit, ifsDelay);
			m_packetToTransmit.reset();
		}
	} else if(m_numberOfSlots == 0 || 
			m_currentSlotNumber >= (m_numberOfSlots - 1)) {
		// If the current slot is m_numberOfSlots - 1, then
		// we've reached the last slot and we didn't transmit
		// in it, so we can stop.
		assert(m_packetToTransmit.get() == 0);
		stopContentionCycle();
		unblockUpperQueues();
	}

	m_currentSlotNumber++;
	assert(m_slotTimer.get() != 0);
	m_slotTimer->reschedule(getSlotTime());
}

void RfidTagMac::handleChannelBusy(PacketPtr packet)
{
	if(isPacketType(packet, RfidTagMacData::Types_Reply))
		stopContentionCycle();
	unblockUpperQueues();
}

void RfidTagMac::handlePacketSent(PacketPtr packet)
{
	if(isPacketType(packet, RfidTagMacData::Types_Generic)) {
		stopContentionCycle();
		unblockUpperQueues();
	}
}

bool RfidTagMac::isPacketType(PacketPtr packet, 
	RfidTagMacData::Types type) const
{
	bool isType = false;
	RfidTagMacDataPtr macData =
		boost::dynamic_pointer_cast<RfidTagMacData>
		(packet->getData(Packet::DataTypes_Link));
	if(macData.get() != 0 && macData->getType() == type) {
		isType = true;
	}
	return isType;
}

PacketPtr RfidTagMac::createReplyPacket(NodeId receiverId) const
{
	RfidTagMacDataPtr macData = RfidTagMacData::create();
	macData->setType(RfidTagMacData::Types_Reply);
	macData->setSenderId(getNode()->getNodeId());
	macData->setReceiverId(receiverId);
	PacketPtr packet = Packet::create();
	packet->addData(Packet::DataTypes_Link, *macData);
	return packet;
}

bool RfidTagMac::handleRequestPacket(RfidReaderMacDataPtr macData, 
	t_uint sendingLayerIdx)
{
	if(!inContentionCycle()) {
		// Reset the current cycle
		m_currentSlotNumber = 0;
		// Get the number of slots from the read packet
		m_numberOfSlots = macData->getNumberOfSlots();
		// We must have at least: (1) a contention slot,
		// (2) a slot for the SELECT to be sent by the ready,
		// (3) a slot for the tag to reply with an app packet,
		// (4) a slot for the reader to reply with an ACK.
		assert(m_numberOfSlots >= 4);
		// Choose a slot uniformly at random.
		if(m_numberOfSlots > 0) {
			RandNumGeneratorPtr rand = 
				Simulator::instance()->getRandNumGenerator();
			m_txSlotNumber = rand->uniformInt(0, m_numberOfSlots - 4);
			assert(m_packetToTransmit.get() == 0);
			if(m_tagApp->getReplyToReads()) {
				// Only create a reply packet if the tag state
				// allows it to reply.
				m_packetToTransmit = 
					createReplyPacket(macData->getSenderId());
				if(m_DEBUG) {
					ostringstream debugStream;
					debugStream << __PRETTY_FUNCTION__ <<
					" nodeId=" << getNode()->getNodeId() <<
					", txSlotNumber=" << m_txSlotNumber <<
					", currentSlot=" << m_currentSlotNumber;
					LogStreamManager::instance()->logDebugItem(
						debugStream.str());
				} // end if DEBUG
			} // end if tag can reply
		} // end if number of slots > 0
	} // end in contention cycle
	return true;
}

bool RfidTagMac::packetIsForMe(RfidReaderMacDataPtr macData) const
{
	return (macData->getReceiverId() == getNode()->getNodeId() ||
		macData->getReceiverId() == NodeId::broadcastDestination());
}

bool RfidTagMac::handleRecvdMacPacket(PacketPtr packet,
	t_uint sendingLayerIdx)
{
	RfidReaderMacDataPtr macData =
		boost::dynamic_pointer_cast<RfidReaderMacData>
		(packet->getData(Packet::DataTypes_Link));

	bool wasSuccessful = true;

	// For now, we'll only handle reader packets.
	if(macData.get() != 0) {
		switch(macData->getType()) {
		case RfidReaderMacData::Types_Request:
			assert(macData->getReceiverId() == 
				NodeId::broadcastDestination());
			wasSuccessful = handleRequestPacket(macData, sendingLayerIdx);
			break;
		case RfidReaderMacData::Types_Select:
			if(macData->getReceiverId() == getNode()->getNodeId()) {
				// If we were selected for a slot, send the packet
				// to the upper layer.
				wasSuccessful = sendToLinkLayer(
					CommunicationLayer::Directions_Upper, packet);
			} else {
				// Otherwise, stop the contention cycle
				stopContentionCycle();
				m_packetToTransmit.reset();
				unblockUpperQueues();
			}
			break;
		case RfidReaderMacData::Types_Generic:
			if(packetIsForMe(macData)) {
				// Just pass the packet to upper layers.
				wasSuccessful = sendToLinkLayer(
					CommunicationLayer::Directions_Upper, packet);
			}
			break;
		case RfidReaderMacData::Types_Ack:
			// We'll set the reply bit here.
			// If the ACK is received, then we stop replying
			// until a reset packet is received.
			if(packetIsForMe(macData))
				m_tagApp->setReplyToReads(false);
			break;
		default:
			wasSuccessful = false;
			assert(false);
		} // end switch
	} // end packet is not null
	
	return wasSuccessful;
}


void RfidTagMac::addGenericHeader(PacketPtr packet,
   NodeId receiverId) const
{
   RfidTagMacDataPtr macData = RfidTagMacData::create();
   macData->setType(RfidTagMacData::Types_Generic);
   macData->setSenderId(getNode()->getNodeId());
   macData->setReceiverId(receiverId);
   packet->addData(Packet::DataTypes_Link, *macData);
}

bool RfidTagMac::handleRecvdUpperLayerPacket(PacketPtr packet,
	t_uint sendingLayerIdx)
{
	RfidTagAppDataPtr appData =
		boost::dynamic_pointer_cast<RfidTagAppData>
		(packet->getData(Packet::DataTypes_Application));

	bool wasSuccessful = false;

	// For now, we only handle application packets.
	if(appData.get() != 0) {
		// We'll only handle one packet at a time.
		blockUpperQueues();
		assert(m_packetToTransmit.get() == 0);
		// This should be an ID packet and it should be
		// sent immediately since the node has already
		// won its contention period.
		// The underlying assumptions are:
		// 1. That this packet
		// reaches the MAC fast enough that it is sent in
		// the next slot after the select packet was received.
		// 2. That this is the packet generated in response
		// to the select packet.
		m_packetToTransmit = packet;
		addGenericHeader(packet, packet->getDestination());
		// The current slot number is incremented at the end
		// of the beginSlot function.  So, this will cause
		// the packet to be transmitted in the next slot.
		m_txSlotNumber = m_currentSlotNumber;
		if(m_DEBUG) {
			ostringstream debugStream;
			debugStream << __PRETTY_FUNCTION__ <<
			" txSlot=" << m_txSlotNumber <<
			", currentSlot=" << m_currentSlotNumber <<
			", numberOfSlots=" << m_numberOfSlots;
			LogStreamManager::instance()->logDebugItem(
				debugStream.str());
		}

		assert(m_slotTimer.get() != 0 && 
			m_slotTimer->isRunning() && inContentionCycle());
	}
	return wasSuccessful;
}

/////////////////////////////////////////////////
// RfidTagMacData Class
/////////////////////////////////////////////////

RfidTagMacData::RfidTagMacData()
	: m_type(RfidTagMacData::Types_Generic)
{
	fill(m_senderId, &m_senderId[m_senderIdBytes], 0);
	fill(m_receiverId, &m_receiverId[m_receiverIdBytes], 0);
}

RfidTagMacData::RfidTagMacData(const RfidTagMacData& rhs)
	: PacketData(rhs), m_type(rhs.m_type)
{
	copy(rhs.m_senderId, &rhs.m_senderId[m_senderIdBytes], m_senderId);
	copy(rhs.m_receiverId, &rhs.m_receiverId[m_receiverIdBytes], 
		m_receiverId);
}

PacketDataPtr RfidTagMacData::clone() const
{
	 PacketDataPtr p(new RfidTagMacData(*this));
	 return p;
}

void RfidTagMacData::setSenderId(const NodeId& nodeId)
{
	nodeId.writeToByteArray(m_senderId, m_senderIdBytes);
}

NodeId RfidTagMacData::getSenderId() const
{
	NodeId nodeId(m_senderId, m_senderIdBytes);
	return nodeId;
}

void RfidTagMacData::setReceiverId(const NodeId& nodeId)
{
	nodeId.writeToByteArray(m_receiverId, m_receiverIdBytes);
}

NodeId RfidTagMacData::getReceiverId() const
{
	NodeId nodeId(m_receiverId, m_receiverIdBytes);
	return nodeId;
}


