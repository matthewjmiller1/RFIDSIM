
#ifndef LINK_LAYER_H
#define LINK_LAYER_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include "communication_layer.hpp"
#include "mac_protocol.hpp"
#include "simulator.hpp"

class Node;
typedef boost::shared_ptr<Node> NodePtr;

/////////////////////////////////////////////////
// LinkLayer Class
/////////////////////////////////////////////////

/**
 * This is the link layer in the network stack.
 * This class contains a MAC object and could handle functions
 * like ARP as well.
 */
class LinkLayer : public CommunicationLayer {
friend class MacProtocol;
public:
	/// Smart pointer that clients should use.
	typedef boost::shared_ptr<LinkLayer> LinkLayerPtr;

	/**
	 * A factory method to ensure that all objects
	 * are created via \c new since we are using smart pointers. 
	 * @param node the owner of this layer.
	 * @param macProtocol the media access protocol that will
	 * be used by this link layer.
	 */
	static inline LinkLayerPtr create(NodePtr node, 
		MacProtocolPtr macProtocol);

	/**
	 * Gives \c this pointer as a \c LinkLayer object.
	 * This is used to deal with \c shared_ptr's, \c this, and
	 * inheritance.
	 * @return a pointer to \c this object.
	 */
	inline LinkLayerPtr thisLinkLayer();

	/**
	 * Gives \c this pointer as a \c SimulationEndListener object.
	 * This is used to deal with \c shared_ptr's, \c this, and
	 * inheritance.
	 * @return a pointer to \c this object.
	 */
	inline SimulationEndListenerPtr thisSimulationEndListener();

	/**
	 * The function called when the simulation ends.
	 */
	virtual void simulationEndHandler();

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
	 * Receives a packet from the MAC protocol associated
	 * with this layer to be sent to the default next layer.
	 * @param direction the direction the packet was sent.
	 * @param packet a pointer to the received packet.
	 * @return true if the specified layer was able to receive
	 * the packet.
	 */
	bool recvFromMacProtocol(CommunicationLayer::Directions direction, 
		PacketPtr packet);

	/**
	 * Queries the underlying physical layer to determine whether
	 * the channel is busy.
	 * @return true if the channel is busy.
	 */
	bool channelIsBusy();

	/**
	 * Get the layer type of an object.
	 * @return the type of the layer.
	 */
	inline CommunicationLayer::Types getLayerType() const;

protected:
	
	/// A constructor.
	LinkLayer(NodePtr node, MacProtocolPtr macProtocol);

private:

	/// An internal pointer to the object to allow it
	/// to return \c this.
	boost::weak_ptr<LinkLayer> m_weakThis;

	/// The queue length for this layer.
	static const t_uint m_LINK_LAYER_QUEUE_LENGTH;


	/// A pointer to the MAC protocol being used by
	/// this link layer.
	MacProtocolPtr m_macProtocol;

};
typedef boost::shared_ptr<LinkLayer> LinkLayerPtr;

/////////////////////////////////////////////////
// Inline Functions
/////////////////////////////////////////////////

inline LinkLayerPtr LinkLayer::create(NodePtr node,
	MacProtocolPtr macProtocol)
{
	LinkLayerPtr p(new LinkLayer(node, macProtocol));
	p->m_weakThis = p;
	// thisLinkLayer() must be called after m_weakThis has
	// been set.
	assert(p->m_macProtocol.get() != 0);
	p->m_macProtocol->setLinkLayer(p->thisLinkLayer());
	Simulator::instance()->addSimulationEndListener(
		p->thisSimulationEndListener());
	return p;
}

inline LinkLayerPtr LinkLayer::thisLinkLayer()
{
	LinkLayerPtr p(m_weakThis);
	return p;
}

inline SimulationEndListenerPtr LinkLayer::thisSimulationEndListener()
{
	SimulationEndListenerPtr p(m_weakThis);
	return p;
}

inline CommunicationLayer::Types LinkLayer::getLayerType() const
{
	return CommunicationLayer::Types_Link;
}

/////////////////////////////////////////////////
// Overloaded Operators
/////////////////////////////////////////////////

#endif // LINK_LAYER_H

