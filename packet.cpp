
#include "packet.hpp"

const t_uint Packet::m_DEFAULT_SIZE_IN_BYTES = 512;
const double Packet::m_DEFAULT_DATA_RATE = 1e6;
const t_uint Packet::m_DEFAULT_DESTINATION = 0;

t_ulong Packet::m_nextUniqueId = 1;

Packet::Packet()
	: m_dataRate(m_DEFAULT_DATA_RATE),
	m_txPower(0.0), m_doMaxTxPower(false), m_hasError(false),
	m_destination(m_DEFAULT_DESTINATION), m_uniqueId(0)
{

}

Packet::~Packet()
{

}

Packet::Packet(const Packet& rhs)
	: m_dataRate(rhs.m_dataRate), m_txPower(rhs.m_txPower), 
	m_hasError(rhs.m_hasError),
	m_destination(rhs.m_destination), m_uniqueId(rhs.m_uniqueId)
{
	// Create a deep copy of the packet's data
	if(rhs.m_data.begin() != rhs.m_data.end()) {
		DataTypeMap::const_iterator p;
		for(p = rhs.m_data.begin(); p != rhs.m_data.end(); ++p) {
			m_data[p->first] = PacketData::create(*(p->second));
		}
	}
}

PacketPtr Packet::clone() const
{
	PacketPtr p(new Packet(*this));
	return p;
}

void Packet::addData(Packet::DataTypes dataType, const PacketData& data)
{
	PacketDataPtr deepCopy = PacketData::create(data);
	m_data[dataType] = deepCopy;
}

PacketDataPtr Packet::getData(Packet::DataTypes dataType)
{
	DataTypeMap::iterator dataIterator = m_data.find(dataType);
	bool dataFound = (dataIterator != m_data.end());

	PacketDataPtr dataPtr;
	if(dataFound) {
		dataPtr = dataIterator->second;
	}

	return dataPtr;

}

bool Packet::removeData(Packet::DataTypes dataType)
{
	int numRemoved = m_data.erase(dataType);
	bool wasSuccessful = (numRemoved > 0);
	return wasSuccessful;
}

bool Packet::hasUpperLayerData(Packet::DataTypes dataType)
{

	bool hasUpperData = false;

	if(dataType == Packet::DataTypes_Transport ||
			dataType == Packet::DataTypes_Network ||
			dataType == Packet::DataTypes_Link ||
			dataType == Packet::DataTypes_Physical) {
		bool hasApplicationData = 
			(getData(Packet::DataTypes_Application).get() != 0);
		hasUpperData |= hasApplicationData;
	}

	if(!hasUpperData && (dataType == Packet::DataTypes_Network ||
			dataType == Packet::DataTypes_Link ||
			dataType == Packet::DataTypes_Physical)) {
		bool hasTransportData = 
			(getData(Packet::DataTypes_Transport).get() != 0);
		hasUpperData |= hasTransportData;
	}

	if(!hasUpperData && (dataType == Packet::DataTypes_Link ||
			dataType == Packet::DataTypes_Physical)) {
		bool hasNetworkData = 
			(getData(Packet::DataTypes_Network).get() != 0);
		hasUpperData |= hasNetworkData;
	}

	if(!hasUpperData && (dataType == Packet::DataTypes_Physical)) {
		bool hasLinkData = 
			(getData(Packet::DataTypes_Link).get() != 0);
		hasUpperData |= hasLinkData;
	}

	return hasUpperData;

}

SimTime Packet::getDuration() const
{
	double seconds = getSizeInBits() / getDataRate();
	SimTime packetDuration(seconds);
	return packetDuration;
}


