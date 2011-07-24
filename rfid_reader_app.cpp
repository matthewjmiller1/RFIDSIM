
#include "rfid_reader_app.hpp"
#include "rfid_tag_app.hpp"
#include "node.hpp"
#include "physical_layer.hpp"

////////////////////////////////////////////////
// RfidReaderApp functions
/////////////////////////////////////////////////

const string RfidReaderApp::m_TAGS_READ_COUNT_STRING = "tagsReadCount";
const string RfidReaderApp::m_TAGS_READ_AT_POWER_COUNT_STRING = "tagsReadCountAtLevel_";
const string RfidReaderApp::m_TAGS_READ_AVG_LATENCY_STRING = "avgTagReadLatency";
const string RfidReaderApp::m_TAGS_READ_PROCESS_AVG_LATENCY_STRING = "avgTagReadProcessLatency";
const string RfidReaderApp::m_LAST_TAG_READ_LATENCY_STRING = "lastTagReadLatency";
const string RfidReaderApp::m_TAG_READ_PROCESS_LATENCY_STRING = "tagReadProcessLatency";
const string RfidReaderApp::m_TAG_READ_LATENCY_STRING = "tagReadLatency";
const string RfidReaderApp::m_TAG_READ_LEVEL_STRING = "tagReadPowerLevel";
const string RfidReaderApp::m_TAG_READ_ID_STRING = "tagReadId";
const string RfidReaderApp::m_TAG_READ_TIME_STRING = "tagReadTime";
const double RfidReaderApp::m_DEFAULT_READ_PERIOD = 60.0;
const t_uint RfidReaderApp::m_DEFAULT_NUM_POWER_CONTROL_LEVELS = 1;

/**
 * The application class for RFID readers.
 * Allows readers to request that tags reset their state and
 * send a read request to all tags within range.
 */
RfidReaderApp::RfidReaderApp(NodePtr node, PhysicalLayerPtr physicalLayer)
	: ApplicationLayer(node), 
	m_firstReadSentTime(0.0),
	m_previousReadSentTime(0.0),
	m_lastTagRead(0,ReadTagData(0,0.0,0.0)),
	m_physicalLayer(physicalLayer),
	m_readPeriod(m_DEFAULT_READ_PERIOD), m_doRepeatedReads(false), 
	m_doReset(true),
	m_numPowerControlLevels(m_DEFAULT_NUM_POWER_CONTROL_LEVELS),
	m_maxTxPower(0.0), 
	m_currentTxPowerLevel(m_DEFAULT_NUM_POWER_CONTROL_LEVELS)
{
	//assert(node == physicalLayer->getNode());
}

RfidReaderApp::~RfidReaderApp()
{

}

void RfidReaderApp::simulationEndHandler()
{

	SimTime readLatencySum(0.0);
	SimTime readProcessLatencySum(0.0);
	for(TagIterator i = m_readTags.begin(); i != m_readTags.end(); ++i) {
		SimTime tagReadProcessLatency = 
			(i->second.getTimeRead() - m_firstReadSentTime);
		readProcessLatencySum += tagReadProcessLatency;

		SimTime tagReadLatency = i->second.getReadLatency();
		readLatencySum += tagReadLatency;

		ostringstream tagIdStream;
		tagIdStream << i->second.getReadTagId();
		LogStreamManager::instance()->logStatsItem(getNodeId(), 
			m_TAG_READ_ID_STRING, tagIdStream.str());

		ostringstream tagReadLevelStream;
		tagReadLevelStream << (i->first + 1);
		LogStreamManager::instance()->logStatsItem(getNodeId(), 
			m_TAG_READ_LEVEL_STRING, tagReadLevelStream.str());

		ostringstream tagReadTimeStream;
		tagReadTimeStream << i->second.getTimeRead();
		LogStreamManager::instance()->logStatsItem(getNodeId(), 
			m_TAG_READ_TIME_STRING, tagReadTimeStream.str());

		ostringstream tagReadLatencyStream;
		tagReadLatencyStream << setprecision(8) << tagReadProcessLatency;
		LogStreamManager::instance()->logStatsItem(getNodeId(), 
			m_TAG_READ_PROCESS_LATENCY_STRING, tagReadLatencyStream.str());

		/*
		ostringstream tagLatencyStream;
		tagLatencyStream << setprecision(8) << tagReadLatency;
		LogStreamManager::instance()->logStatsItem(getNodeId(), 
			m_TAG_READ_LATENCY_STRING, tagLatencyStream.str());
		*/
	}

	for(t_uint i = 0; i < m_numPowerControlLevels; ++i) {
		pair<TagIterator,TagIterator> p = m_readTags.equal_range(i);

		t_uint tagsReadAtPowerTotal = 0;
		for(TagIterator j = p.first; j != p.second; ++j)
			tagsReadAtPowerTotal++;

		ostringstream tagsReadAtPowerTotalStream;
		tagsReadAtPowerTotalStream << tagsReadAtPowerTotal;
		ostringstream powerLevelStream;
		powerLevelStream << m_TAGS_READ_AT_POWER_COUNT_STRING << (i + 1);
		LogStreamManager::instance()->logStatsItem(getNodeId(), 
			powerLevelStream.str(), tagsReadAtPowerTotalStream.str());

	}

	t_uint tagsReadTotal = m_readTags.size();
	ostringstream tagsReadTotalStream;
	tagsReadTotalStream << tagsReadTotal;
	LogStreamManager::instance()->logStatsItem(getNodeId(), 
		m_TAGS_READ_COUNT_STRING, tagsReadTotalStream.str());

	double readLatencyAverage = 0.0;
	double readProcessLatencyAverage = 0.0;
	if(tagsReadTotal > 0) {
		readLatencyAverage = 
			readLatencySum.getTimeInSeconds() / tagsReadTotal;
		readProcessLatencyAverage = 
			readProcessLatencySum.getTimeInSeconds() / tagsReadTotal;
	}

	/*
	ostringstream readLatencyStream;
	readLatencyStream << setprecision(8) << readLatencyAverage;
	LogStreamManager::instance()->logStatsItem(getNodeId(), 
		m_TAGS_READ_AVG_LATENCY_STRING, readLatencyStream.str());
	*/

	ostringstream readProcessLatencyStream;
	readProcessLatencyStream << setprecision(8) << 
		readProcessLatencyAverage;
	LogStreamManager::instance()->logStatsItem(getNodeId(), 
		m_TAGS_READ_PROCESS_AVG_LATENCY_STRING, 
		readProcessLatencyStream.str());

	SimTime lastReadLatency(0.0);
	if(!m_readTags.empty()) {
		lastReadLatency = (m_lastTagRead.second.getTimeRead() -
			m_firstReadSentTime);
	}
	ostringstream lastTagReadLatencyStream;
	lastTagReadLatencyStream << setprecision(8) << lastReadLatency;
	LogStreamManager::instance()->logStatsItem(getNodeId(), 
		m_LAST_TAG_READ_LATENCY_STRING, lastTagReadLatencyStream.str());
}

void RfidReaderApp::startHandler()
{
	// We will assume that the current PHY TX power is
	// the max that will be used and go in uniformly
	// spaced steps between (0,max].
	m_maxTxPower = m_physicalLayer->getMaxTxPower();
	doReadProcess();
}

void RfidReaderApp::stopHandler()
{
	m_readTimer->stop();
}

bool RfidReaderApp::handleRecvdPacket(PacketPtr packet, 
	t_uint recvLayerIdx)
{
	if(!m_isRunning)
		return false;

	RfidTagAppDataPtr tagData = 
		boost::dynamic_pointer_cast<RfidTagAppData>
		(packet->getData(Packet::DataTypes_Application));

	RfidReaderAppDataPtr readerData = 
		boost::dynamic_pointer_cast<RfidReaderAppData>
		(packet->getData(Packet::DataTypes_Application));

	bool wasSuccessful = false;
	// If the shared_ptr is empty, then either the cast failed
	// (which could be the result of the packet having no
	// application data).
	if(tagData.get() != 0) {
		NodeId readTagId = tagData->getTagId();

		bool isNewTagId =
			(m_readTagIds.find(readTagId) == m_readTagIds.end());
		if(isNewTagId) {
			SimTime timeRead = Simulator::instance()->currentTime();
			m_lastTagRead =
				make_pair(m_currentTxPowerLevel, ReadTagData(readTagId, 
				timeRead, m_previousReadSentTime));
			m_readTags.insert(m_lastTagRead);
			pair<set<NodeId>::iterator, bool> p = 
				m_readTagIds.insert(readTagId);
			assert(p.second);
		}

		wasSuccessful = true;
	} else if(readerData.get() != 0) {
		// It may have come from another reader.
		wasSuccessful = true;
	}

	return wasSuccessful;
}

void RfidReaderApp::signalReadEnd()
{
	m_currentTxPowerLevel++;
	doNextRead();
}

void RfidReaderApp::doNextRead()
{
	if(m_currentTxPowerLevel < m_numPowerControlLevels) {

		/*
		// Use this value if the next TX power is
		// independent of the path loss model.
		double nextTxPower = (m_currentTxPowerLevel + 1) * 
			(m_maxTxPower / m_numPowerControlLevels);
		*/

		// This assumes a path loss model of the square of the
		// distance.  If the power changes, the root and
		// power function should be changed to reflect the
		// new exponent.
		double nextTxPower = 
			m_maxTxPower *
				pow(((m_currentTxPowerLevel + 1) / 
				static_cast<double>(m_numPowerControlLevels)), 2);
	
		if(m_DEBUG_POWER_CONTROL) {
			ostringstream debugStream;
			debugStream << __PRETTY_FUNCTION__ << " nextTxPower: " <<
				nextTxPower << ", maxTxPower: " << m_maxTxPower;
			LogStreamManager::instance()->logDebugItem(debugStream.str());
		}
		m_previousReadSentTime = Simulator::instance()->currentTime();
		sendReadPacket(nextTxPower);
	} else {
		// Schedule the next read process.
		if(m_doRepeatedReads)
			m_readTimer->reschedule(m_readPeriod);
	}
}

void RfidReaderApp::doReadProcess()
{

	// Send a reset packet to reset the state of all tags
	// before beginning the process.
	if(m_doReset)
		sendResetPacket();

	assert(m_numPowerControlLevels > 0);

	m_firstReadSentTime = Simulator::instance()->currentTime();
	m_currentTxPowerLevel = 0;
	doNextRead();
}

void RfidReaderApp::sendResetPacket()
{
	PacketPtr packetToSend = Packet::create();
	packetToSend->setDestination(NodeId::broadcastDestination());

	RfidReaderAppDataPtr appData = RfidReaderAppData::create();
	appData->setType(RfidReaderAppData::Types_Reset);
	appData->setReaderId(getNodeId());
	packetToSend->addData(Packet::DataTypes_Application, *appData);

	sendToQueue(packetToSend);
}

void RfidReaderApp::sendReadPacket(double txPower)
{
	PacketPtr packetToSend = Packet::create();
	packetToSend->setTxPower(txPower);
	packetToSend->setDestination(NodeId::broadcastDestination());

	RfidReaderAppDataPtr appData = RfidReaderAppData::create();
	appData->setType(RfidReaderAppData::Types_Read);
	appData->setReaderId(getNodeId());

	// On the last read, we'll go for the entire cycle.
	if(packetToSend->getTxPower() == m_maxTxPower)
		appData->setDoEntireReadCycle(true);
	else
		appData->setDoEntireReadCycle(false);

	packetToSend->addData(Packet::DataTypes_Application, *appData);

	//sendToLayer(CommunicationLayer::Directions_Lower, packetToSend);
	sendToQueue(packetToSend);
}

////////////////////////////////////////////////
// RfidReaderAppData functions
/////////////////////////////////////////////////

RfidReaderAppData::RfidReaderAppData()
	: m_type(RfidReaderAppData::Types_NoType), m_doEntireReadCycle(false)
{
	fill(m_nodeId, &m_nodeId[m_nodeIdBytes], 0);
}

RfidReaderAppData::RfidReaderAppData(const RfidReaderAppData& rhs)
	: PacketData(rhs), m_type(rhs.m_type), 
	m_doEntireReadCycle(rhs.m_doEntireReadCycle)
{
	copy(rhs.m_nodeId, &rhs.m_nodeId[m_nodeIdBytes], m_nodeId);
}

PacketDataPtr RfidReaderAppData::clone() const
{
	 PacketDataPtr p(new RfidReaderAppData(*this));
	 return p;
}

void RfidReaderAppData::setReaderId(const NodeId& nodeId)
{
	nodeId.writeToByteArray(m_nodeId, m_nodeIdBytes);
}

NodeId RfidReaderAppData::getReaderId() const
{
	NodeId nodeId(m_nodeId, m_nodeIdBytes);
	return nodeId;
}


