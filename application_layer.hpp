
#ifndef APPLICATION_LAYER_H
#define APPLICATION_LAYER_H

#include <boost/shared_ptr.hpp>

#include "communication_layer.hpp"

/**
 * The application layer of the network stack.
 * This is the superclass from which application classes
 * should be derived.
 */
class ApplicationLayer : public CommunicationLayer {
friend class AppEpochEvent;
public:
	/// Smart pointer that clients should use.
	typedef boost::shared_ptr<ApplicationLayer> ApplicationLayerPtr;

	/// A destructor.
	/// A virtual destructor is recommended since the class
	/// has virtual functions.
	virtual ~ApplicationLayer();

	/**
	 * Gives \c this pointer as a \c ApplicationLayer object.
	 * This is used to deal with \c shared_ptr's, \c this, and
	 * inheritance.
	 * @return a pointer to \c this object.
	 */
	virtual ApplicationLayerPtr thisApplicationLayer() = 0;

	/**
	 * Start the application running.
	 * @param startTime the time to start the application.
	 */
	void start(const SimTime& startTime);

	/**
	 * Stop the application running.
	 * @param stopTime the time stop the application.
	 */
	void stop(const SimTime& stopTime);

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
	 * Get the layer type for an object.
	 * @return the type of the layer.
	 */
	inline CommunicationLayer::Types getLayerType() const;

protected:

	/// A constructor.
	ApplicationLayer(NodePtr node);

	/**
	 * Let each subclass handle a received packet in their
	 * own specific manner.
	 * @param packet a pointer to the received packet.
	 * @param sendingLayerIdx the index of the layer that sent the
	 * packet.
	 * @return true if the packet was handled successfully.
	 */
	virtual bool handleRecvdPacket(PacketPtr packet, 
		t_uint sendingLayerIdx) = 0;

	/**
	 * Function to execute when the application is started.
	 */
	virtual void startHandler() = 0;

	/**
	 * Function to execute when the application is stopped.
	 */
	virtual void stopHandler() = 0;

	/// Keeps track of whether the application is running.
	/// @see start()
	/// @see stop()
	bool m_isRunning;

private: 

};
typedef boost::shared_ptr<ApplicationLayer> ApplicationLayerPtr;

/////////////////////////////////////////////////
// Inline Functions
/////////////////////////////////////////////////

inline CommunicationLayer::Types ApplicationLayer::getLayerType() const
{
	return CommunicationLayer::Types_Application;
}

////////////////////////////////////////////////
// Overloaded Operators
/////////////////////////////////////////////////

////////////////////////////////////////////////
// Event Subclasses
/////////////////////////////////////////////////

/**
 * An event subclass for when an application begins
 * and ends.
 * This is the event that starts and ends an application
 * at a node.
 */
class AppEpochEvent : public Event {
public:
	/// Smart pointer that clients should use.
	typedef boost::shared_ptr<AppEpochEvent> AppEpochEventPtr;

	/**
	 * Epochs enum.
	 * Specifies the type of epochs possible.
	 */
	enum Epochs {
		Epochs_Start, /**< enum value Epochs_Start. */
		Epochs_Stop /**< enum value Epochs_Stop. */
	};

	/**
	 * A factory method to ensure that all objects
	 * are created via \c new since we are using smart pointers. 
	 */
	static inline AppEpochEventPtr create(Epochs epochType,
		ApplicationLayerPtr appLayer)
	{
		AppEpochEventPtr p(new AppEpochEvent(epochType, appLayer));
		return p;
	}

	void execute()
	{
		switch(m_epochType) {
		case Epochs_Start:
			m_appLayer->m_isRunning = true;
			m_appLayer->startHandler();
			break;
		case Epochs_Stop:
			m_appLayer->stopHandler();
			m_appLayer->m_isRunning = false;
			break;
		default:
			assert(0);
		}
	}

protected:

	/// A constructor.
	AppEpochEvent(Epochs epochType, ApplicationLayerPtr appLayer) 
		: Event()
	{ 
		m_epochType = epochType;
		m_appLayer = appLayer;
	}

private:

	Epochs m_epochType;
	ApplicationLayerPtr m_appLayer;

};
typedef boost::shared_ptr<AppEpochEvent> AppEpochEventPtr;

#endif // APPLICATION_LAYER_H

