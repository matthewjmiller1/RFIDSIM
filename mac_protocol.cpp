
#include "mac_protocol.hpp"
#include "packet.hpp"
#include "link_layer.hpp"

const double SlottedMac::m_DEFAULT_SLOT_TIME = 2.0e-3;

MacProtocol::MacProtocol(NodePtr node)
	: m_node(node)
{

}

MacProtocol::~MacProtocol()
{

}

void MacProtocol::setLinkLayer(LinkLayerPtr linkLayer)
{
	// assert(m_node == linkLayer->getNode());
	m_linkLayer = linkLayer;
}

void MacProtocol::blockUpperQueues()
{
	LinkLayerPtr sharedLinkLayer = m_linkLayer.lock();
	sharedLinkLayer->blockUpperQueues();
}

void MacProtocol::unblockUpperQueues()
{
	LinkLayerPtr sharedLinkLayer = m_linkLayer.lock();
	sharedLinkLayer->unblockUpperQueues();
}

bool MacProtocol::getQueueIsBlocked() const
{
	LinkLayerPtr sharedLinkLayer = m_linkLayer.lock();
	return sharedLinkLayer->getQueueIsBlocked();
}

bool MacProtocol::startSendTimer(
	CommunicationLayer::Directions direction, PacketPtr packet,
	const SimTime& delay)
{
	if(m_sendTimer.get() != 0)
		assert(!m_sendTimer->isRunning());

	SendToLinkLayerEventPtr sendEvent = 
		SendToLinkLayerEvent::create(thisMacProtocol(), direction, packet);
	m_sendTimer = Timer::create(getNode(), sendEvent);
	bool wasSuccessful = m_sendTimer->start(delay);
	return wasSuccessful;
}

bool MacProtocol::sendToLinkLayer(
	CommunicationLayer::Directions direction, PacketPtr packet)
{
	bool wasSuccessful = false;
	LinkLayerPtr sharedLinkLayer = m_linkLayer.lock();
	// If down, check if the channel is busy and call
	// a function if so to allow subclasses to handle this event.
	if(direction == CommunicationLayer::Directions_Lower &&
			sharedLinkLayer->channelIsBusy()) {
		handleChannelBusy(packet);
	} else {
		handlePacketSent(packet);
		wasSuccessful = sharedLinkLayer->recvFromMacProtocol(direction,
			packet);
	}
	return wasSuccessful;
}

bool MacProtocol::recvFromLinkLayer(
	CommunicationLayer::Directions direction, PacketPtr packet,
	t_uint sendingLayerIdx)
{

	bool wasSuccessful = false;


	switch(direction) {
	case CommunicationLayer::Directions_Lower:
		wasSuccessful = handleRecvdMacPacket(packet, sendingLayerIdx);
		/*
		if(packet->hasUpperLayerData(Packet::DataTypes_Link)) {
			assert(!m_linkLayer.expired());
			LinkLayerPtr sharedLinkLayer = m_linkLayer.lock();
			wasSuccessful = sharedLinkLayer->recvFromMacProtocol(direction,
				packet);
		*/
		break;
	case CommunicationLayer::Directions_Upper:
		wasSuccessful = handleRecvdUpperLayerPacket(packet, 
			sendingLayerIdx);
		break;
	default:
		assert(false);
	}

	return wasSuccessful;
}


