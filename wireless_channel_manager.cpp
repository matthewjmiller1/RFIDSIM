
#include "wireless_channel_manager.hpp"
#include "wireless_channel.hpp"
#include "physical_layer.hpp"
#include "event.hpp"
#include "simulator.hpp"

/////////////////////////////////////////////////
// Event Subclasses
/////////////////////////////////////////////////

/**
 * The event for when a signal ends (e.g., after accounting for
 * latency and transmision delay) after being placed on the channel.
 * The packet can the be passed to the receiver.
 */
class SignalEndEvent : public Event {
public:

	/// A constructor
	SignalEndEvent(ConstWirelessChannelManagerPtr channelManager, 
		PhysicalLayerPtr receiver, WirelessCommSignalPtr signal)
		: Event(), m_channelManager(channelManager), 
		m_receiver(receiver), m_signal(signal)
	{
		assert(m_channelManager.get() != 0);
		assert(m_receiver != 0);
		assert(m_signal.get() != 0);
	}

	void execute()
	{
		m_channelManager->passSignalToReceiver(m_receiver, m_signal);
	}

private:
	ConstWirelessChannelManagerPtr m_channelManager;
	PhysicalLayerPtr m_receiver;
	WirelessCommSignalPtr m_signal;
};

/// \var typedef boost::shared_ptr<SignalEndEvent> SignalEndEventPtr
/// \brief Smart pointer that clients should use.
typedef boost::shared_ptr<SignalEndEvent> SignalEndEventPtr;


/////////////////////////////////////////////////
// Wireless Channel Manager Implementation
/////////////////////////////////////////////////

WirelessChannelManager::WirelessChannelManager()
{

}

void WirelessChannelManager::recvSignal(PhysicalLayerPtr sender,
	WirelessCommSignalPtr signal)
{
	assert(sender.get() != 0);
	assert(signal.get() != 0);

	typedef SenderChannel::const_iterator senderIterator;
	pair<senderIterator,senderIterator> iteratorRange = 
		m_senders.equal_range(sender);

	for(senderIterator i = iteratorRange.first;
			i != iteratorRange.second; ++i) {
		WirelessChannelPtr channel = i->second;
		sendSignalOnChannel(sender, signal, channel);
	}
}

t_uint WirelessChannelManager::getChannelId(
	WirelessChannelPtr channel) const
{
	t_uint channelId = 0;
	bool wasFound = false;
	ChannelIdMap::const_iterator p;
	for(p = m_channels.begin(); p != m_channels.end(); ++p) {
		if(p->second == channel) {
			// Make sure we only find this channel once.
			assert(!wasFound);
			wasFound = true;
			channelId = p->first;
		}
	}
	assert(wasFound);

	return channelId;
}

void WirelessChannelManager::sendSignalOnChannel(
	ConstPhysicalLayerPtr sender, WirelessCommSignalPtr signal, 
	WirelessChannelPtr channel)
{
	assert(sender != 0);
	assert(signal.get() != 0);
	assert(channel.get() != 0);

	typedef ChannelObserver::const_iterator observerIterator;
	pair<observerIterator,observerIterator> iteratorRange = 
		m_listeners.equal_range(channel);

	SimTime signalEndTime = signal->getDuration();

	// Let receivers know on which channel the signal is received.
	signal->setChannelId(getChannelId(channel));

	for(observerIterator i = iteratorRange.first;
			i != iteratorRange.second; ++i) {

		PhysicalLayerPtr listener = i->second;

		// Make sure that we don't calculate the receiving power
		// for the sender.
		if(listener != sender) {
			// Calculate the SINR of the signal for each listener
			// based on its location.  Add this value to a hash table.
			double signalStrength = 
				channel->getRecvdStrength(*signal, *listener);

			if(m_DEBUG_SIGNAL_STRENGTH) {
				ostringstream debugStream;
				debugStream << "listener: " << listener->getNodeId() <<
					", ss: " << signalStrength << " RXThresh: " << 
					listener->getRxThreshold();
				LogStreamManager::instance()->logDebugItem(debugStream.str());
			}

			// If the signal strength is sufficiently strong to 
			// receive the signal, set it as the packet which will
			// be received by this physical layer.
			if(listener->captureSignal(signalStrength)) {
				listener->setPendingSignal(signal);
			} 

			// Add the signal to the existing culmulative signal
			// strength being received.
			listener->addSignal(signal, signalStrength);

			// We need to determine if the currently pending
			// signal (if one exists) is still sufficiently strong
			// after this new signal has been added.  If the
			// currently received signal is the pending
			// signal, this check is unnecessary since it was
			// essentially performed via the captureSignal
			// call above.
			if(signal != listener->getPendingSignal() &&
					listener->pendingSignalIsWeak()) {
				listener->resetPendingSignal();
			}

			// This is based on the QualNet model for bit error
			// computations.  When the signal is received or
			// any time the interference changes, compute the
			// probability of packet error for that interference 
			// level.  Thus, if p_i is the probability that the packet
			// is not in error for the i-th computation on the signal,
			// the PER for the signal is:
			// PER = 1 - (p_1 * p_2 * ... * p_n)
			// where n is the number of interference changes during
			// the packet.
			if(listener->getPendingSignal().get() != 0 &&
					!listener->getPendingSignalError()) {
				listener->setPendingSignalError(
					channel->signalHasError(
						listener->getPendingSignalSinr(), 
						*listener->getPendingSignal()
					)
				);
			}

			// Schedule an event for when this signal will finish.
			SignalEndEventPtr signalEnd(
				new SignalEndEvent(shared_from_this(), listener, signal));
			SimTime recvTime = signalEndTime + 
				channel->propagationDelay(*sender, *listener);
			Simulator::instance()->scheduleEvent(signalEnd, recvTime);

		}
	}
}

void WirelessChannelManager::passSignalToReceiver(
	PhysicalLayerPtr receiver, WirelessCommSignalPtr signal) const
{
	assert(receiver != 0);

	// If signal == to receiver's strongest signal, then pass it up.
	if(signal == receiver->getPendingSignal()) {
		WirelessCommSignalPtr signalDeepCopy = 
			WirelessCommSignal::create(*signal);
		double recvdSignalStrength = receiver->getPendingSignalStrength();
		receiver->recvPendingSignal(signalDeepCopy, recvdSignalStrength);
		receiver->resetPendingSignal();
	}

	// Remove the signal from the receier's culmulative interference.
	receiver->removeSignal(signal);

}

bool WirelessChannelManager::attachAsSender(PhysicalLayerPtr physicalLayer,
	t_uint channelId)
{
	assert(physicalLayer != 0);
	ChannelIdMap::iterator channelIterator = m_channels.find(channelId);

	bool wasSuccessful = false;
	bool channelFound = (channelIterator != m_channels.end());
	if(channelFound) {
		WirelessChannelPtr channel = channelIterator->second;
		assert(channel.get() != 0);
		m_senders.insert(make_pair(physicalLayer, channel));
		wasSuccessful = true;
	}

	return wasSuccessful;
}

bool WirelessChannelManager::detachAsSender(PhysicalLayerPtr physicalLayer,
	t_uint channelId)
{
	assert(physicalLayer != 0);
	ChannelIdMap::iterator channelIterator = m_channels.find(channelId);
	bool channelFound = (channelIterator != m_channels.end());

	bool wasSuccessful = false;

	if(channelFound) {
		WirelessChannelPtr channel = channelIterator->second;
		typedef SenderChannel::iterator senderIterator;
		pair<senderIterator,senderIterator> iteratorRange = 
			m_senders.equal_range(physicalLayer);

		for(senderIterator i = iteratorRange.first;
				i != iteratorRange.second; ++i) {
			if(i->second == channel) {
				m_senders.erase(i);
				wasSuccessful = true;
				break;
			}
		}
	}

	return wasSuccessful;
}

bool WirelessChannelManager::attachAsListener(PhysicalLayerPtr physicalLayer,
	t_uint channelId)
{
	assert(physicalLayer != 0);
	ChannelIdMap::iterator channelIterator = m_channels.find(channelId);

	bool wasSuccessful = false;
	bool channelFound = (channelIterator != m_channels.end());
	if(channelFound) {
		WirelessChannelPtr channel = channelIterator->second;
		assert(channel.get() != 0);
		m_listeners.insert(make_pair(channel, physicalLayer));
		wasSuccessful = true;
	}

	return wasSuccessful;
}

bool WirelessChannelManager::detachAsListener(PhysicalLayerPtr physicalLayer,
	t_uint channelId)
{
	assert(physicalLayer != 0);
	ChannelIdMap::iterator channelIterator = m_channels.find(channelId);
	bool channelFound = (channelIterator != m_channels.end());

	bool wasSuccessful = false;

	if(channelFound) {
		WirelessChannelPtr channel = channelIterator->second;
		typedef ChannelObserver::iterator observerIterator;
		pair<observerIterator,observerIterator> iteratorRange = 
			m_listeners.equal_range(channel);

		for(observerIterator i = iteratorRange.first;
				i != iteratorRange.second; ++i) {
			if(i->second == physicalLayer) {
				m_listeners.erase(i);
				wasSuccessful = true;
				break;
			}
		}
	}

	return wasSuccessful;
}

void WirelessChannelManager::addChannel(t_uint channelId, 
	WirelessChannelPtr channel)
{
	assert(channel.get() != 0);
	m_channels[channelId] = channel;
}

bool WirelessChannelManager::removeChannel(t_uint channelId)
{
	int numChannelsRemoved = m_channels.erase(channelId);
	bool wasSuccessful = (numChannelsRemoved > 0);
	return wasSuccessful;
}


