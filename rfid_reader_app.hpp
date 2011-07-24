
#ifndef RFID_READER_APP_H
#define RFID_READER_APP_H

#include <map>
#include <set>
#include <algorithm>
using namespace std;
#include <boost/shared_ptr.hpp>

#include "application_layer.hpp"
#include "packet.hpp"
#include "timer.hpp"
#include "simulator.hpp"

class PhysicalLayer;
typedef boost::shared_ptr<PhysicalLayer> PhysicalLayerPtr;

/**
 * This class keeps track of tag data received.
 */
class ReadTagData {
friend ostream& operator<< (ostream& s, const ReadTagData& readTagData);
public:
	/// A constructor.
	ReadTagData(const NodeId& readTagId, const SimTime& timeRead,
		const SimTime& timeReadSent)
		: m_readTagId(readTagId), m_timeRead(timeRead), 
		m_timeReadSent(timeReadSent) {}

	/**
	 * Get the ID of the tag read.
	 * @return the tag ID.
	 */
	NodeId getReadTagId() const
	{
		return m_readTagId;
	}

	/**
	 * Get the time that the tag was read.
	 * @return the time the tag was read.
	 */
	SimTime getTimeRead() const
	{
		return m_timeRead;
	}

	/**
	 * Get the time that the read packet was sent
	 * for the tag read.
	 * @return the time the read packet was sent.
	 */
	SimTime getTimeReadSent() const
	{
		return m_timeReadSent;
	}

	/**
	 * Get the latency of the read time for the tag.
	 * @return the latency between the time the read
	 * packet was sent and the tag's reply was received.
	 */
	SimTime getReadLatency() const
	{
		return (m_timeRead - m_timeReadSent);
	}

private:
	NodeId m_readTagId;
	SimTime m_timeRead;
	SimTime m_timeReadSent;
};

inline ostream& operator<< (ostream& s, const ReadTagData& readTagData)
{
	return s << "[ tagId=" << readTagData.m_readTagId << 
		", timeRead=" << readTagData.m_timeRead << 
		", timeReadSent=" << readTagData.m_timeReadSent << " ]";
}

/**
 * The RFID reader application class.
 */
class RfidReaderApp : public ApplicationLayer {
friend class RfidReaderAppReadEvent;
public:
	/// Smart pointer that clients should use.
	typedef boost::shared_ptr<RfidReaderApp> RfidReaderAppPtr;
	/// The iterator for tag data which we are storing.
	typedef multimap<t_uint,ReadTagData>::const_iterator TagIterator;

	/// A destructor.
	virtual ~RfidReaderApp();

	/**
	 * A factory method to ensure that all objects
	 * are created via \c new since we are using smart pointers. 
	 */
	static inline RfidReaderAppPtr create(NodePtr node, 
		PhysicalLayerPtr physicalLayer);

	/**
	 * Gives \c this pointer as a \c RfidReaderApp object.
	 * This is used to deal with \c shared_ptr's, \c this, and
	 * inheritance.
	 * @return a pointer to \c this object.
	 */
	virtual inline RfidReaderAppPtr thisRfidReaderApp();

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
	 * The function called when the simulation ends.
	 */
	virtual void simulationEndHandler();

	/**
	 * Set the period with which the read command will be
	 * issued.
	 * @param readPeriod the period of the read time if
	 * repeted reads are done.
	 */
	inline void setReadPeriod(SimTime readPeriod);

	/**
	 * Set whether or not reads are repeated or one-shot.
	 * @param doRepeatedReads true if repeated read
	 * commands are to be issued.
	 */
	inline void setDoRepeatedReads(bool doRepeatedReads);

	/**
	 * Set whether or not a reset packet is send before
	 * beginning the read process.
	 * @param doReset true if a reset packet should be sent.
	 */
	inline void setDoReset(bool doReset);

	/**
	 * Get the next (absolute) time in a cycle that the read 
	 * command is issued.
	 * @return the next (absolute) time in the cycle that the read 
	 * command is issued.  Returns zero if no reads are pending.
	 */
	inline SimTime getNextReadTime() const;

	/**
	 * The period with which the reads are being done.
	 * @return the read period for the cycle (if it is repeated).
	 */
	inline SimTime getReadPeriod() const;

	/**
	 * Whether the application is doing repeated, cyclic reads
	 * or one-shot.
	 * @return true if repeated reads are being done.
	 */
	inline bool getDoRepeatedReads() const;

	/**
	 * Whether the application is sends a reset packet before
	 * the read process.
	 * @return true if reset packets are sent.
	 */
	inline bool getDoReset() const;

	/**
	 * Set the number of power control levels that the application
	 * will use for its reads.
	 * The reader will start with the lowest transmit power level
	 * and continue increasing it on each successive read
	 * command until it reaches the initial power save level
	 * of the physical layer.  The increments of the power
	 * level increase are uniformly spaced.
	 * @param numPowerControlLevels the number of transmit power
	 * levels to be used (must be greater than 0).
	 */
	inline void setNumPowerControlLevels(t_uint numPowerControlLevels);

	/**
	 * Get the number of power control levels that the application
	 * will use for its reads.
	 * @return the number of power save levels being used.
	 * @see setNumPowerControlLevels()
	 */
	inline t_uint getNumPowerControlLevels() const;

	/**
	 * The handler for when the MAC layer finishes a read
	 * request send by the application.
	 * This is required when power control is used so that
	 * the application adjusts the power level to the appropriate
	 * level at the appropriate time.
	 */
	void signalReadEnd();

protected:

	//@{
	/** String for the stats file output. */
	static const string m_TAGS_READ_COUNT_STRING;
	static const string m_TAGS_READ_AT_POWER_COUNT_STRING;
	static const string m_TAGS_READ_AVG_LATENCY_STRING;
	static const string m_TAGS_READ_PROCESS_AVG_LATENCY_STRING;
	static const string m_LAST_TAG_READ_LATENCY_STRING;
	static const string m_TAG_READ_PROCESS_LATENCY_STRING;
	static const string m_TAG_READ_LATENCY_STRING;
	static const string m_TAG_READ_ID_STRING;
	static const string m_TAG_READ_LEVEL_STRING;
	static const string m_TAG_READ_TIME_STRING;
	//@}

	/// The first time a read packet was sent.
	SimTime m_firstReadSentTime;

	/// The previous time a read packet was sent.
	SimTime m_previousReadSentTime;

	/// The data for the tags we read, indexed by the power
	/// level at which it was read.
	multimap<t_uint,ReadTagData> m_readTags;

	/// Keep track of the last tag that was read.
	pair<t_uint,ReadTagData> m_lastTagRead;

	/// A hash to keep track of whether an ID has been seen already.
	set<NodeId> m_readTagIds;

	/// A constructor.
	RfidReaderApp(NodePtr node, PhysicalLayerPtr physicalLayer);

	/**
	 * Handle a received packet.
	 * @param packet a pointer to the received packet.
	 * @param sendingLayerIdx the index of the layer that sent the
	 * packet.
	 * @return true if the packet was handled successfully.
	 */
	bool handleRecvdPacket(PacketPtr packet, t_uint sendingLayerIdx);

	/**
	 * Do the read process for the application.
	 */
	virtual void doReadProcess();

private: 

	/// Controls if debugging info is printed.
	static const bool m_DEBUG_POWER_CONTROL = true;

	/// An internal pointer to the object to allow it
	/// to return \c this.
	boost::weak_ptr<RfidReaderApp> m_weakThis;

	/// The default period for doing repeated reads.
	static const double m_DEFAULT_READ_PERIOD;

	/// The default number of power control levels that we use.
	static const t_uint m_DEFAULT_NUM_POWER_CONTROL_LEVELS;

	/// The physical layer associated with this application.
	PhysicalLayerPtr m_physicalLayer;

	/// A timer for when reads should be done.
	TimerPtr m_readTimer;

	/// The time between reads if repeated reads are done.
	/// @see setReadPeriod()
	/// @see getReadPeriod()
	SimTime m_readPeriod;

	/// Determines whether the read is one-shot or repeated
	/// according to the read period.
	/// @see setDoRepeatedReads()
	/// @see getDoRepeatedReads()
	bool m_doRepeatedReads;

	/// Determines whether a reset packet is sent prior
	/// to do the read process.
	/// @see setDoReset()
	/// @see getDoReset()
	bool m_doReset;

	/// Determines how many power control levels we use.
	t_uint m_numPowerControlLevels;

	/// The maximum transmission power we can use.
	double m_maxTxPower;

	/// The current power control level that will be used in 
	/// the read process.
	t_uint m_currentTxPowerLevel;

	/**
	 * Send a read packet to the tags in range.
	 * @param txPower the power level to use for the packet.
	 * @see Packet::setTxPower()
	 */
	void sendReadPacket(double txPower);

	/**
	 * Send a reset packet to the tags in range.
	 */
	void sendResetPacket();

	/**
	 * Adjusts the power level and sends a read
	 * packet if appropriate.
	 */
	void doNextRead();

};
typedef boost::shared_ptr<RfidReaderApp> RfidReaderAppPtr;

////////////////////////////////////////////////
// Event Subclasses
/////////////////////////////////////////////////

/**
 * The event for when the read process should begin
 * at the application.
 */
class RfidReaderAppReadEvent : public Event {
public:
	/// Smart pointer that clients should use.
	typedef boost::shared_ptr<RfidReaderAppReadEvent> 
		RfidReaderAppReadEventPtr;

	/**
	 * A factory method to ensure that all objects
	 * are created via \c new since we are using smart pointers. 
	 */
	static inline RfidReaderAppReadEventPtr create(
		RfidReaderAppPtr readerApp)
	{
		RfidReaderAppReadEventPtr p(new RfidReaderAppReadEvent(readerApp));
		return p;
	}

	void execute()
	{
		m_readerApp->doReadProcess();
	}

protected:
	/// A constructor.
	RfidReaderAppReadEvent(RfidReaderAppPtr readerApp) 
		: Event()
	{ 
		m_readerApp = readerApp;
	}
private:
	RfidReaderAppPtr m_readerApp;
};
typedef boost::shared_ptr<RfidReaderAppReadEvent> 
	RfidReaderAppReadEventPtr;

////////////////////////////////////////////////
// Back to RfidReaderApp Class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
// Inline Functions
/////////////////////////////////////////////////

inline RfidReaderAppPtr RfidReaderApp::create(NodePtr node, 
	PhysicalLayerPtr physicalLayer)
{
	RfidReaderAppPtr p(new RfidReaderApp(node, physicalLayer));
	// weakThis *must* be set before the thisReaderApp
	// function is called.
	p->m_weakThis = p;
	RfidReaderAppReadEventPtr readEvent = 
		RfidReaderAppReadEvent::create(p->thisRfidReaderApp());
	p->m_readTimer = Timer::create(p->getNode(), readEvent);

	Simulator::instance()->addSimulationEndListener(
		p->thisSimulationEndListener());

	return p;
}

inline RfidReaderAppPtr RfidReaderApp::thisRfidReaderApp()
{
	RfidReaderAppPtr p(m_weakThis);
	return p;
}

inline ApplicationLayerPtr RfidReaderApp::thisApplicationLayer()
{
	ApplicationLayerPtr p(m_weakThis);
	return p;
}

inline SimulationEndListenerPtr RfidReaderApp::thisSimulationEndListener()
{
	SimulationEndListenerPtr p(m_weakThis);
	return p;
}
inline void RfidReaderApp::setReadPeriod(SimTime readPeriod)
{
	m_readPeriod = readPeriod;
}

inline void RfidReaderApp::setDoRepeatedReads(bool doRepeatedReads)
{
	m_doRepeatedReads = doRepeatedReads;
}

inline void RfidReaderApp::setDoReset(bool doReset)
{
	m_doReset = doReset;
}

inline SimTime RfidReaderApp::getNextReadTime() const
{
	SimTime nextReadTime(0.0);
	if(m_readTimer->isRunning()) {
		nextReadTime = (m_readTimer->timeRemaining() + 
			Simulator::instance()->currentTime());
	}
	return nextReadTime;
}

inline SimTime RfidReaderApp::getReadPeriod() const
{
	return m_readPeriod;
}

inline bool RfidReaderApp::getDoRepeatedReads() const
{
	return m_doRepeatedReads;
}

inline bool RfidReaderApp::getDoReset() const
{
	return m_doReset;
}

inline void RfidReaderApp::setNumPowerControlLevels(
	t_uint numPowerControlLevels)
{
	assert(numPowerControlLevels > 0);
	m_numPowerControlLevels = numPowerControlLevels;
}

inline t_uint RfidReaderApp::getNumPowerControlLevels() const
{
	return m_numPowerControlLevels;
}

////////////////////////////////////////////////
// Overloaded Operators
/////////////////////////////////////////////////

////////////////////////////////////////////////
// PacketData Subclass
/////////////////////////////////////////////////

/**
 * The representation of the application data in an RfidReaderApp's
 * data packet.
 */
class RfidReaderAppData : public PacketData {
public:
	/// Smart pointer that clients should use.
	typedef boost::shared_ptr<RfidReaderAppData> RfidReaderAppDataPtr;

	/**
	 * Types enum.
	 * Specifies the types of packets that are sent.
	 */
	enum Types {
		Types_NoType, /**< enum value Types_NoType. */
		Types_Read, /**< enum value Types_Read. */
		Types_Reset /**< enum value Types_Reset. */
	};

	/**
	 * A factory method to ensure that all objects
	 * are created via \c new since we are using smart pointers. 
	 */
	static inline RfidReaderAppDataPtr create();

	/**
	 * A factory method to ensure that all objects
	 * are created via \c new since we are using smart pointers. 
	 */
	static inline RfidReaderAppDataPtr create(
		const RfidReaderAppData& rhs);

	/**
	 * Get the size of this data.
	 * @return the size of the data in bytes.
	 */
	virtual inline t_uint getSizeInBytes() const;

	/**
	 * Write the node ID to the packet data.
	 * @param nodeId the ID to be written.
	 */
	void setReaderId(const NodeId& nodeId);

	/**
	 * Read the node ID from the packet data.
	 * @return the ID of the node in the packet.
	 */
	NodeId getReaderId() const;

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
	 * Sets whether the entire read cycle
	 * must be done or if the read cycle can end
	 * early in response to several consecutive
	 * unsuccessful contention intervals.
	 * @param doEntireReadCycle determines if the entire read
	 * cycle must be performed.
	 * @see getDoEntireReadCycle()
	 */
	inline void setDoEntireReadCycle(bool doEntireReadCycle);

	/**
	 * Gets whether the entire read cycle
	 * must be done or if the read cycle can end
	 * early in response to several consecutive
	 * unsuccessful contention intervals.
	 * @return true if the entire read
	 * cycle must be performed.
	 * @see setDoEntireReadCycle()
	 */
	inline bool getDoEntireReadCycle() const;

	/**
	 * Return the stream representation of the object.
	 */
	inline ostream& print(ostream& s) const;

protected:

	/// A constructor.
	RfidReaderAppData();

	/// A copy constructor.
	RfidReaderAppData(const RfidReaderAppData& rhs);

	/**
	 * Returns a pointer to a deep copy of this object.
	 * This is addresses the slicing problem with
	 * copy construction.
	 * @return a pointer to a deep copy of this object.
	 */
	virtual PacketDataPtr clone() const;

private:

	/// The default number of bytes for a reader ID.
	static const t_uint m_nodeIdBytes = 4;

	/// The default number of bytes for a packet type.
	static const t_uint m_typeBytes = 1;

	/// The data field for the node ID.
	t_uchar m_nodeId[m_nodeIdBytes];

	/// The type field for the packet.
	Types m_type;

	/// Determines whether the entire read cycle
	/// must be done or if the read cycle can end
	/// early in response to several consecutive
	/// unsuccessful contention intervals.
	bool m_doEntireReadCycle;

};
typedef boost::shared_ptr<RfidReaderAppData> RfidReaderAppDataPtr;

/////////////////////////////////////////////////
// Inline Functions
/////////////////////////////////////////////////

inline RfidReaderAppDataPtr RfidReaderAppData::create()
{
	RfidReaderAppDataPtr p(new RfidReaderAppData());
	return p;
}

inline RfidReaderAppDataPtr RfidReaderAppData::create(
	const RfidReaderAppData& rhs)
{
	RfidReaderAppDataPtr p = 
		boost::dynamic_pointer_cast<RfidReaderAppData>(rhs.clone());
	// If the shared_ptr is empty, then the cast failed.
	assert(p.get() != 0);
	return p;
}

inline t_uint RfidReaderAppData::getSizeInBytes() const
{
	t_uint byteCount = (m_nodeIdBytes + m_typeBytes);

   switch(getType()) {
	default:
		break;
	}

	return byteCount;
}

inline void RfidReaderAppData::setType(RfidReaderAppData::Types type)
{
	m_type = type;
}

inline RfidReaderAppData::Types RfidReaderAppData::getType() const
{
	return m_type;
}

inline void RfidReaderAppData::setDoEntireReadCycle(
	bool doEntireReadCycle)
{
	m_doEntireReadCycle = doEntireReadCycle;
}

inline bool RfidReaderAppData::getDoEntireReadCycle() const
{
	return m_doEntireReadCycle;
}

/////////////////////////////////////////////////
// Overloaded Operators
/////////////////////////////////////////////////

inline ostream& operator<< (ostream& s, 
		const RfidReaderAppData::Types& dataType)
{
	switch(dataType) {
	case RfidReaderAppData::Types_NoType:
		s << "NO_TYPE";
		break;
	case RfidReaderAppData::Types_Read:
		s << "READ";
		break;
	case RfidReaderAppData::Types_Reset:
		s << "RESET";
		break;
	}
	return s;
}

inline ostream& RfidReaderAppData::print(ostream& s) const
{
	s << "type=" << m_type << ", " <<
		"nodeId=" << NodeId(m_nodeId, m_nodeIdBytes);
	return s;
}

#endif // RFID_READER_APP_H

