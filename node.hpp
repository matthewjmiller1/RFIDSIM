
#ifndef NODE_H
#define NODE_H

#include <limits>
#include <iostream>
#include <sstream>
using namespace std;
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "sim_time.hpp"
#include "communication_layer.hpp"
#include "location.hpp"

class Event;
typedef boost::shared_ptr<Event> EventPtr;

////////////////////////////////////////////////
// Node Id Class
/////////////////////////////////////////////////

/**
 * The node ID class provides a representation of the node's
 * identifier.
 */
class NodeId
{
/// \todo Eventually, this class should be modified
/// such that the numericValue can handle much
/// larger types than just ints.  Ideally, the representation
/// should be at least 128 bits to handle IPv6 and maybe
/// larger to handle RFID tag ID sizes.
public:

	/// A constructor.
	/// @param numericValue the numeric value for
	/// the node's ID.
	NodeId(t_uint numericValue)
		: m_numericValue(numericValue) 
	{ }

	/**
	 * A constructor.
	 * @param byteArray a pointer to the first element of the
	 * byte array with which to construct
	 * the NodeId.  The zero-th element is the least significant
	 * byte.
	 * @param byteCount the number of bytes in the array.
	 */
	NodeId(const t_uchar* byteArray, t_uint byteCount)
	{
		m_numericValue = 0;
		for(t_uint i = 0; i < byteCount; i++) {
			if(i > sizeof(t_uint))
				assert(byteArray[i] == 0);
			m_numericValue += (byteArray[i] << (8 * i));
		}
	}

	/// A destructor.
	virtual ~NodeId() { }

	/**
	 * Write the NodeId's numeric value to the given byte array.
	 * @param byteArray a pointer to the first element of the byte
	 * array to be written.  The zero-th byte will be the least
	 * signficant byte.
	 * @param byteCount the number of bytes in the array.
	 */
	inline void writeToByteArray(t_uchar* byteArray, 
		t_uint byteCount) const
	{
		t_uint id = getNumericValue();
		assert(sizeof(t_uint) <= byteCount);
		// Reset the data to zero.
		fill(byteArray, &byteArray[byteCount], 0);
		t_uint i = 0;
		while(id > 0) {
			assert(i < byteCount);
			byteArray[i++] = id & 0xFF;
			id >>= 8;
		}
	}

	/**
	 * Return the value that indicates the broadcast
	 * destination.
	 * @return the address of the broadcast destination.
	 */
	static inline t_uint broadcastDestination()
	{
		return numeric_limits<t_uint>::max();
	}

	/**
	 * Get a string representation of the ID.
	 * @return the string representation.
	 */
	virtual string getStringValue() const
	{
		ostringstream streamValue;
		if(m_numericValue == broadcastDestination())
			streamValue << "BROADCAST";
		else
			streamValue << m_numericValue;
		return streamValue.str();
	}

	/**
	 * Get a numeric representation of the ID.
	 * @return the numeric representation.
	 */
	virtual t_uint getNumericValue() const
	{
		return m_numericValue;
	}

	/**
	 * Compare two IDs for equality.
	 * @return true if they are equal.
	 */
	inline bool operator== (const NodeId& rhs) const
	{
		return m_numericValue == rhs.m_numericValue;
	}

	/**
	 * Compare two IDs for inequality.
	 * @return true if the left hand ID is less than the right
	 * hand ID.
	 */
	inline bool operator< (const NodeId& rhs) const
	{
		return m_numericValue < rhs.m_numericValue;
	}

protected:

	/// The internal representation of the ID.
	/// @see getNumericValue()
	/// @see getStringValue()
	t_uint m_numericValue;

};

/**
 * The class for a node in our system, which consists of a stack
 * of CommunicationLayer objects connected together.
 * Eventually, this could contain things like an energy consumption
 * tracker and sensors as well.
 */
class Node : boost::noncopyable,
	public boost::enable_shared_from_this<Node> {
	// NOTE: For now, we will make this class noncopyable
	// since a copyable version would require maintaining
	// pointers to all added layers and copying each one
	// when necessary.
public:
	/// Smart pointer that clients should use.
	typedef boost::shared_ptr<Node> NodePtr;

	/// A destructor.
	virtual ~Node();

	/**
	 * A factory method to ensure that all objects
	 * are created via \c new since we are using smart pointers. 
	 * @param location the location object for this node.
	 * @param nodeId the ID of this node.
	 */
	static inline NodePtr create(const Location& location, 
		const NodeId& nodeId);

	/**
	 * Get a copy of the location of this node.
	 * @return a copy of the location of this node.
	 */
	inline Location getLocation() const;

	/**
	 * Get a copy of the ID of this node.
	 * @return a copy of the ID of this node.
	 */
	inline NodeId getNodeId() const;

	/**
	 * Get the current time at the node.
	 * Note that this may incorporate clock drift to be different
	 * that the simulator's currentTime().
	 * @return The current time at the node.
	 */
	SimTime currentTime() const;

	/**
	 * Add an event to the simulator's event queue.
	 * @param eventToSchedule a pointer to the event being scheduled.
	 * @param eventDelay how far in the future (from the node's local
	 * time) the event should be scheduled.
	 * @return true if the event was successfully scheduled.
	 * @see cancelEvent()
	 */
	bool scheduleEvent(EventPtr eventToSchedule,
		const SimTime& eventDelay);

	/**
	 * Cancel an event from the event queue.
	 * @param eventToCancel a pointer to the event being cancelled.
	 * @return true if the event was found and erased.
	 * @see scheduleEvent()
	 */
	bool cancelEvent(EventPtr eventToCancel);

protected:

	/// A constructor.
	/// @param location the location object for this node.
	/// @param nodeId the ID for this node.
	Node(const Location& location, const NodeId& nodeId);

private:

	/// The location for this node.
	/// @see getLocation()
	Location m_location;

	/// The ID of this node.
	/// @see getNodeId()
	NodeId m_nodeId;

};
typedef boost::shared_ptr<Node> NodePtr;

/////////////////////////////////////////////////
// Inline Functions
/////////////////////////////////////////////////

inline NodePtr Node::create(const Location& location, 
	const NodeId& nodeId)
{
	NodePtr p(new Node(location, nodeId));
	return p;
}

inline Location Node::getLocation() const
{
	return m_location;
}

inline NodeId Node::getNodeId() const
{
	return m_nodeId;
}

////////////////////////////////////////////////
// Overloaded Operators
/////////////////////////////////////////////////

inline ostream& operator<< (ostream& s, const Node& node)
{
	return s<< "Node state (pointer=" << &node << ")";
}

inline ostream& operator<< (ostream& s, const NodeId& nodeId)
{
	return s<< nodeId.getStringValue();
}

#endif // NODE_H

