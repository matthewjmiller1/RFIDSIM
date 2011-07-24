
#include "path_loss.hpp"
#include "utility.hpp"
#include "wireless_comm_signal.hpp"
#include "physical_layer.hpp"

const double FreeSpace::m_DEFAULT_LOSS_FACTOR = 1.0;
const double TwoRay::m_DEFAULT_ANTENNA_HEIGHT = 1.5;

PathLoss::PathLoss()
{

}

PathLoss::~PathLoss()
{

}

FreeSpace::FreeSpace()
	: m_lossFactor(m_DEFAULT_LOSS_FACTOR)
{
	assert(m_lossFactor >= 1.0);
}

FreeSpace::FreeSpace(double lossFactor)
	: m_lossFactor(lossFactor)
{
	if(m_lossFactor < 1.0)
		m_lossFactor = m_DEFAULT_LOSS_FACTOR;
	assert(m_lossFactor >= 1.0);
}

TwoRay::TwoRay()
	: FreeSpace(), m_antennaHeight(m_DEFAULT_ANTENNA_HEIGHT)
{

}

TwoRay::TwoRay(double lossFactor)
	: FreeSpace(lossFactor), m_antennaHeight(m_DEFAULT_ANTENNA_HEIGHT)
{

}

double FreeSpace::getRecvdStrength(const WirelessCommSignal& signal,
	const PhysicalLayer& receiver) const
{
	double numerator = decibelsToPower(signal.getDbStrength()) * 
		signal.getTransmitterGain() * receiver.getGain() * 
		pow(signal.getWavelength(), 2);
	double distance = Location::distance(
		signal.getLocation(), receiver.getLocation());
	double denominator = pow((4.0 * PI), 2) * 
		pow(distance, 2) * m_lossFactor;

	assert(denominator > 0.0);

	return (numerator / denominator);
}

double TwoRay::getRecvdStrength(const WirelessCommSignal& signal,
	const PhysicalLayer& receiver) const
{

	// These equations are largely from ns-2.  If the distance
	// is less than crossoverDistance, then use the FreeSpace
	// model; otherwise use the TwoRay model.

	double crossoverDistance = (4 * PI * 
		m_antennaHeight * m_antennaHeight) / signal.getWavelength();
	double distance = Location::distance(
		signal.getLocation(), receiver.getLocation());

	double recvdStrength = FreeSpace::getRecvdStrength(signal, receiver);

	if(m_DEBUG_SIGNAL_STRENGTH) {
		ostringstream debugStream;
		debugStream << __FUNCTION__ << " crossover: " <<
			crossoverDistance << ", dist: " << distance <<
			", FS SS: " << recvdStrength;
		LogStreamManager::instance()->logDebugItem(debugStream.str());
	}

	// If the distance is greater, use the Two Ray equation.
	if(distance > crossoverDistance) {
		double numerator = decibelsToPower(signal.getDbStrength()) *
			signal.getTransmitterGain() * receiver.getGain() *
			pow(m_antennaHeight, 2) * pow(m_antennaHeight, 2);
		double denominator = pow(distance, 4) * m_lossFactor;
		assert(denominator > 0.0);
		recvdStrength = numerator / denominator;
	}
	
	return recvdStrength;
}


