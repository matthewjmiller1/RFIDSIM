
#include "signal.hpp"

Signal::Signal(const Location& location, double dbStrength)
	: m_location(location), m_dbStrength(dbStrength)
{

}

Signal::Signal(const Signal& rhs)
	: m_location(rhs.m_location), m_dbStrength(rhs.m_dbStrength)
{

}

Signal::~Signal()
{

}

SignalPtr Signal::clone() const
{
	SignalPtr p(new Signal(*this));
	return p;
}


