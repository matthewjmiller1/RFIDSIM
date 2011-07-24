
#include "log_stream_manager.hpp"
#include "simulator.hpp"
#include "packet.hpp"
#include "node.hpp"

LogStreamManagerPtr LogStreamManager::m_instance;

LogStreamManager::LogStreamManager()
{

	// NOTE: You must open the streams to write to cout in this manner.
	// Using the form: 
	// m_pktRecvStream.reset(&cout);
	// will result in a seg fault when free is called by the shared_ptr.
	m_pktSendStream.reset(new ostream(cout.rdbuf()));
	m_pktRecvStream.reset(new ostream(cout.rdbuf()));
	m_energyStream.reset(new ostream(cout.rdbuf()));
	m_newSenseEventStream.reset(new ostream(cout.rdbuf()));
	m_sensedEventStream.reset(new ostream(cout.rdbuf()));
	m_statsStream.reset(new ostream(cout.rdbuf()));
	m_userDefinedStream.reset(new ostream(cout.rdbuf()));
	m_debugStream.reset(new ostream(cout.rdbuf()));

}

LogStreamManager::~LogStreamManager()
{

}

void LogStreamManager::logPktSendItem(const NodeId& nodeId,
	const CommunicationLayer::Types& layer, const Packet& sendPkt)
{
	assert(m_pktSendStream != 0);
	*m_pktSendStream << 
		eventString(LogStreamManager::LogEvents_PktSend) <<
		" " << currentTimeString() << " " <<
		nodeIdString(nodeId) << " " << 
		layerString(layer) << " " << sendPkt << endl;
}

void LogStreamManager::logPktRecvItem(const NodeId& nodeId,
	const CommunicationLayer::Types& layer, const Packet& recvPkt)
{
	assert(m_pktRecvStream != 0);
	*m_pktRecvStream << 
		eventString(LogStreamManager::LogEvents_PktRecv) <<
		" " << currentTimeString() << " " <<
		nodeIdString(nodeId) << " " << 
		layerString(layer) << " " << recvPkt << endl;
}

void LogStreamManager::logStatsItem(const string& statsKeyString, 
	const string& statsValueString)
{
	assert(m_statsStream != 0);
	*m_statsStream << 
		eventString(LogStreamManager::LogEvents_Stats) <<
		" " << currentTimeString() << " " << 
		"-globalStat -" << statsKeyString <<
		" " << statsValueString << endl;
}

void LogStreamManager::logStatsItem(const NodeId& nodeId,
	const string& statsKeyString, const string& statsValueString)
{
	assert(m_statsStream != 0);
	*m_statsStream << 
		eventString(LogStreamManager::LogEvents_Stats) <<
		" " << currentTimeString() << " " << 
		nodeIdString(nodeId) << " -" << statsKeyString <<
		" " << statsValueString << endl;
}

void LogStreamManager::logUserDefinedItem(const string& userString)
{
	assert(m_userDefinedStream != 0);
	*m_userDefinedStream << 
		eventString(LogStreamManager::LogEvents_UserDefined) <<
		" " << currentTimeString() << " " << 
		userString << endl;
}

void LogStreamManager::logDebugItem(const string& debugString)
{
	assert(m_debugStream != 0);
	*m_debugStream << 
		eventString(LogStreamManager::LogEvents_Debug) <<
		" " << currentTimeString() << " " << 
		debugString << endl;
}

string LogStreamManager::eventString(
	const LogStreamManager::LogEvents& event) const
{
	ostringstream eventStream;
	eventStream << "-event " << event;
	return eventStream.str();
}

string LogStreamManager::currentTimeString() const
{
	ostringstream curTimeStream;
	curTimeStream << "-time " << setprecision(12) << fixed <<
		Simulator::instance()->currentTime();
	return curTimeStream.str();
}

string LogStreamManager::nodeIdString(const NodeId& nodeId) const
{
	ostringstream nodeIdStream;
	nodeIdStream << "-nodeId " << nodeId;
	return nodeIdStream.str();
}

string LogStreamManager::layerString(
	const CommunicationLayer::Types& layer) const
{
	ostringstream layerStream;
	layerStream << "-layer " << layer;
	return layerStream.str();
}

