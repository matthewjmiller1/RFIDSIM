
#ifndef RFID_TAG_APP_H
#define RFID_TAG_APP_H

#include <algorithm>
using namespace std;
#include <boost/shared_ptr.hpp>

#include "application_layer.hpp"
#include "packet.hpp"
#include "simulator.hpp"

/**
 * The RFID tag application class.
 */
class RfidTagApp : public ApplicationLayer {
public:
	/// Smart pointer that clients should use.
	typedef boost::shared_ptr<RfidTagApp> RfidTagAppPtr;

	/**
	 * A factory method to ensure that all objects
	 * are created via \c new since we are using smart pointers. 
	 */
	static inline RfidTagAppPtr create(NodePtr node);

	/**
	 * Gives \c this pointer as a \c RfidTagApp object.
	 * This is used to deal with \c shared_ptr's, \c this, and
	 * inheritance.
	 * @return a pointer to \c this object.
	 */
	virtual inline RfidTagAppPtr thisRfidTagApp();

	/**
	 * Gives \c this pointer as a \c ApplicationLayer object.
	 * This is used to deal with \c shared_ptr's, \c this, and
	 * inheritance.
	 * @return a pointer to \c this object.
	 */
	virtual inline ApplicationLayerPtr thisApplicationLayer();

	/**
	 * Gives \c this pointer as a \c SimulationEndListener object.
	 * This is used to deal with \c shared_ptr's, \c this, and
	 * inheritance.
	 * @return a pointer to \c this object.
	 */
	virtual inline SimulationEndListenerPtr thisSimulationEndListener();

	/**
	 * Function to execute when the application is started.
	 */
	void startHandler();

	/**
	 * Function to execute when the application is stopped.
	 */
	void stopHandler();

	/**
	 * Determine whether or not the state of this tag
	 * allows it to reply to read requests.
	 * @return true if the tag can reply to read requests.
	 */
	bool getReplyToReads() const;

	/**
	 * Set whether or not the state of this tag
	 * allows it to reply to read requests.
	 * @param replyToReads whether the tag can reply to read requests.
	 */
	void setReplyToReads(bool replyToReads);

	/**
	 * The function called when the simulation ends.
	 */
	virtual void simulationEndHandler();

	/**
	 * Return the stream representation of the object.
	 */
	inline ostream& print(ostream& s) const;

protected:

	/// A constructor.
	RfidTagApp(NodePtr node);

	/**
	 * Handle a received packet.
	 * @param packet a pointer to the received packet.
	 * @param sendingLayerIdx the index of the layer that sent the
	 * packet.
	 * @return true if the packet was handled successfully.
	 */
	bool handleRecvdPacket(PacketPtr packet, t_uint sendingLayerIdx);

private: 

	/// An internal pointer to the object to allow it
	/// to return \c this.
	boost::weak_ptr<RfidTagApp> m_weakThis;

	/// Determines whether or not the tag will reply to
	/// read packets or ignore them.
	bool m_replyToReads;

	/**
	 * Send an ID packet to the destination.
	 * @param destination the destination to which
	 * the packet will be sent.
	 */
	void sendIdPacket(const NodeId& destination);

};
typedef boost::shared_ptr<RfidTagApp> RfidTagAppPtr;

/////////////////////////////////////////////////
// Inline Functions
/////////////////////////////////////////////////

inline RfidTagAppPtr RfidTagApp::create(NodePtr node)
{
	RfidTagAppPtr p(new RfidTagApp(node));
	p->m_weakThis = p;
	// weakThis *must* be set before the this function is called.
	Simulator::instance()->addSimulationEndListener(
		p->thisSimulationEndListener());
	return p;
}

inline RfidTagAppPtr RfidTagApp::thisRfidTagApp()
{
	RfidTagAppPtr p(m_weakThis);
	return p;
}

inline ApplicationLayerPtr RfidTagApp::thisApplicationLayer()
{
	ApplicationLayerPtr p(m_weakThis);
	return p;
}

inline SimulationEndListenerPtr RfidTagApp::thisSimulationEndListener()
{
	SimulationEndListenerPtr p(m_weakThis);
	return p;
}

////////////////////////////////////////////////
// Overloaded Operators
/////////////////////////////////////////////////

////////////////////////////////////////////////
// PacketData Subclass
/////////////////////////////////////////////////

/**
 * The representation of the application data in an RfidTagApp's
 * data packet.
 */
class RfidTagAppData : public PacketData {
public:
	/// Smart pointer that clients should use.
	typedef boost::shared_ptr<RfidTagAppData> RfidTagAppDataPtr;

	/**
	 * Types enum.
	 * Specifies the types of packets that are sent.
	 */
	enum Types {
		Types_NoType, /**< enum value Types_NoType. */
		Types_Reply /**< enum value Types_Reply. */
	};

	/**
	 * A factory method to ensure that all objects
	 * are created via \c new since we are using smart pointers. 
	 */
	static inline RfidTagAppDataPtr create();

	/**
	 * A factory method to ensure that all objects
	 * are created via \c new since we are using smart pointers. 
	 */
	static inline RfidTagAppDataPtr create(const RfidTagAppData& rhs);

	/**
	 * Get the size of this data.
	 * @return the size of the data in bytes.
	 */
	virtual inline t_uint getSizeInBytes() const;

	/**
	 * Write the node ID to the packet data.
	 * @param nodeId the ID to be written.
	 */
	void setTagId(const NodeId& nodeId);

	/**
	 * Read the node ID from the packet data.
	 * @return the ID of the node in the packet.
	 */
	NodeId getTagId() const;

	/**
	 * Write the type field to this packet data.
	 * @param type the value for the type field.
	 */
	inline void setType(Types type);

	/**
	 * Read the type field from this packet data.
	 * @return the type field of the packet.
	 */
	inline Types getType() const;

	/**
	 * Return the stream representation of the object.
	 */
	inline ostream& print(ostream& s) const;

protected:

	/// A constructor.
	RfidTagAppData();

	/// A copy constructor.
	RfidTagAppData(const RfidTagAppData& rhs);

	/**
	 * Returns a pointer to a deep copy of this object.
	 * This is addresses the slicing problem with
	 * copy construction.
	 * @return a pointer to a deep copy of this object.
	 */
	virtual PacketDataPtr clone() const;

private:

	/// The default number of bytes for a tag ID.
	/// Most of the cheap tags seem to use 96 bits right now.
	static const t_uint m_nodeIdBytes = 12;

	/// The default number of bytes for a packet type.
	static const t_uint m_typeBytes = 1;

	/// The data field for the node ID.
	t_uchar m_nodeId[m_nodeIdBytes];

	/// The type field for the packet.
	Types m_type;

};
typedef boost::shared_ptr<RfidTagAppData> RfidTagAppDataPtr;

/////////////////////////////////////////////////
// Inline Functions
/////////////////////////////////////////////////

inline RfidTagAppDataPtr RfidTagAppData::create()
{
	RfidTagAppDataPtr p(new RfidTagAppData());
	return p;
}

inline RfidTagAppDataPtr RfidTagAppData::create(const RfidTagAppData& rhs)
{
	RfidTagAppDataPtr p = 
		boost::dynamic_pointer_cast<RfidTagAppData>(rhs.clone());
	// If the shared_ptr is empty, then the cast failed.
	assert(p.get() != 0);
	return p;
}

inline t_uint RfidTagAppData::getSizeInBytes() const
{
	return (m_nodeIdBytes + m_typeBytes);
}

inline void RfidTagAppData::setType(RfidTagAppData::Types type)
{
	m_type = type;
}

inline RfidTagAppData::Types RfidTagAppData::getType() const
{
	return m_type;
}

/////////////////////////////////////////////////
// Overloaded Operators
/////////////////////////////////////////////////

inline ostream& operator<< (ostream& s,
	const RfidTagAppData::Types& dataType)
{
	switch(dataType) {
	case RfidTagAppData::Types_NoType:
		s << "NO_TYPE";
		break;
	case RfidTagAppData::Types_Reply:
		s << "REPLY";
		break;
	}
	return s;
}

inline ostream& RfidTagAppData::print(ostream& s) const
{
	s << "type=" << m_type << ", " <<
		"nodeId=" << NodeId(m_nodeId, m_nodeIdBytes);
	return s;
}

#endif // RFID_TAG_APP_H

