
#include "communication_layer.hpp"
#include "node.hpp"
#include "log_stream_manager.hpp"
#include "packet.hpp"

const t_uint CommunicationLayer::m_DEFAULT_MAX_QUEUE_LENGTH = 50;

CommunicationLayer::CommunicationLayer(NodePtr node)
	: m_lowerLayerRecvEventPending(false),
	m_node(node), m_lowerLayerDelay(0.0), m_upperLayerDelay(0.0),
	m_defaultLowerLayerIdx(0), m_defaultUpperLayerIdx(0),
	m_maxQueueLength(m_DEFAULT_MAX_QUEUE_LENGTH), m_queueIsBlocked(false)
{

}

CommunicationLayer::~CommunicationLayer()
{

}

void CommunicationLayer::blockUpperQueues()
{
	for(t_uint i = 0; i < m_upperLayers.size(); ++i) {
		m_upperLayers[i]->blockQueue();
	}
}

void CommunicationLayer::unblockUpperQueues()
{
	t_uint i = 0;
	while(!queueIsFull() && i < m_upperLayers.size()) {
		m_upperLayers[i]->unblockQueue();
		++i;
	}
}

void CommunicationLayer::sendFromQueue()
{
	while(!m_lowerLayerRecvEventPending && !m_queueIsBlocked && 
			!m_packetQueue.empty()) {
		pair<PacketPtr,t_uint> queueElement = m_packetQueue.front();
		m_packetQueue.pop_front();
		sendToLayer(CommunicationLayer::Directions_Lower, 
			queueElement.first, queueElement.second);
	}

	if(!queueIsFull())
		unblockUpperQueues();
}

bool CommunicationLayer::sendToQueue(PacketPtr packet)
{
	return sendToQueue(packet, 
		getDefaultLayer(CommunicationLayer::Directions_Lower));
}

bool CommunicationLayer::sendToQueue(PacketPtr packet, 
	t_uint lowerLayerIdx)
{
	bool wasSuccessful = false;
	pair<PacketPtr,t_uint> queueElement = 
		make_pair(packet, lowerLayerIdx);
	if(!queueIsFull()) {
		wasSuccessful = true;
		m_packetQueue.push_back(queueElement);
		sendFromQueue();
	} else {
		// The packet is dropped.
	}

	if(queueIsFull())
		blockUpperQueues();

	return wasSuccessful;
}

bool CommunicationLayer::sendToLayer(
	Directions direction, PacketPtr packet)
{
	return sendToLayer(direction, packet, getDefaultLayer(direction));
}

bool CommunicationLayer::sendToAllLayers(
	Directions direction, PacketPtr packet)
{
	t_uint layersTotal = numberOfLayers(direction);
	bool wasSentToAllLayers = true;
	for(t_uint i = 0; i < layersTotal; i++) {
		wasSentToAllLayers &= sendToLayer(direction, packet, i);
	}
	return wasSentToAllLayers;
}

bool CommunicationLayer::sendToLayer(
	Directions direction, PacketPtr packet, t_uint recvingLayerIdx)
{
	assert(packet != 0);
	assert(recvingLayerIdx < numberOfLayers(direction));
	CommunicationLayerPtr recvingLayer;
	switch(direction) {
	case Directions_Lower:
		recvingLayer = m_lowerLayers[recvingLayerIdx];
		break;
	case Directions_Upper:
		removeLayerData(packet);
		recvingLayer = m_upperLayers[recvingLayerIdx];
		break;
	default:
		assert(false);
	}
	assert(recvingLayer.get() != 0);

	LogStreamManager::instance()->logPktSendItem(getNodeId(),
		getLayerType(), *packet);
	EventPtr recvEvent = LayerRecvEvent::create(direction, 
		packet, recvingLayer, shared_from_this());

	if(direction == Directions_Lower)
		setLowerLayerRecvEventPending(true);

	assert(m_node != 0);
	bool wasScheduled = m_node->scheduleEvent(recvEvent, 
		getLayerDelay(direction));
	return wasScheduled;
}

bool CommunicationLayer::recvFromLayer(Directions direction, 
	PacketPtr packet, CommunicationLayerPtr sendingLayer)
{
	bool layerFound = false;
	t_uint sendingLayerIdx;

	vector<CommunicationLayerPtr>* layerVector = 0;
	switch(direction) {
	case Directions_Lower:
		layerVector = &m_lowerLayers;
		break;
	case Directions_Upper:
		layerVector = &m_upperLayers;
		break;
	default:
		assert(false);
	}
	assert(layerVector != 0);

	t_uint layersTotal = numberOfLayers(direction);
	for(t_uint i = 0; i < layersTotal; i++) {
		if((*layerVector)[i] == sendingLayer) {
			layerFound = true;
			sendingLayerIdx = i;
			break;
		}
	}

	bool wasRecvd = false;
	if(layerFound) {
		LogStreamManager::instance()->logPktRecvItem(getNodeId(),
			getLayerType(), *packet);
		wasRecvd = recvFromLayer(direction, packet, sendingLayerIdx);
	}
	return wasRecvd;
}

bool CommunicationLayer::recvFromLayer(
	Directions direction, PacketPtr packet, t_uint recvLayerIdx)
{
	// By default, we just pass this along to the default
	// next layer or do nothing if none exist
	Directions directionToSend = 
		Directions_Lower;
	switch(direction) {
	case Directions_Lower:
		directionToSend = Directions_Upper;
		break;
	case Directions_Upper:
		directionToSend = Directions_Lower;
		break;
	default:
		assert(false);
	}

	if(numberOfLayers(directionToSend) > 0) {
		sendToLayer(directionToSend, packet);
	}
	return true;

}

NodeId CommunicationLayer::getNodeId() const
{
	NodePtr owner = getNode();
	assert(owner.get() != 0);
	return owner->getNodeId();
}

void CommunicationLayer::removeLayerData(PacketPtr packet) const
{
	switch(getLayerType()) {
	case Types_Application:
		packet->removeData(Packet::DataTypes_Application);
		break;
	case Types_Transport:
		packet->removeData(Packet::DataTypes_Transport);
		break;
	case Types_Network:
		packet->removeData(Packet::DataTypes_Network);
		break;
	case Types_Link:
		packet->removeData(Packet::DataTypes_Link);
		break;
	case Types_Physical:
		packet->removeData(Packet::DataTypes_Physical);
		break;
	default:
		assert(false);
	}
}

