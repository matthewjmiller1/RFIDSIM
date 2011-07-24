
#include "rfid_reader_mac.hpp"
#include "packet.hpp"
#include "rfid_tag_mac.hpp"
#include "rfid_reader_app.hpp"
#include "log_stream_manager.hpp"

const double RfidReaderMac::m_READER_IFS = 10e-6;
const t_uint RfidReaderMac::m_DEFAULT_NUMBER_OF_SLOTS = 10;
const double RfidReaderMac::m_DEFAULT_CYCLE_TIME = 5.25;
const t_uint RfidReaderMac::m_MISSED_READ_THRESHOLD = 3;

const string RfidReaderMac::m_MISSED_READ_TOTAL_STRING = "missedReadTotal";
const string RfidReaderMac::m_MISSED_READ_SLOT_AVG_STRING = "missedReadSlotAvg";
const string RfidReaderMac::m_WINNING_SLOT_AVG_STRING = "winningSlotAvg";

RfidReaderMac::RfidReaderMac(NodePtr node, RfidReaderAppPtr readerApp)
	: SlottedMac(node), 
	m_doResetSlot(false), m_resetSlotNumber(0),
	m_doEntireReadCycle(false), m_missedReadCount(0),
	m_nextCycleNumberOfSlots(m_DEFAULT_NUMBER_OF_SLOTS),
	m_nextCycleTime(m_DEFAULT_CYCLE_TIME), m_readerApp(readerApp)
{
	setSlotTime(SimTime(m_DEFAULT_SLOT_TIME));
}

RfidReaderMac::~RfidReaderMac()
{

}

void RfidReaderMac::simulationEndHandler()
{
	t_uint missedReadSlotSum = 0;
	for(t_uint i = 0; i < m_missedReads.size(); ++i) {
		missedReadSlotSum += m_missedReads[i];
	}
	double missedReadSlotAvg = 0.0;
	if(m_missedReads.size() > 0)
		missedReadSlotAvg = static_cast<double>(missedReadSlotSum) / 
			m_missedReads.size();

	ostringstream missedReadTotalStream;
	missedReadTotalStream << m_missedReads.size();
	LogStreamManager::instance()->logStatsItem(getNode()->getNodeId(),
		m_MISSED_READ_TOTAL_STRING, missedReadTotalStream.str());

	ostringstream missedReadSlotAvgStream;
	missedReadSlotAvgStream << missedReadSlotAvg;
	LogStreamManager::instance()->logStatsItem(getNode()->getNodeId(),
		m_MISSED_READ_SLOT_AVG_STRING, missedReadSlotAvgStream.str());

	t_uint winningSlotSum = 0;
	for(t_uint i = 0; i < m_winningSlotNumbers.size(); ++i) {
		winningSlotSum += m_winningSlotNumbers[i].second;
	}
	double winningSlotAvg = 0.0;
	if(m_winningSlotNumbers.size() > 0)
		winningSlotAvg = static_cast<double>(winningSlotSum) / 
			m_winningSlotNumbers.size();

	ostringstream winningSlotAvgStream;
	winningSlotAvgStream << winningSlotAvg;
	LogStreamManager::instance()->logStatsItem(getNode()->getNodeId(),
		m_WINNING_SLOT_AVG_STRING, winningSlotAvgStream.str());

}

bool RfidReaderMac::isEnoughTimeForContentionCycle() const
{
	// We need one extra slot be cycle to send the REQUEST
	// packet.
	double nextContentionCycleTime = 
	(m_nextCycleNumberOfSlots + 1) * m_DEFAULT_SLOT_TIME;

	if(m_DEBUG_CONTENTION_CYCLE_TIME) {
		SimTime timeRemaining = m_cycleTimer->timeRemaining();
		if(timeRemaining > 0.0) {
			ostringstream debugStream;
			debugStream << __PRETTY_FUNCTION__ <<
			" nextCycleTime=" << nextContentionCycleTime <<
			", readCycleRemaining=" << timeRemaining;
			LogStreamManager::instance()->logDebugItem(
				debugStream.str());
		}
	}

	return (SimTime(nextContentionCycleTime) < 
			m_cycleTimer->timeRemaining());
}

void RfidReaderMac::beginSlotEvent()
{
	// Check for a transmission slot before checking for the end
	// slot.  This lets us transmit on the next slot after calling
	// stopContentionCycle() without triggering the actions
	// for the end slot.

	if(m_currentSlotNumber == m_txSlotNumber) {
		if(m_packetToTransmit.get() != 0) {
			if(m_DEBUG) {
				ostringstream debugStream;
				debugStream << __PRETTY_FUNCTION__ <<
				" is enough time, txSlot=" << m_txSlotNumber <<
				", currentSlot=" << m_currentSlotNumber;
				LogStreamManager::instance()->logDebugItem(
					debugStream.str());
			}

			startSendTimer(CommunicationLayer::Directions_Lower, 
				m_packetToTransmit, m_READER_IFS);
			m_packetToTransmit.reset();
		}
	} else if(m_numberOfSlots == 0 ||
			m_currentSlotNumber >= m_numberOfSlots ||
			(m_doResetSlot && m_currentSlotNumber == m_resetSlotNumber)) {
		// Send another request if there is sufficient time in
		// the current cycle.  Note that this occurs when
		// no tags were read in a contention cycle.  There is a one
		// slot delay between the last slot of the previous contention
		// cycle and when the next REQUEST packet gets sent.  This
		// is needed to avoid stopping the contention cycle when
		// a tag is transmitting data in the last slot of the
		// contention cycle.
		assert(m_packetToTransmit.get() == 0);

		ostringstream debugStream;
		debugStream << __PRETTY_FUNCTION__ << 
			" currentSlot: " << m_currentSlotNumber << " resetSlot: " <<
			m_resetSlotNumber << " numSlots: " << m_numberOfSlots <<
			" missedReadCount: " << m_missedReadCount <<
			boolalpha << " doReset: " << m_doResetSlot <<
			" isEnoughCycleTime: " << isEnoughTimeForContentionCycle();
		LogStreamManager::instance()->logDebugItem(
			debugStream.str());

		// We were unable to successfully read a packet
		// in the given interval.  We only do this if we
		// are currently in a read interval (i.e., the timer
		// is running).
		if(!m_doEntireReadCycle && m_cycleTimer->isRunning()) {
			m_missedReads.push_back(m_currentSlotNumber);
			m_missedReadCount++;
		} else
			m_missedReadCount = 0;

		m_doResetSlot = false;
		//cout << "stopCycle1" << endl;
		stopContentionCycle();

		if(!m_doEntireReadCycle &&
				m_missedReadCount == m_MISSED_READ_THRESHOLD) {
			// The timer should be running since, otherwise,
			// the missedReadCount was reset to zero above.
			assert(m_cycleTimer->isRunning());
			// If we have missed too many consecutive reads, then
			// we will force the current read cycle to stop.
			m_cycleTimer->stop();
			endRequestCycleEvent();
		} else {
			if(isEnoughTimeForContentionCycle()) {
				m_packetToTransmit = createRequestPacket();
				// Have to set this for the next slot number since
				// currentSlotNumber is incremented at the end of this
				// function.
				m_txSlotNumber = m_currentSlotNumber + 1;
			} else if(!m_cycleTimer->isRunning()) {
				assert(!getQueueIsBlocked());
			}
		}
	}

	m_currentSlotNumber++;
	assert(m_slotTimer.get() != 0);
	m_slotTimer->reschedule(getSlotTime());
}

void RfidReaderMac::endRequestCycleEvent()
{
	assert(!inContentionCycle());
	m_currentAppReadPacket.reset();
	unblockUpperQueues();
	m_readerApp->signalReadEnd();
}

void RfidReaderMac::handleChannelBusy(PacketPtr packet)
{
	// For now, we will just silently drop the packet
	// rather than implement a retransmission scheme.
	if(!isPacketType(packet, RfidReaderMacData::Types_Request) && 
			!isPacketType(packet, RfidReaderMacData::Types_Select)) {
		unblockUpperQueues();
	}
}

void RfidReaderMac::handlePacketSent(PacketPtr packet)
{
	if(isPacketType(packet, RfidReaderMacData::Types_Request)) {
		// Start a cycle
		m_currentSlotNumber = 0;
		RfidReaderMacDataPtr macData =
			boost::dynamic_pointer_cast<RfidReaderMacData>
			(packet->getData(Packet::DataTypes_Link));
		assert(macData.get() != 0);
		m_numberOfSlots = macData->getNumberOfSlots();
	} else if(isPacketType(packet, RfidReaderMacData::Types_Select)) {
		m_doResetSlot = true;
		// m_currentSlotNumber is already set to slot i+1, so
		// the resetSlotNumber should be slot i+2.  Thus, we
		// add one to currentSlotNumber.
		m_resetSlotNumber = m_currentSlotNumber + 1;
		/*
		cout << "slot: " << m_currentSlotNumber << " resetSlot: " <<
			m_resetSlotNumber << endl;
		*/
	} else if(isPacketType(packet, RfidReaderMacData::Types_Ack)) {
		if(isEnoughTimeForContentionCycle()) {
			startNextContentionCycle();
		}
	} else {
		unblockUpperQueues();
	}
}

bool RfidReaderMac::isPacketType(PacketPtr packet, 
	RfidReaderMacData::Types type) const
{
	bool isType = false;
	RfidReaderMacDataPtr macData =
		boost::dynamic_pointer_cast<RfidReaderMacData>
		(packet->getData(Packet::DataTypes_Link));
	if(macData.get() != 0 && macData->getType() == type) {
		isType = true;
	}
	return isType;
}

bool RfidReaderMac::packetIsForMe(RfidTagMacDataPtr macData) const
{
	return (macData->getReceiverId() == getNode()->getNodeId() ||
		macData->getReceiverId() == NodeId::broadcastDestination());
}

void RfidReaderMac::startNextContentionCycle()
{
	// TODO: Remove this before the assertion.
	if(m_packetToTransmit.get() != 0) {
		ostringstream debugStream;
		debugStream << __PRETTY_FUNCTION__ << *m_packetToTransmit <<
			" curSlot: " << m_currentSlotNumber << ", numSlots: " <<
			m_numberOfSlots;
		LogStreamManager::instance()->logDebugItem(
			debugStream.str());
	}
	assert(m_packetToTransmit.get() == 0);
	m_packetToTransmit = createRequestPacket();
	m_missedReadCount = 0;
	m_doResetSlot = false;
	//cout << "stopCycle2" << endl;
	stopContentionCycle();
	m_txSlotNumber = m_currentSlotNumber;
}

bool RfidReaderMac::handleRecvdMacPacket(PacketPtr packet, 
	t_uint sendingLayerIdx)
{
	RfidTagMacDataPtr macData =
		boost::dynamic_pointer_cast<RfidTagMacData>
		(packet->getData(Packet::DataTypes_Link));

	bool wasSuccessful = true;

	// For now, we'll only handle MAC packets from tags.
	if(macData.get() != 0) {
		if(packetIsForMe(macData)) {
			switch(macData->getType()) {
			case RfidTagMacData::Types_Reply:
				// If we ended the cycle early due to too many
				// consecutive missed reads, then the timer will
				// be stopped and we shouldn't handle anymore
				// REPLY packets.
				if(m_cycleTimer->isRunning()) {
					// It could be the case that this reply is received
					// in a contention cycle after the a SELECT packet
					// was lost.  In response to the SELECT packet being
					// lost in slot i, the reader will reset in
					// slot i+2 and send a REQUEST in slot i+3.
					// If this reply is received in slot i+2, we should
					// ignore it since m_packetToTransmit already
					// has a REQUEST packet pending.
					if(m_packetToTransmit.get() == 0) {
						assert(m_currentAppReadPacket.get() != 0);
						// Send SELECT message header on the original
						// app packet.
						addSelectHeader(m_currentAppReadPacket, 
							macData->getSenderId());
						m_packetToTransmit = m_currentAppReadPacket;
						m_txSlotNumber = m_currentSlotNumber;
						assert(m_slotTimer.get() != 0 && 
							m_slotTimer->isRunning());
					}
				}
				break;
			case RfidTagMacData::Types_Generic:
				// Subtract three from the winning slot number for:
				// 1. The current slot number was incremented
				// at the beginning of the slot.
				// 2. The REPLY was sent two slots prior
				// to this packet being received (the SELECT
				// packet was sent one slot prior).
				m_winningSlotNumbers.push_back(
					make_pair(macData->getSenderId(), 
					(m_currentSlotNumber - 3)));
				// Just pass the packet to upper layers.
				wasSuccessful = sendToLinkLayer(
					CommunicationLayer::Directions_Upper, packet);
				m_packetToTransmit = createAckPacket(macData->getSenderId());
				m_txSlotNumber = m_currentSlotNumber;
				assert(m_slotTimer.get() != 0 && 
					m_slotTimer->isRunning());
				break;
			default:
				wasSuccessful = false;
			}
		}
	}

	return wasSuccessful;
}

PacketPtr RfidReaderMac::createRequestPacket() const
{
	RfidReaderMacDataPtr macData = RfidReaderMacData::create();
	macData->setType(RfidReaderMacData::Types_Request);
	macData->setSenderId(getNode()->getNodeId());
	macData->setReceiverId(NodeId::broadcastDestination());
	// We must have at least: (1) a contention slot,
	// (2) a slot for the SELECT to be sent by the ready,
	// (3) a slot for the tag to reply with an app packet,
	// (4) a slot for the reader to reply with an ACK.
	assert(m_nextCycleNumberOfSlots >= 4);
	macData->setNumberOfSlots(m_nextCycleNumberOfSlots);
	PacketPtr packet = Packet::create();
	assert(m_currentAppReadPacket.get() != 0);
	packet->setTxPower(m_currentAppReadPacket->getTxPower());
	packet->addData(Packet::DataTypes_Link, *macData);
	return packet;
}

PacketPtr RfidReaderMac::createAckPacket(NodeId destination) const
{
	RfidReaderMacDataPtr macData = RfidReaderMacData::create();
	macData->setType(RfidReaderMacData::Types_Ack);
	macData->setSenderId(getNode()->getNodeId());
	macData->setReceiverId(destination);

	PacketPtr packet = Packet::create();
	packet->setDoMaxTxPower(true);
	packet->addData(Packet::DataTypes_Link, *macData);
	return packet;
}

void RfidReaderMac::addGenericHeader(PacketPtr packet, 
	NodeId receiverId) const
{
	RfidReaderMacDataPtr macData = RfidReaderMacData::create();
	macData->setType(RfidReaderMacData::Types_Generic);
	macData->setSenderId(getNode()->getNodeId());
	macData->setReceiverId(receiverId);
	packet->addData(Packet::DataTypes_Link, *macData);
}

void RfidReaderMac::addSelectHeader(PacketPtr packet, 
	NodeId receiverId) const
{
	RfidReaderMacDataPtr macData = RfidReaderMacData::create();
	macData->setType(RfidReaderMacData::Types_Select);
	macData->setSenderId(getNode()->getNodeId());
	macData->setReceiverId(receiverId);
	packet->setDoMaxTxPower(true);
	packet->addData(Packet::DataTypes_Link, *macData);
}

bool RfidReaderMac::handleRecvdUpperLayerPacket(PacketPtr packet,
	t_uint sendingLayerIdx)
{
	RfidReaderAppDataPtr appData =
		boost::dynamic_pointer_cast<RfidReaderAppData>
		(packet->getData(Packet::DataTypes_Application));

	bool wasSuccessful = false;

	if(m_DEBUG) {
		ostringstream debugStream;
		debugStream << __PRETTY_FUNCTION__;
		LogStreamManager::instance()->logDebugItem(
			debugStream.str());
	}

	// For now, we only handle application packets.
	if(appData.get() != 0) {
		switch(appData->getType()) {
		case RfidReaderAppData::Types_Read:
			// We'll only handle one packet at a time
			blockUpperQueues();

			assert(m_currentAppReadPacket.get() == 0);
			m_currentAppReadPacket = packet;
			m_doEntireReadCycle = appData->getDoEntireReadCycle();
			// Start cycle timer
			assert(m_cycleTimer.get() != 0);
			m_cycleTimer->start(m_nextCycleTime);
			if(isEnoughTimeForContentionCycle()) {
				startNextContentionCycle();
			}
			wasSuccessful = true;
			break;
		case RfidReaderAppData::Types_Reset:
			// We'll only handle one packet at a time
			blockUpperQueues();

			assert(m_packetToTransmit == 0);
			addGenericHeader(packet, NodeId::broadcastDestination());
			m_packetToTransmit = packet;
			m_txSlotNumber = m_currentSlotNumber;
			wasSuccessful = true;
			break;
		default:
			wasSuccessful = false;
		}
		assert(m_slotTimer.get() != 0 && m_slotTimer->isRunning());
	}

	return wasSuccessful;
}

////////////////////////////////////////////////
// RfidReaderMacData functions
/////////////////////////////////////////////////

RfidReaderMacData::RfidReaderMacData()
	: m_numberOfSlots(0), m_type(RfidReaderMacData::Types_Generic)
{
	fill(m_senderId, &m_senderId[m_senderIdBytes], 0);
	fill(m_receiverId, &m_receiverId[m_receiverIdBytes], 0);
}

RfidReaderMacData::RfidReaderMacData(const RfidReaderMacData& rhs)
	: PacketData(rhs), m_numberOfSlots(rhs.m_numberOfSlots), 
	m_type(rhs.m_type)
{
	copy(rhs.m_senderId, &rhs.m_senderId[m_senderIdBytes], m_senderId);
	copy(rhs.m_receiverId, &rhs.m_receiverId[m_receiverIdBytes], 
		m_receiverId);
}

PacketDataPtr RfidReaderMacData::clone() const
{
	 PacketDataPtr p(new RfidReaderMacData(*this));
	 return p;
}

void RfidReaderMacData::setSenderId(const NodeId& nodeId)
{
	nodeId.writeToByteArray(m_senderId, m_senderIdBytes);
}

NodeId RfidReaderMacData::getSenderId() const
{
	NodeId nodeId(m_senderId, m_senderIdBytes);
	return nodeId;
}

void RfidReaderMacData::setReceiverId(const NodeId& nodeId)
{
	nodeId.writeToByteArray(m_receiverId, m_receiverIdBytes);
}

NodeId RfidReaderMacData::getReceiverId() const
{
	NodeId nodeId(m_receiverId, m_receiverIdBytes);
	return nodeId;
}


