
#ifndef COMMUNICATION_LAYER_H
#define COMMUNICATION_LAYER_H

#include <vector>
#include <list>
#include <iostream>
using namespace std;
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/utility.hpp>

#include "event.hpp"
#include "utility.hpp"
#include "sim_time.hpp"
#include "simulation_end_listener.hpp"

class Node;
typedef boost::shared_ptr<Node> NodePtr;
typedef boost::shared_ptr<Node const> ConstNodePtr;
class NodeId;
class Packet;
typedef boost::shared_ptr<Packet> PacketPtr;

/**
 * The communication layer class which is a superclass of
 * classes implemented layers in the network stack.
 * This is the superclass for each of the layers in the network
 * stack in a node.  A communiation layer follows the OSI model where
 * layers communicate with layers adjacent to it both above and below.
 * Our class supports multiple adjacent layers.  This allows, for example,
 * a MAC layer to have multiple interfaces (PHY layers) with which
 * it can communicate or multiple applications to share lower layers.
 */
class CommunicationLayer : 
	public SimulationEndListener,
	boost::noncopyable, 
	public boost::enable_shared_from_this<CommunicationLayer> {
	// Only node can call setNode()
	friend class Node;
	friend class LayerRecvEvent;
public:
	/// Smart pointer that clients should use.
	typedef boost::shared_ptr<CommunicationLayer> CommunicationLayerPtr;
	/// Smart pointer that clients should use.
	typedef boost::shared_ptr<SimulationEndListener> 
		SimulationEndListenerPtr;

	/**
	 * Directions enum.
	 * Used to specify which direction an operation should
	 * operate on.
	 */
	enum Directions { 
		Directions_Lower, /**< enum value Directions_Lower. */
		Directions_Upper  /**< enum value Direction_Upper. */
	};

	/**
	 * Types enum.
	 * Used to classify the type of a CommunicationLayer object.
	 */
	enum Types {
		Types_Physical, /**< enum value Types_Physical. */
		Types_Link, /**< enum value Types_Link. */
		Types_Network, /**< enum value Types_Network. */
		Types_Transport, /**< enum value Types_Transport. */
		Types_Application /**< enum value Types_Application. */
	};

	/// A destructor.
	/// A virtual destructor is recommended since the class
	/// has virtual functions.
	virtual ~CommunicationLayer();

	/**
	 * Gives \c this pointer as a \c SimulationEndListener object.
	 * This is used to deal with \c shared_ptr's, \c this, and
	 * inheritance.
	 * @return a pointer to \c this object.
	 */
	virtual SimulationEndListenerPtr thisSimulationEndListener() = 0;

	/**
	 * Add a layer below this layer.
	 * This object will be inserted as the upper layer for
	 * \c layerToInsert.
	 * The layer being inserted must be owned by the same
	 * node as this object.
	 * @param layerToInsert a pointer to the lower layer.
	 */
	inline void insertLowerLayer(CommunicationLayerPtr layerToInsert);

	/**
	 * The number of layers available in the given direction.
	 * @param direction the direction being queried.
	 * @return the number of layers.
	 */
	inline t_uint numberOfLayers(Directions direction) const;

	/**
	 * Set the layer being used as the default for communication.
	 * @param direction the direction of the action.
	 * @param newDefaultIdx the index of the layer to be used as
	 * the default.
	 * @return true if the desired layer was set as the default.
	 */
	inline bool setDefaultLayer(Directions direction, t_uint newDefaultIdx);

	/**
	 * Get the index of the layer being used as the default.
	 * @param direction the direction being queried.
	 * @return the index of the default communication layer.
	 */
	inline t_uint getDefaultLayer(Directions direction) const;

	/**
	 * Set the time delay before a sent packet is received by the
	 * next layer.
	 * @param direction the direction of the action.
	 * @param delay the time delay.
	 */
	inline void setLayerDelay(Directions direction, const SimTime& delay);

	/**
	 * Get the time delay before a sent packet is received by the next
	 * layer.
	 * @param direction the direction being queried.
	 * @return the time delay.
	 */
	inline SimTime getLayerDelay(Directions direction) const;

	/**
	 * Send a packet to the default lower layer using
	 * a queue.
	 * @param packet a pointer to the packet being sent.
	 * @return true if the packet was successfully queued.
	 */
	bool sendToQueue(PacketPtr packet);

	/**
	 * Send a packet to the specified lower layer using a queue.
	 * @param packet a pointer to the packet being sent.
	 * @param lowerLayerIdx the index of the lower layer to
	 * which we are sending.
	 * @return true if the packet was successfully queued.
	 */
	bool sendToQueue(PacketPtr packet, t_uint lowerLayerIdx);

	/**
	 * Unblock this layer's packet queue so that it
	 * can resume sending.
	 */
	inline void unblockQueue();

	/**
	 * Block this layer's packet queue so that it can
	 * no longer pass packet to lower layers.
	 */
	inline void blockQueue();

	/**
	 * Determine whether of not the queue is currently blocked.
	 * @return true if the queue is blocked.
	 */
	inline bool getQueueIsBlocked() const;

	/**
	 * Set the maximum packet queue size.
	 * @param maxQueueLength the maximimum number of
	 * packets that the queue can hold.
	 */
	inline void setMaxQueueLength(t_uint maxQueueLength);

	/**
	 * Get the maximum packet queue size.
	 * @return the maximum number of packet that the queue
	 * can hold.
	 */
	inline t_uint getMaxQueueLength() const;

	/**
	 * Send a packet to the default next layer.
	 * Packets send to upper layers will have their header
	 * data removed.
	 * @param direction the direction of the action.
	 * @param packet a pointer to the packet being sent.
	 * @return true if the packet was successfully scheduled to
	 * be sent to the next layer.
	 */
	bool sendToLayer(Directions direction, PacketPtr packet);

	/**
	 * Send a packet to the specified next layer.
	 * Packets send to upper layers will have their header
	 * data removed.
	 * @param direction the direction of the action.
	 * @param packet a pointer to the packet being sent.
	 * @param recvingLayerIdx the index of the layer to which the
	 * packet is being sent.
	 * @return true if the packet was successfully scheduled to
	 * be sent to the next layer.
	 */
	bool sendToLayer(Directions direction, PacketPtr packet, 
		t_uint recvingLayerIdx);

	/**
	 * Send a packet to \e all adjacent layers.
	 * @param direction the direction of the action.
	 * @param packet a pointer to the packet being sent.
	 * @return true if the packet was successfully scheduled to
	 * be sent to all layers.
	 */
	bool sendToAllLayers(Directions direction, PacketPtr packet);

	/**
	 * Receives a packet from a sending layer.
	 * @param direction the direction the packet was sent.
	 * @param packet a pointer to the received packet.
	 * @param sendingLayer a pointer to the layer that sent the
	 * packet.
	 * @return true if the specified layer was able to receive
	 * the packet.
	 */
	bool recvFromLayer(Directions direction, PacketPtr packet, 
		CommunicationLayerPtr sendingLayer);

	/**
	 * Receives a packet from a sending layer.
	 * This is the function that subclasses should override to
	 * receive packets since it specifies the index of the sending
	 * layer rather than a pointer.
	 * @param direction the direction the packet was sent.
	 * @param packet a pointer to the received packet.
	 * @param sendingLayerIdx the index of the layer that sent the
	 * packet.
	 * @return true if the specified layer was able to receive
	 * the packet.
	 */
	virtual bool recvFromLayer(Directions direction, PacketPtr packet, 
		t_uint sendingLayerIdx);

	/**
	 * Get the layer type for an object.
	 * @return the type of the layer.
	 */
	virtual Types getLayerType() const = 0;

	/**
	 * Get the node ID associated with this object.
	 * @return the node ID.
	 */
	NodeId getNodeId() const;

protected:

	/** 
	 * A constructor.
	 * @param node the container of this object.
	 * @see getNode()
	 */
	CommunicationLayer(NodePtr node);

	/// Determines if a lower layer receive event
	/// has been scheduled.
	bool m_lowerLayerRecvEventPending;

	/**
	 * Get the node that contains this object.
	 */
	inline NodePtr getNode() const;

	/**
	 * Check if the packet queue is full.
	 * @return true if the packet queue is full.
	 */
	inline bool queueIsFull() const;

	/**
	 * Block the queues of each of our upper layers.
	 */
	void blockUpperQueues();

	/**
	 * Unblock the queue of each upper layer or until
	 * the queue fills up.
	 */
	void unblockUpperQueues();

	/**
	 * Send a packets from the queue while it is not empty
	 * and not blocked.
	 */
	void sendFromQueue();

	/**
	 * Set whether or not their is a packet currently pending
	 * for the lower layer.
	 * For example, if their is a layer delay then this will
	 * be pending until the lower layer actually receives the
	 * packet.
	 * @param isPending whether of not such an event is pending.
	 */
	inline void setLowerLayerRecvEventPending(bool isPending);

	/**
	 * Remove the layer specific data from a packet.
	 * @param packet a pointer to the packet whose layer
	 * data will be removed.
	 */
	void removeLayerData(PacketPtr packet) const;

	/**
	 * Get a pointer to the specified communication layer object.
	 * @param direction the direction the packet was sent.
	 * @param sendingLayerIdx the index of the layer that sent the
	 * packet.
	 * @return a pointer to the communication layer.
	 */
	inline CommunicationLayerPtr getLayer(Directions direction, 
		t_uint sendingLayerIdx);

	/**
	 * Add an adjacent layer in the specified direction.
	 * The layer being inserted must be owned by the same
	 * node as this object.
	 * @param direction the direction of the action.
	 * @param layerToInsert a pointer to the neighboring layer.
	 */
	inline void insertLayer(Directions direction, 
		CommunicationLayerPtr layerToInsert);

private: 

	/// The default maximum queue size.
	static const t_uint m_DEFAULT_MAX_QUEUE_LENGTH;

	/// The node that contains this object.
	/// @see setNode()
	/// @see getNode()
	NodePtr m_node;

	/// The time delay for sending to a lower layer.
	/// @see setLayerDelay()
	/// @see getLayerDelay()
	SimTime m_lowerLayerDelay;

	/// The time delay for sending to a upper layer.
	/// @see setLayerDelay()
	/// @see getLayerDelay()
	SimTime m_upperLayerDelay;

	/// Pointers to all lower layers with which this object can
	/// communicate.
	/// @see insertLayer()
	vector<CommunicationLayerPtr> m_lowerLayers;

	/// Pointers to all upper layers with which this object can
	/// communicate.
	/// @see insertLayer()
	vector<CommunicationLayerPtr> m_upperLayers;

	/// The index in the lower layer vector of the pointer to
	/// the default layer to use for sending.
	/// @see setDefaultLayer()
	/// @see getDefaultLayer()
	/// @see sendToLayer()
	t_uint m_defaultLowerLayerIdx;

	/// The index in the upper layer vector of the pointer to
	/// the default layer to use for sending.
	/// @see setDefaultLayer()
	/// @see getDefaultLayer()
	/// @see sendToLayer()
	t_uint m_defaultUpperLayerIdx;

	/// The maximum packet queue length.
	t_uint m_maxQueueLength;

	/// The packet queue with a pointer to each packet
	/// as well as the lower layer index of the intended
	/// receiver.
	list<pair<PacketPtr,t_uint> > m_packetQueue;

	/// Keep track of whether our queue is blocked.
	/// @see blockQueue()
	/// @see unblockQueue()
	/// @see getQueueIsBlocked()
	bool m_queueIsBlocked;

};
typedef boost::shared_ptr<CommunicationLayer> CommunicationLayerPtr;

/////////////////////////////////////////////////
// Inline Functions
/////////////////////////////////////////////////

inline NodePtr CommunicationLayer::getNode() const
{
	return m_node;
}

inline void CommunicationLayer::insertLowerLayer(
	CommunicationLayerPtr layerToInsert)
{
	assert(layerToInsert.get() != 0);
	assert(getNode() == layerToInsert->getNode());
	insertLayer(CommunicationLayer::Directions_Lower, layerToInsert);
	layerToInsert->insertLayer(CommunicationLayer::Directions_Upper, 
		shared_from_this());
}

inline void CommunicationLayer::insertLayer(
	CommunicationLayer::Directions direction,
	CommunicationLayerPtr layerToInsert)
{
	assert(layerToInsert.get() != 0);
	assert(getNode() == layerToInsert->getNode());
	switch(direction) {
	case CommunicationLayer::Directions_Lower:
		m_lowerLayers.push_back(layerToInsert);
		break;
	case CommunicationLayer::Directions_Upper:
		m_upperLayers.push_back(layerToInsert);
		break;
	default:
		assert(false);
	}
	// If this is the first layer inserted, set it as the
	// default layer
	if(numberOfLayers(direction) == 1)
		setDefaultLayer(direction, 0);
}

inline void CommunicationLayer::blockQueue()
{
	m_queueIsBlocked = true;
}

inline void CommunicationLayer::unblockQueue()
{
	m_queueIsBlocked = false;
	sendFromQueue();
}

inline void CommunicationLayer::setLowerLayerRecvEventPending(
	bool isPending)
{
	bool previousValue = m_lowerLayerRecvEventPending;
	m_lowerLayerRecvEventPending = isPending;
	// If there was an event pending, but there no longer is,
	// then we need to try sending from the queue again.
	if(previousValue && !m_lowerLayerRecvEventPending)
		sendFromQueue();
}

inline bool CommunicationLayer::getQueueIsBlocked() const
{
	return m_queueIsBlocked;
}

inline bool CommunicationLayer::queueIsFull() const
{
	assert(m_packetQueue.size() <= m_maxQueueLength);
	return (m_packetQueue.size() == m_maxQueueLength);
}

inline void CommunicationLayer::setMaxQueueLength(t_uint maxQueueLength)
{
	assert(maxQueueLength > 0);
	m_maxQueueLength = maxQueueLength;
}

inline t_uint CommunicationLayer::getMaxQueueLength() const
{
	return m_maxQueueLength;
}

inline CommunicationLayerPtr CommunicationLayer::getLayer(
	CommunicationLayer::Directions direction, t_uint sendingLayerIdx)
{
	assert(sendingLayerIdx < numberOfLayers(direction));

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

	return (*layerVector)[sendingLayerIdx];

}

inline t_uint CommunicationLayer::numberOfLayers(
	CommunicationLayer::Directions direction) const
{
	switch(direction) {
	case CommunicationLayer::Directions_Lower:
		return m_lowerLayers.size();
		break;
	case CommunicationLayer::Directions_Upper:
		return m_upperLayers.size();
		break;
	default:
		assert(false);
		return 0;
	}
}

inline bool CommunicationLayer::setDefaultLayer(
	CommunicationLayer::Directions direction, t_uint newDefaultIdx)
{
	bool setDefaultLayer = false;
	if(newDefaultIdx < numberOfLayers(direction)) {
		switch(direction) {
		case CommunicationLayer::Directions_Lower:
			m_defaultLowerLayerIdx = newDefaultIdx;
			break;
		case CommunicationLayer::Directions_Upper:
			m_defaultUpperLayerIdx = newDefaultIdx;
			break;
		default:
			assert(false);
		}
		setDefaultLayer = true;
	}

	return setDefaultLayer;
}

inline t_uint CommunicationLayer::getDefaultLayer(
	CommunicationLayer::Directions direction) const
{
	switch(direction) {
	case CommunicationLayer::Directions_Lower:
		return m_defaultLowerLayerIdx;
		break;
	case CommunicationLayer::Directions_Upper:
		return m_defaultUpperLayerIdx;
		break;
	default:
		assert(false);
		return 0;
	}
}

inline void CommunicationLayer::setLayerDelay(
	CommunicationLayer::Directions direction, const SimTime& delay)
{
	switch(direction) {
	case CommunicationLayer::Directions_Lower:
		m_lowerLayerDelay = delay;
		break;
	case CommunicationLayer::Directions_Upper:
		m_upperLayerDelay = delay;
		break;
	default:
		assert(false);
	}
}

inline SimTime CommunicationLayer::getLayerDelay(
	CommunicationLayer::Directions direction) const
{
	switch(direction) {
	case CommunicationLayer::Directions_Lower:
		return m_lowerLayerDelay;
		break;
	case CommunicationLayer::Directions_Upper:
		return m_upperLayerDelay;
		break;
	default:
		assert(false);
		SimTime neverReturned;
		return neverReturned;
	}
}

////////////////////////////////////////////////
// Overloaded Operators
/////////////////////////////////////////////////

inline ostream& operator<< (ostream& s, 
		const CommunicationLayer& communicationLayer)
{
	return s<< "CommunicationLayer state (pointer=" << 
			&communicationLayer << ", number of upper layers= " << 
			communicationLayer.numberOfLayers(
			CommunicationLayer::Directions_Upper) << 
			", number of lower layers= " <<
			communicationLayer.numberOfLayers(
			CommunicationLayer::Directions_Lower) << 
			", default upper layer= " <<
			communicationLayer.getDefaultLayer(
			CommunicationLayer::Directions_Upper) << 
			", default lower layer= " <<
			communicationLayer.getDefaultLayer(
			CommunicationLayer::Directions_Lower) << 
			", upper layer delay= " <<
			communicationLayer.getLayerDelay(
			CommunicationLayer::Directions_Upper) << 
			", lower layer delay= " <<
			communicationLayer.getLayerDelay(
			CommunicationLayer::Directions_Lower) << 
			")";
}

inline ostream& operator<< (ostream& s, 
		const CommunicationLayer::Types& communicationLayerType)
{

	switch (communicationLayerType) {
		case CommunicationLayer::Types_Physical:
			s << "PHY";
			break;
		case CommunicationLayer::Types_Link:
			s << "LINK";
			break;
		case CommunicationLayer::Types_Network:
			s << "NET";
			break;
		case CommunicationLayer::Types_Transport:
			s << "TRAN";
			break;
		case CommunicationLayer::Types_Application:
			s << "APP";
			break;
	}

	return s;

}

////////////////////////////////////////////////
// Event Subclasses
/////////////////////////////////////////////////

/**
 * This is the event for when a layer receives a packet
 * from one of its adjacent layers.
 */
class LayerRecvEvent : public Event {
public:
	/// Smart pointer that clients should use.
	typedef boost::shared_ptr<LayerRecvEvent> LayerRecvEventPtr;

	/**
	 * A factory method to ensure that all objects
	 * are created via \c new since we are using smart pointers. 
	 */
	static inline LayerRecvEventPtr create(
		CommunicationLayer::Directions sendDirection,
		PacketPtr packet, 
		CommunicationLayerPtr recvingLayer, 
		CommunicationLayerPtr sendingLayer)
	{
		LayerRecvEventPtr p(new LayerRecvEvent(sendDirection,
			packet, recvingLayer, sendingLayer));
		return p;
	}

	void execute()
	{
		// Give a default recvDirection for the compiler even
		// though the switch statement should be setup such
		// that it is guaranteed to set recvDirection.
		CommunicationLayer::Directions recvDirection = 
			CommunicationLayer::Directions_Upper;

		// Just toggle the direction from the sending direction.
		switch(m_sendDirection) {
		case CommunicationLayer::Directions_Lower:
			recvDirection = CommunicationLayer::Directions_Upper;
			break;
		case CommunicationLayer::Directions_Upper:
			recvDirection = CommunicationLayer::Directions_Lower;
			break;
		default:
			assert(false);
		}

		assert(m_recvingLayer.get() != 0);
		m_recvingLayer->recvFromLayer(recvDirection, m_packet, 
			m_sendingLayer);
		if(m_sendDirection == CommunicationLayer::Directions_Lower) {
			assert(m_sendingLayer.get() != 0);
			m_sendingLayer->setLowerLayerRecvEventPending(false);
		}
	}

protected:
	/// A constructor.
	LayerRecvEvent(CommunicationLayer::Directions sendDirection,
		PacketPtr packet, 
		CommunicationLayerPtr recvingLayer, 
		CommunicationLayerPtr sendingLayer)
			: Event()
	{
		m_sendDirection = sendDirection;
		m_packet = packet;
		m_recvingLayer = recvingLayer;
		m_sendingLayer = sendingLayer;
	}

private:
	PacketPtr m_packet;
	CommunicationLayer::Directions m_sendDirection;
	CommunicationLayerPtr m_recvingLayer;
	CommunicationLayerPtr m_sendingLayer;
};
typedef boost::shared_ptr<LayerRecvEvent> LayerRecvEventPtr;

#endif // COMMUNICATION_LAYER_H

