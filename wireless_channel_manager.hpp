
#ifndef CHANNEL_MANAGER_H
#define CHANNEL_MANAGER_H

#include <map>
#include <set>
using namespace std;
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/utility.hpp>

#include "utility.hpp"

class WirelessChannel;
typedef boost::shared_ptr<WirelessChannel> WirelessChannelPtr;
class PhysicalLayer;
typedef boost::shared_ptr<PhysicalLayer> PhysicalLayerPtr;
typedef boost::shared_ptr<PhysicalLayer const> ConstPhysicalLayerPtr;
class WirelessCommSignal;
typedef boost::shared_ptr<WirelessCommSignal> WirelessCommSignalPtr;
class Simulator;
typedef Simulator* SimulatorPtr;
class SignalListeners;
typedef boost::shared_ptr<SignalListeners> SignalListenersPtr;
class ListenerSignals;
typedef boost::shared_ptr<ListenerSignals> ListenerSignalsPtr;

typedef map<t_uint,WirelessChannelPtr> ChannelIdMap;
typedef multimap<WirelessChannelPtr,PhysicalLayerPtr> ChannelObserver;
typedef multimap<PhysicalLayerPtr,WirelessChannelPtr> SenderChannel;

/**
 * This class manages which nodes are listening and transmitting
 * on which channels.
 */
class WirelessChannelManager : boost::noncopyable,
	public boost::enable_shared_from_this<WirelessChannelManager> {
public:
	/// Smart pointer that clients should use.
	typedef boost::shared_ptr<WirelessChannelManager> 
		WirelessChannelManagerPtr;

	/**
	 * A factory method to ensure that all objects
	 * are created via \c new since we are using smart pointers. 
	 */
	static inline WirelessChannelManagerPtr create();
	
	/**
	 * Receive a signal from a physical layer that will sent
	 * on all channels to which the sender is attached as a sender.
	 * @param sender a pointer to the physical layer sending the signal.
	 * @param signal the signal being sent.
	 */
	void recvSignal(PhysicalLayerPtr sender, 
		WirelessCommSignalPtr signal);

	/**
	 * When the signal is finished, pass it on to the receiver
	 * if it was the captured signal for that receiver.
	 * @param receiver the physical layer that may potentially
	 * receive the signal.
	 * @param signal the signal that may be received.
	 */
	void passSignalToReceiver(PhysicalLayerPtr receiver, 
		WirelessCommSignalPtr signal) const;

	/**
	 * Add the physical layer as a sender on the given channel.
	 * @param physicalLayer a pointer to the physical layer.
	 * @param channelId the ID of the channel on which the physical layer
	 * will transmit.
	 * @return true if the channel is successfully added on which the
	 * physical layer can transmit.
	 */
	bool attachAsSender(PhysicalLayerPtr physicalLayer, t_uint channelId);

	/**
	 * Add the physical layer as a listener of the given channel.
	 * @param physicalLayer a pointer to the physical layer.
	 * @param channelId the ID of the channel on which the physical layer
	 * will listen.
	 * @return true if the layer is successfully added as a listener
	 * on the channel.
	 */
	bool attachAsListener(PhysicalLayerPtr physicalLayer, t_uint channelId);

	/**
	 * Remove the physical layer as a sender on the given channel.
	 * @param physicalLayer a pointer to the physical layer.
	 * @param channelId the ID of the channel from which the physical layer
	 * will be removed.
	 * @return true if the channel is successfully removed from the
	 * physical layer's list of channels on which it can transmit.
	 */
	bool detachAsSender(PhysicalLayerPtr physicalLayer, t_uint channelId);

	/**
	 * Remove the physical layer as a listener of the given channel.
	 * @param physicalLayer a pointer to the physical layer.
	 * @param channelId the ID of the channel from which the physical layer
	 * will stop listening.
	 * @return true if the layer is successfully removed as a listener
	 * on the channel.
	 */
	bool detachAsListener(PhysicalLayerPtr physicalLayer, t_uint channelId);

	/**
	 * Add the given channel to this manager with the specified ID.
	 * If a different already exists with the specified ID, it
	 * will be replaced with the newly specified channel.
	 * @param channelId the ID to identify the channel.
	 * @param channel a pointer to the channel to be added.
	 */
	void addChannel(t_uint channelId, WirelessChannelPtr channel);

	/**
	 * Remove the channel with the given ID from this manager.
	 * @param channelId the ID of the channel to be removed.
	 * @return true if the channel was found and successfully removed.
	 */
	bool removeChannel(t_uint channelId);

protected:

	/// A constructor
	WirelessChannelManager();

private:

	/// Determine when debugging info is printed.
	static const bool m_DEBUG_SIGNAL_STRENGTH = false;

	/// A mapping of IDs to channel pointers.
	ChannelIdMap m_channels;

	/// A mapping of channels to physical layers which observe it.
	ChannelObserver m_listeners;

	/// A mapping of physical layers to the channels on which
	/// their packets are transmitted.
	SenderChannel m_senders;

	/**
	 * Function to control which receivers should hear
	 * a copy of the signal when sent on the given channel.
	 * @param sender the sender of the signal.
	 * @param signal the signal being sent.
	 * @param channel the channel on which the signal
	 * is being sent.
	 */
	void sendSignalOnChannel(ConstPhysicalLayerPtr sender, 
		WirelessCommSignalPtr signal, WirelessChannelPtr channel);

	/**
	 * Does a revese look-up on the channel pointer to find
	 * its ID (since our map is keyed by ID).
	 * Assertions ensure that the channel is found exactly
	 * once in the channel ID map.
	 * @param channel a pointer to the channel being looked up.
	 * @return the channel ID of the channel.
	 */
	t_uint getChannelId(WirelessChannelPtr channel) const;

};
typedef boost::shared_ptr<WirelessChannelManager> 
	WirelessChannelManagerPtr;
typedef boost::shared_ptr<WirelessChannelManager const> 
	ConstWirelessChannelManagerPtr;

/////////////////////////////////////////////////
// Inline Functions
/////////////////////////////////////////////////

inline WirelessChannelManagerPtr WirelessChannelManager::create()
{
	WirelessChannelManagerPtr p(new WirelessChannelManager());
	return p;
}

/////////////////////////////////////////////////
// Overloaded Operators
/////////////////////////////////////////////////

#endif // CHANNEL_MANAGER_H

