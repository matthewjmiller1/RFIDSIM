
#ifndef PACKET_H
#define PACKET_H

#include <map>
#include <iostream>
#include <iomanip>
using namespace std;
#include <boost/shared_ptr.hpp>

#include "utility.hpp"
#include "sim_time.hpp"
#include "node.hpp"

/**
 * A class which holds the data that is contained within a packet.
 */
class PacketData {
public:
	/// Smart pointer that clients should use.
	typedef boost::shared_ptr<PacketData> PacketDataPtr;

	/// A destructor.
	virtual ~PacketData() {}

	/**
	 * A factory method to ensure that all objects
	 * are created via \c new since we are using smart pointers. 
	 */
	static inline PacketDataPtr create()
	{
		PacketDataPtr p(new PacketData());
		return p;
	}

	/**
	 * A factory method to ensure that all objects
	 * are created via \c new since we are using smart pointers. 
	 */
	static inline PacketDataPtr create(const PacketData& rhs)
	{
		return rhs.clone();
	}

	/**
	 * Get the size of this data.
	 * @return the size of the data in bytes.
	 */
	virtual inline t_uint getSizeInBytes() const
	{
		return m_DEFAULT_SIZE_IN_BYTES;
	}

	/**
	 * Return the stream representation of the object.
	 */
	virtual inline ostream& print(ostream& s) const
	{
		return s << "sizeInBytes=" << getSizeInBytes();
	}

protected:

	/// A constructor.
	PacketData() {}

	/// A copy constructor.
	PacketData(const PacketData& rhs) {}

	/**
	 * Returns a pointer to a deep copy of this object.
	 * This is addresses the slicing problem with
	 * copy construction.
	 * @return a pointer to a deep copy of this object.
	 */
	virtual PacketDataPtr clone() const
	{
		PacketDataPtr p(new PacketData(*this));
		return p;
	}

private:

	/// The default size of packet data.
	static const t_uint m_DEFAULT_SIZE_IN_BYTES = 32;

	/// Declared private to restrict use.
	PacketData& operator= (const PacketData& rhs);

};
typedef boost::shared_ptr<PacketData> PacketDataPtr;

/**
 * Defines a representation of the packets that are exchanged between
 * nodes.
 */
class Packet {
friend ostream& operator<< (ostream& s, const Packet& packet);
public:
	/// Smart pointer that clients should use.
	typedef boost::shared_ptr<Packet> PacketPtr;

	/**
	 * DataTypes enum.
	 * The different data types that a packet may hold.
	 */
	enum DataTypes {
		DataTypes_Physical, /**< enum value DataTypes_Physical. */
		DataTypes_Link, /**< enum value DataTypes_Link. */
		DataTypes_Network, /**< enum value DataTypes_Network. */
		DataTypes_Transport, /**< enum value DataTypes_Transport. */
		DataTypes_Application /**< enum value DataTypes_Application. */
	};

	/// Map of a packet pointer for each of the network stack layers.
	typedef map<DataTypes,PacketDataPtr> DataTypeMap;

	/// A destructor.
	virtual ~Packet();

	/**
	 * A factory method to ensure that all objects
	 * are created via \c new since we are using smart pointers. 
	 */
	static inline PacketPtr create();

	/**
	 * A factory method to ensure that all objects
	 * are created via \c new since we are using smart pointers. 
	 */
	static inline PacketPtr create(const Packet& rhs);

	/**
	 * Returns the packet size.
	 * @return the number of bytes in the packet.
	 * @see getSizeInBites()
	 */
	inline t_uint getSizeInBytes() const;

	/**
	 * Returns the packet size.
	 * @return the number of \e bits in the packet.
	 * @see getSizeInBytes()
	 */
	inline t_uint getSizeInBits() const;

	/**
	 * Get the data rate at which the packet is sent.
	 * @return the data rate in bps.
	 * @see setDataRate()
	 */
	inline double getDataRate() const;
	
	/**
	 * Set the data rate at which the packet is sent.
	 * @return the data rate in bps.
	 * @see getDataRate()
	 */
	inline void setDataRate(double dataRate);

	/**
	 * Get whether or not the packet has an error.
	 * @return true if the packet has an error.
	 * @see setHasError()
	 */
	inline bool getHasError() const;

	/**
	 * Set whether or not the packet has an error.
	 * @param hasError set to true if the packet has
	 * an error.
	 * @see getHasError()
	 */
	inline void setHasError(bool hasError);

	/**
	 * Get the time duration of the packet.
	 * @return the time duration of the packet.
	 */
	SimTime getDuration() const;

	/**
	 * Add a copy of the packet data to the packet
	 * as the specified type of data.
	 * @param dataType the type of the data within the packet.
	 * @param data the packet data; a deep copy of the data
	 * will be stored in the packet.
	 * @see getData()
	 * @see removeData()
	 */
	void addData(DataTypes dataType, const PacketData& data);

	/**
	 * Get a pointer to the data of the specified type
	 * stored in this packet.
	 * @param dataType the type of the data within the packet.
	 * @return a pointer to the data of \c dataType or an
	 * empty pointer if no data is stored for that \c dataType.
	 */
	PacketDataPtr getData(DataTypes dataType);

	/**
	 * Remove the data stored in the packet associated with
	 * the specified type.
	 * @param dataTypes the type of the data within the packet.
	 * @return true if the data of \c dataTypes was successfully
	 * removed.
	 */
	bool removeData(DataTypes dataTypes);

	/**
	 * Check if the packet has any data intended for upper
	 * layers.
	 * @param dataTypes the type of data for the layer being
	 * checked.
	 * @return true if there is any data in the packet for layers 
	 * above the once being checked.
	 */
	bool hasUpperLayerData(DataTypes dataTypes);

	/**
	 * Set the destination field of the packet.
	 * This is used by lower layers that are unaware of the
	 * data format of upper layers and does not count
	 * towards the size of the packet.
	 * @param destination the desintation.
	 */
	inline void setDestination(const NodeId& destination);

	/**
	 * Get the destination field of the packet.
	 * This is used by lower layers that are unaware of the
	 * data format of upper layers and does not count
	 * towards the size of the packet.
	 * @return the desintation.
	 */
	inline NodeId getDestination() const;

	/**
	 * Set the transmit power of the packet.
	 * If this is set to zero, then the
	 * current power level will be used.
	 * The value must be greater than or equal to zero.
	 * This is superseded if \c getDoMaxTxPower() is true.
	 * In this case, the packet will be transmitted at
	 * the physical layer's maximum power regardless
	 * of the input to this function.
	 * @param txPower the transmit power to use or zero
	 * if the current power level should be used.
	 * @see getTxPower()
	 * @see setDoMaxTxPower()
	 * @see getDoMaxTxPower()
	 */
	inline void setTxPower(double txPower);

	/**
	 * Get the transmit power of the packet.
	 * If this is set to zero, then the
	 * current power level will be used.
	 * The value must be greater than or equal to zero.
	 * This is superseded if \c getDoMaxTxPower() is true.
	 * In this case, the packet will be transmitted at
	 * the physical layer's maximum power regardless
	 * of the input to this function.
	 * @return the transmit power to use or zero
	 * if the current power level should be used.
	 * @see setTxPower()
	 * @see setDoMaxTxPower()
	 * @see getDoMaxTxPower()
	 */
	inline double getTxPower() const;

	/**
	 * Set whether the packet should be transmitted at
	 * the maximum power available.
	 * If this is set to true, then the value of \c getTxPower()
	 * is ignored.
	 * @param doMaxTxPower set to true if the packet
	 * should be transmitted at the maximum power level.
	 * @see getDoMaxTxPower()
	 * @see setTxPower()
	 * @see getTxPower()
	 */
	inline void setDoMaxTxPower(bool doMaxTxPower);

	/**
	 * Get whether the packet should be transmitted at
	 * the maximum power available.
	 * If this is set to true, then the value of \c getTxPower()
	 * is ignored.
	 * @return true if the packet
	 * should be transmitted at the maximum power level.
	 * @see setDoMaxTxPower()
	 * @see setTxPower()
	 * @see getTxPower()
	 */
	inline bool getDoMaxTxPower() const;

	/**
	 * Return the unique ID of this packet (used for debugging
	 * traces).
	 * @return the unique ID of the packet.
	 */
	inline t_ulong getUniqueId() const;

protected:

	/// A constructor.
	Packet();

	/// A copy constructor.
	Packet(const Packet& rhs);

	/**
	 * Returns a pointer to a deep copy of this object.
	 * This is addresses the slicing problem with
	 * copy construction.
	 * @return a pointer to a deep copy of this object.
	 */
	virtual PacketPtr clone() const;

private:

	/// The default size of the packet when no data
	/// is added.
	static const t_uint m_DEFAULT_SIZE_IN_BYTES;

	/// The default data rate at which the packet is sent.
	static const double m_DEFAULT_DATA_RATE;

	/// Default destination for packets.
	static const t_uint m_DEFAULT_DESTINATION;

	/// A counter to give each packet a unique ID
	/// for debugging purposes.
	static t_ulong m_nextUniqueId;

	/// The data rate of the packet in bps.
	/// @see getDataRate()
	/// @see setDataRate()
	double m_dataRate;

	/// The power at which the packet should be transmitted.
	/// If it is zero, then the current power level should be
	/// used.
	/// @see setTxPower()
	/// @see getTxPower()
	double m_txPower;

	/// If this is true, then the packet should be transmitted
	/// at the maximum power regardless of the value of
	/// \c m_txPower.
	/// @see setDoMaxTxPower()
	/// @see getDoMaxTxPower()
	bool m_doMaxTxPower;

	/// Indicates whether or not a packet error has occurred.
	/// @see getHasError()
	/// @see setHasError()
	bool m_hasError;

	/// Keeps track of the data in the packet associated
	/// with each dataType.
	/// @see addData()
	/// @see getData()
	/// @see removeData()
	DataTypeMap m_data;

	/// The destination of the packet, which lower
	/// layers can use if they are unaware of the
	/// data format from upper layers.
	/// This field doesn't count towards the packet's size.
	/// @see setDestination()
	/// @see getDestination()
	NodeId m_destination;

	/// The unique ID for this packet for debugging
	/// purposes.
	/// @see getUniqueId()
	t_ulong m_uniqueId;

	/// Delared private to restrict use.
	Packet& operator= (const Packet& rhs);

};

typedef boost::shared_ptr<Packet> PacketPtr;
typedef boost::shared_ptr<Packet const> ConstPacketPtr;

/////////////////////////////////////////////////
// Inline Functions
/////////////////////////////////////////////////

inline PacketPtr Packet::create()
{
	PacketPtr p(new Packet());
	p->m_uniqueId = Packet::m_nextUniqueId++;
	return p;
}

inline PacketPtr Packet::create(const Packet& rhs)
{
	return rhs.clone();
}

inline t_uint Packet::getSizeInBytes() const
{

	t_uint sizeInBytes = m_DEFAULT_SIZE_IN_BYTES;

	// If data has been added to the packet, then
	// we'll sum up the size of all the data rather
	// than using the default size.
	if(m_data.begin() != m_data.end()) {
		sizeInBytes = 0;
		DataTypeMap::const_iterator p;
		for(p = m_data.begin(); p != m_data.end(); ++p) {
			sizeInBytes += p->second->getSizeInBytes();
		}
	}

	return sizeInBytes;
}

inline t_uint Packet::getSizeInBits() const
{
	return (getSizeInBytes() * 8);
}

inline double Packet::getDataRate() const
{
	return m_dataRate;
}

inline void Packet::setDataRate(double dataRate)
{
	assert(dataRate > 0.0);
	m_dataRate = dataRate;
}

inline bool Packet::getHasError() const
{
	return m_hasError;
}

inline void Packet::setHasError(bool hasError)
{
	m_hasError = hasError;
}

inline void Packet::setDestination(const NodeId& destination)
{
	m_destination = destination;
}

inline NodeId Packet::getDestination() const
{
	return m_destination;
}

inline void Packet::setTxPower(double txPower)
{
	assert(txPower >= 0.0);
	m_txPower = txPower;
}

inline double Packet::getTxPower() const
{
	return m_txPower;
}

inline void Packet::setDoMaxTxPower(bool doMaxTxPower)
{
	m_doMaxTxPower = doMaxTxPower;
}

inline bool Packet::getDoMaxTxPower() const
{
	return m_doMaxTxPower;
}

inline t_ulong Packet::getUniqueId() const
{
	return m_uniqueId;
}

////////////////////////////////////////////////
// Overloaded Operators
/////////////////////////////////////////////////

inline ostream& operator<< (ostream& s, 
	const Packet::DataTypes& dataType)
{
	switch(dataType) {
	case Packet::DataTypes_Physical:
		s << "physical";
		break;
	case Packet::DataTypes_Link:
		s << "link";
		break;
	case Packet::DataTypes_Network:
		s << "network";
		break;
	case Packet::DataTypes_Transport:
		s << "transport";
		break;
	case Packet::DataTypes_Application:
		s << "application";
		break;
	}
	s << "LayerData";
	return s;
}

inline ostream& operator<< (ostream& s, const PacketData& data)
{
	return data.print(s);
}

inline ostream& operator<< (ostream& s, const Packet& packet)
{
	ostringstream powerStream;
	if(packet.getDoMaxTxPower()) {
		powerStream << ", doMaxTxPower=" << boolalpha << 
			packet.getDoMaxTxPower();
	} else {
		powerStream << ", txPower=" << packet.getTxPower();
	}

	// cout << "pointer=" << &packet;
	s << "[ packetState " << 
		"uniqueId=" << packet.getUniqueId() <<
		", sizeInBytes=" << packet.getSizeInBytes() << 
		powerStream.str() <<
		", dataRate=" << packet.getDataRate() << 
		", durationInSeconds=" << setprecision(8) << 
		packet.getDuration() << 
		", hasError=" << boolalpha << packet.getHasError() << " ]";

	typedef map<Packet::DataTypes,PacketDataPtr> DataTypeMap;
	// Put each of the packet headers in the stream
	if(packet.m_data.begin() != packet.m_data.end()) {
		DataTypeMap::const_iterator p;
		for(p = packet.m_data.begin(); p != packet.m_data.end(); ++p) {
			s << " [ " << p->first << " " << *p->second << " ]";
		}
	}
	return s;
}

#endif // PACKET_H

