
#include "node.hpp"
#include "simulator.hpp"

Node::Node(const Location& location, const NodeId& nodeId)
	: m_location(location), m_nodeId(nodeId)
{

}

/*
Node::Node(const Node& rhs)
	: m_location(rhs.m_location), m_nodeId(rhs.m_nodeId)
{
	
}
*/

Node::~Node()
{

}

/*
NodePtr Node::clone() const
{
	NodePtr p(new Node(*this));
	return p;
}
*/

SimTime Node::currentTime() const
{
	// If local clock drift is desired, it could be added
	// here.
	return Simulator::instance()->currentTime();
}

bool Node::scheduleEvent(EventPtr eventToSchedule,
	const SimTime& eventDelay)
{
	return Simulator::instance()->scheduleEvent(
		eventToSchedule, eventDelay);
}

bool Node::cancelEvent(EventPtr eventToCancel)
{
	return Simulator::instance()->cancelEvent(eventToCancel);
}


