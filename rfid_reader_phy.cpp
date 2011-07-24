
#include "rfid_reader_phy.hpp"
#include "node.hpp"

// These are the 802.11 values from ns-2.
const double RfidReaderPhy::m_DEFAULT_READER_RX_THRESHOLD = 3.652e-10;
const double RfidReaderPhy::m_DEFAULT_READER_CS_THRESHOLD = 1.559e-11;

RfidReaderPhy::RfidReaderPhy(
	NodePtr node, WirelessChannelManagerPtr wirelessChannelManager)
	: PhysicalLayer(node), m_regularChannelIsValid(false), 
	m_allSendersChannelIsValid(false)
{
	m_wirelessChannelManagerPtr = wirelessChannelManager;
	setRxThreshold(m_DEFAULT_READER_RX_THRESHOLD);
	setCsThreshold(m_DEFAULT_READER_CS_THRESHOLD);
}

RfidReaderPhy::~RfidReaderPhy()
{
	// Detatch from any channels to which it is still attached.
	if(m_regularChannelIsValid) {
		m_wirelessChannelManagerPtr->detachAsListener(thisPhysicalLayer(), 
			m_regularChannel);
		m_wirelessChannelManagerPtr->detachAsSender(thisPhysicalLayer(), 
			m_regularChannel);
	}

	if(m_allSendersChannelIsValid) {
		m_wirelessChannelManagerPtr->detachAsSender(thisPhysicalLayer(),
			m_allSendersChannel);
	}
}

void RfidReaderPhy::simulationEndHandler()
{

}

void RfidReaderPhy::resetRegularChannel()
{
	assert(m_wirelessChannelManagerPtr.get() != 0);

	if(m_regularChannelIsValid) {
		m_wirelessChannelManagerPtr->detachAsListener(thisPhysicalLayer(), 
			m_regularChannel);
		m_wirelessChannelManagerPtr->detachAsSender(thisPhysicalLayer(), 
			m_regularChannel);
	}

}

bool RfidReaderPhy::setRegularChannel(t_uint channelId)
{
	assert(m_wirelessChannelManagerPtr.get() != 0);

	// First, we need to see if we can detach from our current
	// channel if it's valid.
	resetRegularChannel();

	bool wasSuccessful = true;
	wasSuccessful &= m_wirelessChannelManagerPtr->attachAsListener(
		thisPhysicalLayer(), channelId);
	wasSuccessful &= m_wirelessChannelManagerPtr->attachAsSender(
		thisPhysicalLayer(), channelId);

	m_regularChannelIsValid = wasSuccessful;
	m_regularChannel = channelId;

	return wasSuccessful;
}

bool RfidReaderPhy::setAllSendersChannel(t_uint channelId)
{
	assert(m_wirelessChannelManagerPtr.get() != 0);

	resetAllSendersChannel();

	bool wasSuccessful = true;
	wasSuccessful &= m_wirelessChannelManagerPtr->attachAsSender(
		thisPhysicalLayer(), channelId);

	m_allSendersChannelIsValid = wasSuccessful;
	m_allSendersChannel = channelId;

	return wasSuccessful;

}

void RfidReaderPhy::resetAllSendersChannel()
{

	assert(m_wirelessChannelManagerPtr.get() != 0);
	if(m_allSendersChannelIsValid) {
		m_wirelessChannelManagerPtr->detachAsSender(thisPhysicalLayer(),
			m_allSendersChannel);
	}

	m_allSendersChannelIsValid = false;
}


