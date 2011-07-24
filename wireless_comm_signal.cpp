
#include "wireless_comm_signal.hpp"
#include "sim_time.hpp"

WirelessCommSignal::WirelessCommSignal(const Location& location,
	double dbStrength, double wavelength, 
	double transmitterGain, PacketPtr packet)
	: Signal(location, dbStrength), m_packetPtr(packet), 
	m_wavelength(wavelength), m_transmitterGain(transmitterGain),
	m_channelId(m_DEFAULT_CHANNEL_ID)
{
	assert(m_wavelength > 0.0);
	assert(m_transmitterGain > 0.0);
	assert(m_packetPtr.get() != 0);
}

WirelessCommSignal::WirelessCommSignal(const WirelessCommSignal& rhs)
	: Signal(rhs), m_wavelength(rhs.m_wavelength), 
	m_transmitterGain(rhs.m_transmitterGain), 
	m_channelId(rhs.m_channelId)
{
	m_packetPtr = Packet::create(*rhs.m_packetPtr);
}

SignalPtr WirelessCommSignal::clone() const
{
	SignalPtr p(new WirelessCommSignal(*this));
	return p;
}

SimTime WirelessCommSignal::getDuration() const
{
	ConstPacketPtr packet = getPacketPtr();
	assert(packet.get() != 0);
	return packet->getDuration();
}


