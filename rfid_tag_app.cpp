
#include "rfid_tag_app.hpp"
#include "rfid_reader_app.hpp"
#include "node.hpp"

////////////////////////////////////////////////
// RfidTagApp functions
/////////////////////////////////////////////////

RfidTagApp::RfidTagApp(NodePtr node)
	: ApplicationLayer(node), m_replyToReads(true)
{

}

void RfidTagApp::simulationEndHandler()
{

}

void RfidTagApp::startHandler()
{

}

void RfidTagApp::stopHandler()
{

}

bool RfidTagApp::getReplyToReads() const
{
	return m_replyToReads;
}

void RfidTagApp::setReplyToReads(bool replyToReads)
{
	m_replyToReads = replyToReads;
}

bool RfidTagApp::handleRecvdPacket(PacketPtr packet, t_uint recvLayerIdx)
{
	if(!m_isRunning)
		return false;

	RfidReaderAppDataPtr readerData = 
		boost::dynamic_pointer_cast<RfidReaderAppData>
		(packet->getData(Packet::DataTypes_Application));

	bool wasSuccessful = false;
	// If the shared_ptr is empty, then either the cast failed
	// (which could be the result of the packet having no
	// application data).
	if(readerData.get() != 0) {
		switch(readerData->getType()) {
		case RfidReaderAppData::Types_Read:
			if(m_replyToReads) {
				sendIdPacket(readerData->getReaderId());
			}
			wasSuccessful = true;
			break;
		case RfidReaderAppData::Types_Reset:
			m_replyToReads = true;
			wasSuccessful = true;
			break;
		default:
			wasSuccessful = false;
		}
	}

	return wasSuccessful;
}

void RfidTagApp::sendIdPacket(const NodeId& destination)
{
	PacketPtr packetToSend = Packet::create();
	packetToSend->setDestination(destination);

	RfidTagAppDataPtr appData = RfidTagAppData::create();
	appData->setTagId(getNodeId());
	packetToSend->addData(Packet::DataTypes_Application, *appData);

	//sendToLayer(CommunicationLayer::Directions_Lower, packetToSend);
	sendToQueue(packetToSend);
}

////////////////////////////////////////////////
// RfidTagAppData functions
/////////////////////////////////////////////////

RfidTagAppData::RfidTagAppData()
	: m_type(RfidTagAppData::Types_NoType)
{
	fill(m_nodeId, &m_nodeId[m_nodeIdBytes], 0);
}

RfidTagAppData::RfidTagAppData(const RfidTagAppData& rhs)
	: PacketData(rhs), m_type(rhs.m_type)
{
	copy(rhs.m_nodeId, &rhs.m_nodeId[m_nodeIdBytes], m_nodeId);
}

PacketDataPtr RfidTagAppData::clone() const
{
	 PacketDataPtr p(new RfidTagAppData(*this));
	 return p;
}

void RfidTagAppData::setTagId(const NodeId& nodeId)
{
	nodeId.writeToByteArray(m_nodeId, m_nodeIdBytes);
}

NodeId RfidTagAppData::getTagId() const
{
	NodeId nodeId(m_nodeId, m_nodeIdBytes);
	return nodeId;
}


