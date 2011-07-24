
#include "rfid_tag_phy.hpp"
#include "node.hpp"

RfidTagPhy::RfidTagPhy(
	NodePtr node, WirelessChannelManagerPtr wirelessChannelManager)
	: PhysicalLayer(node), m_sendingChannelIsValid(false), 
	m_allListenersChannelIsValid(false)
{
	m_wirelessChannelManagerPtr = wirelessChannelManager;
}

RfidTagPhy::~RfidTagPhy()
{
	// Detatch from any channels to which it is still attached.
	if(m_sendingChannelIsValid) {
		m_wirelessChannelManagerPtr->detachAsSender(thisPhysicalLayer(), 
			m_sendingChannel);
	}

	if(m_allListenersChannelIsValid) {
		m_wirelessChannelManagerPtr->detachAsListener(thisPhysicalLayer(),
			m_allListenersChannel);
	}
}

void RfidTagPhy::simulationEndHandler()
{

}

void RfidTagPhy::resetSendingChannel()
{
	assert(m_wirelessChannelManagerPtr.get() != 0);

	if(m_sendingChannelIsValid) {
		m_wirelessChannelManagerPtr->detachAsSender(thisPhysicalLayer(), 
			m_sendingChannel);
	}

}

bool RfidTagPhy::setSendingChannel(t_uint channelId)
{
	assert(m_wirelessChannelManagerPtr.get() != 0);

	// First, we need to see if we can detach from our current
	// channel if it's valid.
	resetSendingChannel();

	bool wasSuccessful = true;
	wasSuccessful &= m_wirelessChannelManagerPtr->attachAsSender(
		thisPhysicalLayer(), channelId);

	m_sendingChannelIsValid = wasSuccessful;
	m_sendingChannel = channelId;

	return wasSuccessful;
}

bool RfidTagPhy::setAllListenersChannel(t_uint channelId)
{
	assert(m_wirelessChannelManagerPtr.get() != 0);

	resetAllListenersChannel();

	bool wasSuccessful = true;
	wasSuccessful &= m_wirelessChannelManagerPtr->attachAsListener(
		thisPhysicalLayer(), channelId);

	m_allListenersChannelIsValid = wasSuccessful;
	m_allListenersChannel = channelId;

	return wasSuccessful;

}

void RfidTagPhy::resetAllListenersChannel()
{

	assert(m_wirelessChannelManagerPtr.get() != 0);
	if(m_allListenersChannelIsValid) {
		m_wirelessChannelManagerPtr->detachAsListener(thisPhysicalLayer(),
			m_allListenersChannel);
	}

	m_allListenersChannelIsValid = false;
}

void RfidTagPhy::recvdErrorFreeSignal(WirelessCommSignalPtr signal,
	double recvdSignalStrength)
{
	// The tag should lock onto the channel on which the
	// packet was received.
	setSendingChannel(signal->getChannelId());
	setCurrentTxPower(recvdSignalStrength);
}


