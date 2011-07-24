
#include "physical_layer.hpp"
#include "node.hpp"
#include "packet.hpp"

// These values are from a variety of sources, most
// notably Alien and EPCglobal.

// Alien ALR-9800: 30 dBm RF Power == 1 Watt
const double PhysicalLayer::m_DEFAULT_TX_POWER = 1.0;
const double PhysicalLayer::m_DEFAULT_MAX_TX_POWER = 1.0;
// This is based on published reports that the received
// power should be 100 to 400 microwatts for commercial tags.
// www.enigmatic-consulting.com/Communications_articles/
// RFID/Link_budgets.html
const double PhysicalLayer::m_DEFAULT_RX_THRESHOLD = 100e-6;
// In ns-2 the default ratio between CS and RX is about a
// factor of 23.
const double PhysicalLayer::m_DEFAULT_CS_THRESHOLD = 5e-6;
// This is from ns-2
const double PhysicalLayer::m_DEFAULT_CAPTURE_THRESHOLD = 10;
// This is -111 dBm, which is the thermal noise floor for
// GPS signals and the default for Qualnet
const double PhysicalLayer::m_DEFAULT_MINIMUM_SIGNAL_STRENGTH = 7.94e-12;
// EPCglobal: 26.7 to 128 kbps for reader
// 40 to 640 kbps or 5 to 320 kbps for tag depending on modulation
const double PhysicalLayer::m_DEFAULT_DATA_RATE = 128e3;
// EPCglobal: 860 to 960 MHz
const double PhysicalLayer::m_DEFAULT_BANDWIDTH = 960e6;

// Temperature is in Kelvins
const double PhysicalLayer::m_RADIO_TEMPERATURE = 290;
const double PhysicalLayer::m_RADIO_NOISE_FACTOR = 10;
const double PhysicalLayer::m_BOLTZMANNS_CONSTANT = 1.3806503e-23;

const t_uint PhysicalLayer::m_PHYSICAL_QUEUE_LENGTH = 1;

PhysicalLayer::PhysicalLayer(NodePtr node)
	: CommunicationLayer(node),
	m_currentTxPower(m_DEFAULT_TX_POWER),
	m_maxTxPower(m_DEFAULT_MAX_TX_POWER),
	m_rxThreshold(m_DEFAULT_RX_THRESHOLD),
	m_csThreshold(m_DEFAULT_CS_THRESHOLD),
	m_captureThreshold(m_DEFAULT_CAPTURE_THRESHOLD),
	m_minimumSignalStrength(m_DEFAULT_MINIMUM_SIGNAL_STRENGTH),
	m_dataRate(m_DEFAULT_DATA_RATE),
	m_bandwidth(m_DEFAULT_BANDWIDTH),
	m_pendingRecvSignalError(false),
	m_signalSendingDelay(0.0)
{
	assert(m_dataRate > 0.0);
	assert(m_bandwidth > 0.0);
	assert(m_currentTxPower > 0.0);
	assert(m_maxTxPower >= m_currentTxPower);
	setMaxQueueLength(m_PHYSICAL_QUEUE_LENGTH);
	DummyEventPtr dummyEvent = DummyEvent::create();
	m_transmittingTimer = Timer::create(getNode(), dummyEvent);
}

PhysicalLayer::~PhysicalLayer()
{

}

bool PhysicalLayer::recvPendingSignal(WirelessCommSignalPtr signal, 
	double recvdSignalStrength)
{
	// By default, just pass the signals packet on to the
	// default upper layer if it's not in error.
	bool wasSuccessful = true;
	PacketPtr packet = signal->getPacketPtr();
	packet->setHasError(m_pendingRecvSignalError);
	LogStreamManager::instance()->logPktRecvItem(getNodeId(),
		getLayerType(), *packet);
	if(!packet->getHasError()) {
		recvdErrorFreeSignal(signal, recvdSignalStrength);
		wasSuccessful = sendToLayer(
			CommunicationLayer::Directions_Upper, packet);
	}

	return wasSuccessful;
}

void PhysicalLayer::recvdErrorFreeSignal(WirelessCommSignalPtr signal,
	double recvdSignalStrength)
{

}

bool PhysicalLayer::recvFromLayer(
	CommunicationLayer::Directions direction,
	PacketPtr packet, t_uint sendingLayerIdx)
{
	// The physical layer should not have a lower communication
	// layer from which to receive.  Its lower layer is a channel
	// which is not a CommunicationLayer subclass.
	assert(direction == CommunicationLayer::Directions_Upper);
	assert(getNode().get() != 0);
	Location myLocation = getNode()->getLocation();
	packet->setDataRate(getDataRate());

	double signalTxPower = m_currentTxPower;
	if(packet->getDoMaxTxPower()) {
		signalTxPower = m_maxTxPower;
	} else if(packet->getTxPower() > 0.0) {
		signalTxPower = packet->getTxPower();
	}

	if(m_DEBUG_TRANSMIT_POWER) {
		ostringstream debugStream;
		debugStream << __PRETTY_FUNCTION__ << ": txPower: " <<
			signalTxPower;
		LogStreamManager::instance()->logDebugItem(debugStream.str());
	}

	WirelessCommSignalPtr signal = 
		WirelessCommSignal::create(myLocation, 
		powerToDecibels(signalTxPower), 
		getWavelength(), getGain(), packet);
	return sendSignal(signal);
}

bool PhysicalLayer::isTransmitting() const
{
	assert(m_transmittingTimer.get() != 0);
	return m_transmittingTimer->isRunning();
}

bool PhysicalLayer::sendSignal(WirelessCommSignalPtr signal)
{
	return scheduleSignal(signal);
}

bool PhysicalLayer::scheduleSignal(WirelessCommSignalPtr signal)
{
	EventPtr recvEvent(new SignalRecvEvent(m_wirelessChannelManagerPtr,
		thisPhysicalLayer(), signal));
	assert(getNode().get() != 0);
	bool wasScheduled = getNode()->scheduleEvent(recvEvent, 
		getSignalSendingDelay());
	return wasScheduled;
}

void PhysicalLayer::addSignal(WirelessCommSignalPtr signal, 
	double signalStrength)
{
	if(signalStrength > getMinimumSignalStrength()) {
		assert(signal.get() != 0);
		SignalStrengthMap::const_iterator mapIterator = 
			m_signalStrengths.find(signal);
		
		bool wasFound = (mapIterator != m_signalStrengths.end());
		if(wasFound) {
			// We replace the existing signal strength with a new
			// signal strength.
			m_signalStrengths.erase(signal);
		}
	
		m_signalStrengths.insert(make_pair(signal, signalStrength));
	}
}

void PhysicalLayer::removeSignal(WirelessCommSignalPtr signal)
{
	assert(signal.get() != 0);
	m_signalStrengths.erase(signal);
	//int numErased = m_signalStrengths.erase(signal);
	//bool wasRemoved = (numErased > 0);
}

double PhysicalLayer::getPendingSignalSinr()
{
	// This is similar to the captureSignal function except
	// that the signal strength in consideration, that of the
	// pending signal, is already included in the culmulative
	// signal strength.  So, we need to subtract that signal
	// strength out of the culmulative signal strength and
	// use it for the numerator.  
	double sinr = 0.0;
	if(m_pendingRecvSignal.get() != 0) {
		double pendingSignalStrength = getPendingSignalStrength();
		double interferenceFloor =
			(getCulmulativeSignalStrength() - pendingSignalStrength) + 
			getNoiseFloor();
		sinr = pendingSignalStrength / interferenceFloor;
	}
	return sinr;
}

bool PhysicalLayer::pendingSignalIsWeak()
{
	// Note that this function returns
	// true if the signal is too weak, whereas captureSignal
	// returns true if the signal is sufficiently strong.
	bool isWeak = (getPendingSignalStrength() <= getRxThreshold());
	if(m_pendingRecvSignal.get() != 0) {
		isWeak |= (getPendingSignalSinr() <= getCaptureThreshold());
	}
	return isWeak;
}

bool PhysicalLayer::captureSignal(double signalStrength) const
{
	bool isSufficientlyStrong = (signalStrength > getRxThreshold());

	if(isSufficientlyStrong) {
		double interferenceFloor = getCulmulativeSignalStrength() + 
			getNoiseFloor();
		double sinr = signalStrength / interferenceFloor;
		/// \todo When a full BER model is implemented, this
		/// signal can just be captured if its signal stength
		/// is stronger than the currently pending signal signal
		/// strength.  The BER model will take care of whether or
		/// not the packet is received in error based on the SINR
		/// and rate.
		isSufficientlyStrong &= (sinr > getCaptureThreshold());
		if(m_DEBUG_SIGNAL_CAPTURE) {
			ostringstream debugStream;
			debugStream << __PRETTY_FUNCTION__ << 
				": NodeId: " << getNodeId() << 
				" SINR: " << sinr <<
				" SS: " << signalStrength << 
				" RxThreshold: " << getRxThreshold() << 
				" CsThreshold: " << getCsThreshold() << 
				" captureThresh: " << getCaptureThreshold() << 
				" INTR: " << interferenceFloor <<
				" culmulative: " << getCulmulativeSignalStrength() <<
				" noise: " << getNoiseFloor();
			LogStreamManager::instance()->logDebugItem(debugStream.str());
		}
	}
	return isSufficientlyStrong;
}

double PhysicalLayer::getCulmulativeSignalStrength() const
{
	double culmulativeStrength = 0;
	SignalStrengthMap::const_iterator mapIterator;
	for(mapIterator = m_signalStrengths.begin();
			mapIterator != m_signalStrengths.end(); ++mapIterator) {
		culmulativeStrength += mapIterator->second;
	}
	return culmulativeStrength;
}

void PhysicalLayer::setPendingSignal(WirelessCommSignalPtr signal)
{
	assert(signal.get() != 0);
	m_pendingRecvSignal = signal;
}

ConstWirelessCommSignalPtr PhysicalLayer::getPendingSignal() const
{
	return m_pendingRecvSignal;
}

double PhysicalLayer::getPendingSignalStrength()
{
	double signalStrength = 0.0;
	if(m_pendingRecvSignal.get() != 0) {
		SignalStrengthMap::iterator mapIterator = 
			m_signalStrengths.find(m_pendingRecvSignal);
	
		bool wasFound = (mapIterator != m_signalStrengths.end());
		assert(wasFound);
		signalStrength = mapIterator->second;
	}

	return signalStrength;
}

void PhysicalLayer::resetPendingSignal()
{
	m_pendingRecvSignalError = false;
	m_pendingRecvSignal.reset();
}

void PhysicalLayer::resetRecvSignals()
{
	resetPendingSignal();
	m_signalStrengths.empty();
}

bool PhysicalLayer::channelCarrierSensedBusy() const
{
	bool isBusy = (getCulmulativeSignalStrength() > getCsThreshold());
	return isBusy;
}

Location PhysicalLayer::getLocation() const
{
	return getNode()->getLocation();
}

