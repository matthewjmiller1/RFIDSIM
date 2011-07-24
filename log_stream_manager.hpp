
#ifndef LOG_STREAM_MANAGER_H
#define LOG_STREAM_MANAGER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
using namespace std;
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include "communication_layer.hpp"
#include "utility.hpp"

class Packet;
typedef boost::shared_ptr<Packet> PacketPtr;
class NodeId;

/**
 * Keeps track of which stream should be used for logging
 * various types of events.
 */
class LogStreamManager : boost::noncopyable {
private:

	/**
	 * LogEvents enum.
	 * Specifies the types of events that are logged.
	 */
	enum LogEvents {
		LogEvents_PktSend, /**< enum value LogEvents_PktSend. */
		LogEvents_PktRecv, /**< enum value LogEvents_PktRecv. */
		LogEvents_Stats, /**< enum value LogEvents_Stats. */
		LogEvents_UserDefined, /**< enum value LogEvents_UserDefined. */
		LogEvents_Debug /**< enum value LogEvents_Debug. */
	};
	friend ostream& operator<< (ostream& s, const LogEvents& logEvent);

public:
	/// Pointer that clients should use.
	typedef LogStreamManager* LogStreamManagerPtr;

	/**
	 * The class uses the Singleton design pattern.
	 * @return Pointer to the single instance of LogStreamManager
	 */
	static inline LogStreamManagerPtr instance();

	/**
	 * Log the packet being sent to the appropriate stream.
	 * @param nodeId the ID of the node logging the event.
	 * @param layer the layer that should be logged.
	 * @param recvPkt the packet that should be logged.
	 */
	void logPktSendItem(const NodeId& nodeId,
		const CommunicationLayer::Types& layer, const Packet& recvPkt);

	/**
	 * Log the packet being received to the appropriate stream.
	 * @param nodeId the ID of the node logging the event.
	 * @param layer the layer that should be logged.
	 * @param recvPkt the packet that should be logged.
	 */
	void logPktRecvItem(const NodeId& nodeId,
		const CommunicationLayer::Types& layer, const Packet& recvPkt);

	/*
	void logEnergyItem();

	void logNewSenseEventItem();

	void logSensedEventItem();
	*/

	/**
	 * Log the user-specified string to the appropriate stream.
	 * This function is for a global stat rather than a
	 * node-specifc stat.
	 * Each stat will be logged in the stream as:
	 * -statsKeyString statsValueString
	 * @param statsKeyString a string representing the key for 
	 * the stat being logged.
	 * @param statsValueString a string representing the value
	 * for the stat being logged.
	 */
	void logStatsItem(const string& statsKeyString, 
		const string& statsValueString);

	/**
	 * Log the user-specified string to the appropriate stream.
	 * Each stat will be logged in the stream as:
	 * -statsKeyString statsValueString
	 * @param nodeId the ID of the node logging the event.
	 * @param statsKeyString a string representing the key for 
	 * the stat being logged.
	 * @param statsValueString a string representing the value
	 * for the stat being logged.
	 */
	void logStatsItem(const NodeId& nodeId, 
		const string& statsKeyString, const string& statsValueString);

	/**
	 * Log the user-specified string to the appropriate stream.
	 * @param userString the string that should be logged.
	 */
	void logUserDefinedItem(const string& userString);

	/**
	 * Log the debug string to the appropriate stream.
	 * @param debugString the string that should be logged.
	 */
	void logDebugItem(const string& debugString);

	/**
	 * Set all of the streams to point to the given ostream.
	 * @param newStream the ostream to which all streams will point.
	 */
	inline void setAllStreams(ostreamPtr newStream);

	/**
	 * Set the packet send stream to point to the given ostream.
	 * @param newStream the ostream to which the packet send 
	 * stream will point.
	 */
	inline void setPktSendStream(ostreamPtr newStream);

	/**
	 * Set the packet receive stream to point to the given ostream.
	 * @param newStream the ostream to which the packet receive 
	 * stream will point.
	 */
	inline void setPktRecvStream(ostreamPtr newStream);

	/**
	 * Set the energy stream to point to the given ostream.
	 * @param newStream the ostream to which the energy
	 * stream will point.
	 */
	inline void setEnergyStream(ostreamPtr newStream);

	/**
	 * Set the new sense event stream to point to the given ostream.
	 * @param newStream the ostream to which the new sense event
	 * stream will point.
	 */
	inline void setNewSenseEventStream(ostreamPtr newStream);

	/**
	 * Set the sensed event stream to point to the given ostream.
	 * @param newStream the ostream to which the sensed event
	 * stream will point.
	 */
	inline void setSensedEventStream(ostreamPtr newStream);

	/**
	 * Set the stats stream to point to the given ostream.
	 * @param newStream the ostream to which the stats
	 * stream will point.
	 */
	inline void setStatsStream(ostreamPtr newStream);

	/**
	 * Set the user defined stream to point to the given ostream.
	 * @param newStream the ostream to which the user defined
	 * stream will point.
	 */
	inline void setUserDefinedStream(ostreamPtr newStream);

	/**
	 * Set the debug stream to point to the given ostream.
	 * @param newStream the ostream to which the debug
	 * stream will point.
	 */
	inline void setDebugStream(ostreamPtr newStream);

private:

	/// The lone instance of the log stream manager class.
	/// @see instance()
	static LogStreamManagerPtr m_instance;

	/// The stream used for packet sending events.
	/// @see setPktSendStream()
	/// @see logPktSendItem()
	/// @see setAllStreams()
	ostreamPtr m_pktSendStream;

	/// The stream used for received packet events.
	/// @see setPktRecvStream()
	/// @see logPktRecvItem()
	/// @see setAllStreams()
	ostreamPtr m_pktRecvStream;

	/// The stream used for energy-related events.
	/// @see setEnergyStream()
	/// @see logEnergyItem()
	/// @see setAllStreams()
	ostreamPtr m_energyStream;

	/// The stream used for new events which will be sensed.
	/// @see setNewSenseEventStream()
	/// @see logNewSenseEventItem()
	/// @see setAllStreams()
	ostreamPtr m_newSenseEventStream;

	/// The stream used for sensed events.
	/// @see setSensedEventStream()
	/// @see logSensedEventItem()
	/// @see setAllStreams()
	ostreamPtr m_sensedEventStream;

	/// The stream used for statistic strings.
	/// @see setStatsStream()
	/// @see logStatsItem()
	/// @see setAllStreams()
	ostreamPtr m_statsStream;

	/// The stream used for arbitrary user-defined strings.
	/// @see setUserDefinedStream()
	/// @see logUserDefinedItem()
	/// @see setAllStreams()
	ostreamPtr m_userDefinedStream;

	/// The stream used for debug events.
	/// @see setDebugStream()
	/// @see logDebugItem()
	/// @see setAllStreams()
	ostreamPtr m_debugStream;

	/// A constructor.
	LogStreamManager();

	/// A destructor.
	/// This is private to avoid smart pointers to this
	/// class since it is a Singleton.
	~LogStreamManager();

	/**
	 * Return a string representation of the simulator's current
	 * time to be used in our output streams.
	 * @return a string representation of the simulator's current
	 * time.
	 */
	string currentTimeString() const;

	/**
	 * Return a string representation of the given layer to be used
	 * in our output streams.
	 * @param layer the layer for which the event is being printed.
	 * @return a string representation of the layer.
	 */
	string layerString(const CommunicationLayer::Types& layer) const;

	/**
	 * Return a string representation of the given event to be used
	 * in our output streams.
	 * @param event the event being printed.
	 * @return a string representation of the event.
	 */
	string eventString(const LogEvents& event) const;

	/**
	 * Return a string representation of the given node ID to be used
	 * in our output streams.
	 * @param nodeId the node ID being printed.
	 * @return a string representation of the node ID.
	 */
	string nodeIdString(const NodeId& nodeId) const;

};
typedef LogStreamManager* LogStreamManagerPtr;

/////////////////////////////////////////////////
// Inline Functions
/////////////////////////////////////////////////

inline LogStreamManagerPtr LogStreamManager::instance()
{
	// See the Singleton design pattern for an explanation.
	if(m_instance == 0) {
		m_instance = new LogStreamManager();
	}
	return m_instance;
}

inline void LogStreamManager::setAllStreams(ostreamPtr newStream)
{
	assert(newStream != 0);

	setPktSendStream(newStream);
	setPktRecvStream(newStream);
	setEnergyStream(newStream);
	setNewSenseEventStream(newStream);
	setSensedEventStream(newStream);
	setStatsStream(newStream);
	setUserDefinedStream(newStream);
	setDebugStream(newStream);
}

inline void LogStreamManager::setPktSendStream(ostreamPtr newStream)
{
	assert(newStream != 0);
	m_pktSendStream = newStream;
}

inline void LogStreamManager::setPktRecvStream(ostreamPtr newStream)
{
	assert(newStream != 0);
	m_pktRecvStream = newStream;
}

inline void LogStreamManager::setEnergyStream(ostreamPtr newStream)
{
	assert(newStream != 0);
	m_energyStream = newStream;
}

inline void LogStreamManager::setNewSenseEventStream(ostreamPtr newStream)
{
	assert(newStream != 0);
	m_newSenseEventStream = newStream;
}

inline void LogStreamManager::setSensedEventStream(ostreamPtr newStream)
{
	assert(newStream != 0);
	m_sensedEventStream = newStream;
}

inline void LogStreamManager::setStatsStream(ostreamPtr newStream)
{
	assert(newStream != 0);
	m_statsStream = newStream;
}

inline void LogStreamManager::setUserDefinedStream(ostreamPtr newStream)
{
	assert(newStream != 0);
	m_userDefinedStream = newStream;
}

inline void LogStreamManager::setDebugStream(ostreamPtr newStream)
{
	assert(newStream != 0);
	m_debugStream = newStream;
}

////////////////////////////////////////////////
// Overloaded Operators
/////////////////////////////////////////////////

inline ostream& operator<< (ostream& s, 
		const LogStreamManager::LogEvents& logEvent)
{

	switch (logEvent) {
		case LogStreamManager::LogEvents_PktSend:
			s << "pktSend";
			break;
		case LogStreamManager::LogEvents_PktRecv:
			s << "pktRecv";
			break;
		case LogStreamManager::LogEvents_Stats:
			s << "stats";
			break;
		case LogStreamManager::LogEvents_UserDefined:
			s << "userDefined";
			break;
		case LogStreamManager::LogEvents_Debug:
			s << "debug";
			break;
	}

	return s;

}

#endif // LOG_STREAM_MANAGER_H

