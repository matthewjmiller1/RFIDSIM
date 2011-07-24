
#include "application_layer.hpp"
#include "simulator.hpp"

ApplicationLayer::ApplicationLayer(NodePtr node)
	: CommunicationLayer(node), m_isRunning(false)
{

}

ApplicationLayer::~ApplicationLayer()
{

}

void ApplicationLayer::start(const SimTime& startTime)
{
	AppEpochEventPtr startEvent = AppEpochEvent::create(
		AppEpochEvent::Epochs_Start, thisApplicationLayer());
	SimTime scheduledTime = startTime - 
		Simulator::instance()->currentTime();
	assert(scheduledTime >= 0.0);
	Simulator::instance()->scheduleEvent(startEvent, scheduledTime);
}

void ApplicationLayer::stop(const SimTime& stopTime)
{
	AppEpochEventPtr stopEvent = AppEpochEvent::create(
		AppEpochEvent::Epochs_Stop, thisApplicationLayer());
	SimTime scheduledTime = stopTime - 
		Simulator::instance()->currentTime();
	assert(scheduledTime >= 0.0);
	Simulator::instance()->scheduleEvent(stopEvent, scheduledTime);
}

bool ApplicationLayer::recvFromLayer(
	CommunicationLayer::Directions direction,
	PacketPtr packet, t_uint recvLayerIdx)
{
	// The app layer should not have an upper communication
	// layer from which to receive.
	assert(direction == CommunicationLayer::Directions_Lower);
	return handleRecvdPacket(packet, recvLayerIdx);

}


