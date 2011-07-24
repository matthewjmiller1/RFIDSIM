
#include "link_layer.hpp"
#include "mac_protocol.hpp"
#include "physical_layer.hpp"

const t_uint LinkLayer::m_LINK_LAYER_QUEUE_LENGTH = 1;

LinkLayer::LinkLayer(NodePtr node, MacProtocolPtr macProtocol)
	: CommunicationLayer(node), m_macProtocol(macProtocol)
{
	setMaxQueueLength(m_LINK_LAYER_QUEUE_LENGTH);
}

void LinkLayer::simulationEndHandler()
{

}

bool LinkLayer::channelIsBusy()
{
	CommunicationLayer::Directions direction = 
		CommunicationLayer::Directions_Lower;
	PhysicalLayerPtr physicalLayer = 
		boost::dynamic_pointer_cast<PhysicalLayer>
		(getLayer(direction, getDefaultLayer(direction)));
	assert(physicalLayer.get() != 0);
	return physicalLayer->channelCarrierSensedBusy();
}

bool LinkLayer::recvFromMacProtocol(
	CommunicationLayer::Directions direction, PacketPtr packet)
{
	// Use the default function with packet passed back from
	// the MAC.
	return CommunicationLayer::sendToLayer(direction, packet);
}

bool LinkLayer::recvFromLayer(
	CommunicationLayer::Directions direction, PacketPtr packet,
	t_uint sendingLayerIdx)
{
	assert(m_macProtocol.get() != 0);
	return m_macProtocol->recvFromLinkLayer(
		direction, packet, sendingLayerIdx);
}


