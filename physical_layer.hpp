
#ifndef PHYSICAL_LAYER_H
#define PHYSICAL_LAYER_H

#include <map>
#include <boost/shared_ptr.hpp>

#include "utility.hpp"
#include "communication_layer.hpp"
#include "wireless_comm_signal.hpp"
#include "wireless_channel_manager.hpp"
#include "log_stream_manager.hpp"
#include "node.hpp"
#include "timer.hpp"

typedef map<WirelessCommSignalPtr,double> SignalStrengthMap;

/**
 * The class for the physical layer of a node (i.e., the
 * radio).
 */
class PhysicalLayer : public CommunicationLayer {
friend class SignalRecvEvent;
public:
	/// Smart pointer that clients should use.
	typedef boost::shared_ptr<PhysicalLayer> PhysicalLayerPtr;
	
	/// A destructor
	/// A virtual destructor is recommended since the class
	/// has virtual functions.
	virtual ~PhysicalLayer();

	/**
	 * Gives \c this pointer as a \c PhysicalLayer object.
	 * This is used to deal with \c shared_ptr's, \c this, and
	 * inheritance.
	 * @return a pointer to \c this object.
	 */
	virtual PhysicalLayerPtr thisPhysicalLayer() = 0;

	/**
	 * Add the channel manager.
	 * @param wirelessChannelManager a pointer to the channel 
	 * manager being added.
	 */
	inline void setWirelessChannelManager(
		WirelessChannelManagerPtr wirelessChannelManager);

	/**
	 * Handle the successfully received signal.
	 * The caller of this function should ensure that
	 * a deep copy of the signal is being passed to this object
	 * since the signal may be modified locally and changes should
	 * not affect the signal globally.
	 * @param signal a pointer to a deep copy of the 
	 * signal being received.
	 * @param recvdSignalStrength the strength of the signal
	 * at the receiver.
	 * @return true if the signal was able to be handled
	 * by this object.
	 */
	bool recvPendingSignal(WirelessCommSignalPtr signal,
		double recvdSignalStrength);

	/**
	 * Send a wireless signal on the channel.
	 * @param signal the signal to send.
	 * @return true if the signal was sent successfully.
	 */
	virtual bool sendSignal(WirelessCommSignalPtr signal);

	/**
	 * Determine if this layer is currently transmitting a
	 * signal.
	 * @return true if the layer is transmitting a signal.
	 */
	bool isTransmitting() const;

	/**
	 * Set the delay for how long it takes before a signal is
	 * transmitted on the channel.
	 * @param signalSendingDelay the delay from the time
	 * a signal is sent by this object until it actually
	 * reaches the channel.
	 */
	inline void setSignalSendingDelay(SimTime signalSendingDelay);

	/**
	 * Get the delay for how long it takes to send a signal
	 * on the channel.
	 * @return the delay from the time
	 * a signal is sent by this object until it actually
	 * reaches the channel.
	 */
	inline SimTime getSignalSendingDelay() const;

	/**
	 * Receives a packet from a sending layer.
	 * @param direction the direction the packet was sent.
	 * @param packet a pointer to the received packet.
	 * @param sendingLayerIdx the index of the layer that sent the
	 * packet.
	 * @return true if the specified layer was able to receive
	 * the packet.
	 */
	bool recvFromLayer(CommunicationLayer::Directions direction, 
		PacketPtr packet, t_uint sendingLayerIdx);

	/**
	 * Return the geographic location of this physical layer.
	 * This is needed to compute received signal strength.
	 * @return a pointer to the location of the node that owns
	 * this physical layer.
	 */
	Location getLocation() const;

	/**
	 * Add a signal currently being received and its computed
	 * signal strength.
	 * @param signal a pointer to the signal to add.
	 * @param signalStrength the strength of the signal.
	 * @see removeSignal()
	 * @see getCulmulativeSignalStrength()
	 */
	void addSignal(WirelessCommSignalPtr signal, 
		double signalStrength);

	/**
	 * Remove a signal from the signals currently being received by
	 * this object.
	 * @param signal a pointer to the signal to remove.
	 * @see addSignal()
	 * @see getCulmulativeSignalStrength()
	 */
	void removeSignal(WirelessCommSignalPtr signal);

	/**
	 * Determine if the currently pending signal is still
	 * sufficiently strong to be captured at this object.
	 * @return true if the currently pending signal is now too
	 * weak to be captured to be received or there is no currently
	 * pending signal.
	 */
	bool pendingSignalIsWeak();

	/**
	 * Determine whether of not a given signal is strong enough
	 * to be captured at this object.
	 * @param signalStrength the strength of the signal at this
	 * receiver.  This signal should \e not have been added
	 * (via addSignal) at the time this function is called.
	 * @return true if the signal is strong enough for the
	 * receiver to capture it.
	 * @see addSignal()
	 */
	bool captureSignal(double signalStrength) const;

	/**
	 * Return the total signal strength of all signals
	 * currently being received by this object.
	 * @return the culmulative signal strength of the signals.
	 * @see addSignal()
	 * @see removeSignal()
	 */
	double getCulmulativeSignalStrength() const;

	/**
	 * Return the noise floor for the radio.
	 * @return the noise floor in watts.
	 */
	inline double getNoiseFloor() const;

	/**
	 * Set a signal as the signal which would be received if
	 * it were to end right now.
	 * @param signal a pointer to the signal.
	 * @see getPendingSignal()
	 * @see resetPendingSignal()
	 * @see getPendingSignalStrength()
	 */
	void setPendingSignal(WirelessCommSignalPtr signal);

	/**
	 * Set whether or not the currently pending signal
	 * will have an error when/if received.
	 * @param hasError true if the signal will have an error.
	 */
	inline void setPendingSignalError(bool hasError);

	/**
	 * Get whether or not the currently pending signal
	 * will have an error upon reception.
	 * @return true if the signal will have an error upon
	 * reception.
	 */
	inline bool getPendingSignalError() const;
	
	/**
	 * Get a pointer to the signal which would be received
	 * if it were to end right now.
	 * @return a pointer to the signal.
	 * @see setPendingSignal()
	 * @see resetPendingSignal()
	 * @see getPendingSignalStrength()
	 */
	ConstWirelessCommSignalPtr getPendingSignal() const;

	/**
	 * Get the signal strength of the signal which would be
	 * received if it were to end right now.
	 * @return the signal strength of the signal or zero if
	 * no such signal is being received.
	 * @see setPendingSignal()
	 * @see getPendingSignal()
	 * @see resetPendingSignal()
	 */
	double getPendingSignalStrength();

	/**
	 * Get the SINR of the signal which would be received
	 * if it were to end right now.
	 * @return the SINR of the signal or zero if no such
	 * signal is being received.
	 * @see setPendingSignal()
	 * @see getPendingSignal()
	 * @see resetPendingSignal()
	 */
	double getPendingSignalSinr();

	/**
	 * Reset this object such that there is no currently pending
	 * signal which is strong enough to receive right now.
	 * @see resetRecvSignals()
	 */
	void resetPendingSignal();

	/**
	 * Reset this object such that there are \e no signals
	 * currently pending.
	 * @see resetPendingSignal()
	 */
	void resetRecvSignals();

	/**
	 * Determine whether the channel is carrier sensed busy.
	 * @return true if the channel is carrier sensed busy.
	 */
	bool channelCarrierSensedBusy() const;

	/**
	 * Set the current transmit power for the physical layer.
	 * @param txPower the new power level in watts.
	 * Must be greater than zero.
	 */
	inline void setCurrentTxPower(double txPower);

	/**
	 * Get the current transmit power in watts.
	 * @return the current transmit power in watts.
	 */
	inline double getCurrentTxPower() const;

	/**
	 * Get the maximum transmit power in watts.
	 * @return the maximum transmit power in watts.
	 */
	inline double getMaxTxPower() const;

	/**
	 * Set the power threshold for receiving a packet.
	 * @param rxThreshold the new receive power threshold.
	 */
	inline void setRxThreshold(double rxThreshold);

	/**
	 * Get the power threshold for receiving a packet.
	 * @return the current receive power threshold.
	 */
	inline double getRxThreshold() const;

	/**
	 * Set the power threshold for carrier sensing a channel busy.
	 * @param csThreshold the new carrier sense power threshold.
	 */
	inline void setCsThreshold(double csThreshold);

	/**
	 * Get the power threshold for carrier sensing a channel busy.
	 * @return the current carrier sense power threshold.
	 */
	inline double getCsThreshold() const;

	/**
	 * Set the capture threshold for considering a SINR sufficiently
	 * strong.
	 * @param captureThreshold the new capture threshold.
	 */
	inline void setCaptureThreshold(double captureThreshold);

	/**
	 * Get the capture threshold for considering a SINR sufficiently
	 * strong.
	 * @return the current capture threshold.
	 */
	inline double getCaptureThreshold() const;

	/**
	 * Set the minimum signal strength for considering a signal
	 * stronger than thermal noise.
	 * @param minimumSignalStrength the new minimum signal strength.
	 */
	inline void setMinimumSignalStrength(double minimumSignalStrength);

	/**
	 * Get the minimum signal strength for considering a signal
	 * stronger than thermal noise.
	 * @return the current minimum signal strength.
	 */
	inline double getMinimumSignalStrength() const;

	/**
	 * Set the current data rate in bits per second.
	 * @param dataRate the new data rate.
	 */
	inline void setDataRate(double dataRate);

	/**
	 * Get the current data rate.
	 * @return data rate in bits per second.
	 */
	inline double getDataRate() const;

	/**
	 * Set the current radio bandwidth in Hz.
	 * @param bandwidth the new radio bandwidth.
	 */
	inline void setBandwidth(double bandwidth);

	/**
	 * Get the current radio bandwidth in Hz.
	 * @return the radio bandwidth in Hz.
	 */
	inline double getBandwidth() const;

	/**
	 * Get the gain of the antenna.
	 * @return the gain (unit-less).
	 */
	inline double getGain() const;

	/**
	 * Get the wavelength of the antenna based on the
	 * current bandwidth.
	 * @return the wavelength in meters.
	 */
	inline double getWavelength() const;

	/**
	 * Get the layer type for an object.
	 * @return the type of the layer.
	 */
	inline CommunicationLayer::Types getLayerType() const;

protected:

	/// The channel manager being used by this object.
	/// @see setWirelessChannelManager()
	WirelessChannelManagerPtr m_wirelessChannelManagerPtr;

	/// A constructor
	/// This is protected to ensure that all objects
	/// are created via \c new since we are using smart pointers. 
	PhysicalLayer(NodePtr node);

	/**
	 * This function is called when an error free signal is
	 * received, just before its packet is passed to the upper layer.
	 * This gives subclasses an opportunity to adjust their
	 * state in response to the signal reception if need be.
	 * @param signal the signal that was successfully received.
	 * @param recvdSignalStrength the strength at which the
	 * signal was received.
	 */
	virtual void recvdErrorFreeSignal(WirelessCommSignalPtr signal,
		double recvdSignalStrength);

	/**
	 * Set the maximum transmit power for the physical layer.
	 * If the current transmit power level is less than the new
	 * max value, then it will be set to the new max value.
	 * @param txPower the new maximum power level in watts.
	 * Must be greater than zero.
	 */
	inline void setMaxTxPower(double txPower);

	/**
	 * Schedule the signal to be received by the channel
	 * manager.
	 * @param signal the signal to be scheduled.
	 * @return true if the signal was scheduled.
	 */
	bool scheduleSignal(WirelessCommSignalPtr signal);

private:

	/// The default transmit power.
	static const double m_DEFAULT_TX_POWER;

	/// The default maximum transmit power.
	static const double m_DEFAULT_MAX_TX_POWER;

	/// The default receive power threshold.
	static const double m_DEFAULT_RX_THRESHOLD;

	/// The default carrier sense power threshold.
	static const double m_DEFAULT_CS_THRESHOLD;

	/// The default capture threshold.
	static const double m_DEFAULT_CAPTURE_THRESHOLD;

	/// The default minimum signal strength.
	static const double m_DEFAULT_MINIMUM_SIGNAL_STRENGTH;

	/// The default data rate.
	static const double m_DEFAULT_DATA_RATE;

	/// The default bandwidth.
	static const double m_DEFAULT_BANDWIDTH;

	/// The default radio temperature.
	static const double m_RADIO_TEMPERATURE;

	/// The default radio noise factor.
	static const double m_RADIO_NOISE_FACTOR;

	/// Boltzmann's constant.
	static const double m_BOLTZMANNS_CONSTANT;

	/// Queue length for physical layers.
	static const t_uint m_PHYSICAL_QUEUE_LENGTH;

	/// Determine when debugging info gets printed.
	static const bool m_DEBUG_SIGNAL_CAPTURE = true;
	static const bool m_DEBUG_TRANSMIT_POWER = true;

	/// The transmit power in watts.
	/// @see getCurrentTxPower()
	/// @see setCurrentTxPower()
	double m_currentTxPower;

	/// The maximum transmit power in watts.
	/// @see getMaxTxPower()
	/// @see setMaxTxPower()
	double m_maxTxPower;

	/// The receive power threshold in decibels.
	/// @see getRxThreshold()
	/// @see setRxThreshold()
	double m_rxThreshold;

	/// The carrier sense power threshold in decibels.
	/// @see getCsThreshold()
	/// @see setCsThreshold()
	double m_csThreshold;

	/// The SINR required for a signal to be captured.
	/// @see getCaptureThreshold()
	/// @see setCaptureThreshold()
	double m_captureThreshold;

	/// The minimum signal strength for a signal to
	/// be added in the culmulative signal strength; weaker
	/// signals blend in with the thermal noise.
	/// @see getMinimumSignalStrength()
	/// @see setMinimumSignalStrength()
	double m_minimumSignalStrength;

	/// The radio's current data rate.
	/// @see getDataRate()
	/// @see setDataRate()
	double m_dataRate;

	/// The radio's bandwidth.
	/// @see getBandwidth()
	/// @see setBandwidth()
	double m_bandwidth;

	/// Keeps track of whether the pending packet will
	/// have an error upon reception.
	/// @see setPendingSignalError()
	/// @see getPendingSignalError()
	bool m_pendingRecvSignalError;

	/// Map of signals being received to their corresponding
	/// signal strength.
	/// @see addSignal()
	/// @see removeSignal()
	/// @see getCulmulativeSignalStrength()
	/// @see resetRecvSignals
	SignalStrengthMap m_signalStrengths;

	/// A pointer to the current signal that this object
	/// would receive if it were to finish right now.
	/// @see setPendingSignal()
	/// @see getPendingSignal()
	/// @see resetPendingSignal()
	WirelessCommSignalPtr m_pendingRecvSignal;

	/// The delay for sending a packet to the channel.
	/// @see setSignalSendingDelay()
	/// @see getSignalSendingDelay()
	SimTime m_signalSendingDelay;

	/// A timer that is running whenever a signal is
	/// being transmitting by this object.
	/// @see isTransmitting()
	TimerPtr m_transmittingTimer;

};
typedef boost::shared_ptr<PhysicalLayer> PhysicalLayerPtr;

/////////////////////////////////////////////////
// Inline Functions
/////////////////////////////////////////////////

inline void PhysicalLayer::setWirelessChannelManager(
	WirelessChannelManagerPtr wirelessChannelManager)
{
	m_wirelessChannelManagerPtr = wirelessChannelManager;
}

inline void PhysicalLayer::setCurrentTxPower(double txPower)
{
	assert(txPower > 0.0);
	m_currentTxPower = txPower;
}

inline double PhysicalLayer::getCurrentTxPower() const
{
	return m_currentTxPower;
}

inline void PhysicalLayer::setMaxTxPower(double txPower)
{
	assert(txPower > 0.0);
	m_maxTxPower = txPower;
	m_currentTxPower = min(m_currentTxPower, m_maxTxPower);
}

inline double PhysicalLayer::getMaxTxPower() const
{
	return m_maxTxPower;
}

inline void PhysicalLayer::setRxThreshold(double rxThreshold)
{
	assert(rxThreshold > 0.0);
	m_rxThreshold = rxThreshold;
}

inline double PhysicalLayer::getRxThreshold() const
{
	return m_rxThreshold;
}

inline void PhysicalLayer::setCsThreshold(double csThreshold)
{
	assert(csThreshold > 0.0);
	m_csThreshold = csThreshold;
}

inline double PhysicalLayer::getCsThreshold() const
{
	return m_csThreshold;
}

inline void PhysicalLayer::setCaptureThreshold(double captureThreshold)
{
	assert(captureThreshold > 0.0);
	m_captureThreshold = captureThreshold;
}

inline double PhysicalLayer::getCaptureThreshold() const
{
	return m_captureThreshold;
}

inline void PhysicalLayer::setMinimumSignalStrength(
	double minimumSignalStrength)
{
	assert(minimumSignalStrength > 0.0);
	m_minimumSignalStrength = minimumSignalStrength;
}

inline double PhysicalLayer::getMinimumSignalStrength() const
{
	return m_minimumSignalStrength;
}

inline void PhysicalLayer::setDataRate(double dataRate)
{
	assert(dataRate > 0.0);
	m_dataRate = dataRate;
}

inline double PhysicalLayer::getDataRate() const
{
	return m_dataRate;
}

inline void PhysicalLayer::setBandwidth(double bandwidth)
{
	assert(bandwidth > 0.0);
	m_bandwidth = bandwidth;
}

inline double PhysicalLayer::getBandwidth() const
{
	return m_bandwidth;
}

inline double PhysicalLayer::getNoiseFloor() const
{
	return (m_RADIO_NOISE_FACTOR * m_BOLTZMANNS_CONSTANT * 
		m_RADIO_TEMPERATURE * getBandwidth());
}

inline double PhysicalLayer::getGain() const
{
	// This is just a placeholder function for now.
	return 1.0;
}

inline void PhysicalLayer::setPendingSignalError(bool hasError)
{
	m_pendingRecvSignalError = hasError;
}

inline bool PhysicalLayer::getPendingSignalError() const
{
	return m_pendingRecvSignalError;
}

inline double PhysicalLayer::getWavelength() const
{
	return (SPEED_OF_LIGHT / m_bandwidth);
}

inline void PhysicalLayer::setSignalSendingDelay(
	SimTime signalSendingDelay)
{
	m_signalSendingDelay = signalSendingDelay;
}

inline SimTime PhysicalLayer::getSignalSendingDelay() const
{
	return m_signalSendingDelay;
}

inline CommunicationLayer::Types PhysicalLayer::getLayerType() const
{
	return CommunicationLayer::Types_Physical;
}

/////////////////////////////////////////////////
// Overloaded Operators
/////////////////////////////////////////////////

////////////////////////////////////////////////
// Event Subclasses
/////////////////////////////////////////////////

/**
 * The event for when a signal is transmitted onto the channel.
 */
class SignalRecvEvent : public Event {
public:
	/// Smart pointer that clients should use.
	typedef boost::shared_ptr<SignalRecvEvent> SignalRecvEventPtr;

	/// A constructor.
	SignalRecvEvent(WirelessChannelManagerPtr channelManager,
			PhysicalLayerPtr sender, WirelessCommSignalPtr signal)
			: Event(), m_channelManager(channelManager), 
			m_sender(sender), m_signal(signal)
	{
		assert(m_channelManager.get() != 0);
	}

	void execute()
	{
		assert(m_channelManager.get() != 0);
		LogStreamManager::instance()->logPktSendItem(
			m_sender->getNodeId(), m_sender->getLayerType(), 
			*(m_signal->getPacketPtr()));
		assert(m_sender->m_transmittingTimer.get() != 0);
		assert(!m_sender->m_transmittingTimer->isRunning());
		// Start a timer so we know if the layer is in the process
		// of sending a packet.
		m_sender->m_transmittingTimer->reschedule(m_signal->getDuration());
		m_channelManager->recvSignal(m_sender, m_signal);
	}

private:
	WirelessChannelManagerPtr m_channelManager;
	PhysicalLayerPtr m_sender;
	WirelessCommSignalPtr m_signal;
};
typedef boost::shared_ptr<SignalRecvEvent> SignalRecvEventPtr;

#endif // PHYSICAL_LAYER_H

