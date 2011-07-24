
#include "wireless_channel.hpp"
#include "path_loss.hpp"
#include "fading.hpp"
#include "wireless_comm_signal.hpp"
#include "log_stream_manager.hpp"
#include "physical_layer.hpp"

WirelessChannel::WirelessChannel(PathLossPtr pathLossModel)
{
	assert(pathLossModel.get() != 0);
	m_pathLossModel = pathLossModel;
}

WirelessChannel::WirelessChannel(PathLossPtr pathLossModel, 
	FadingPtr fadingModel)
{
	assert(pathLossModel.get() != 0);
	assert(fadingModel.get() != 0);
	m_pathLossModel = pathLossModel;
	m_fadingModel = fadingModel;
}

double WirelessChannel::getRecvdStrength(const WirelessCommSignal& signal,
	const PhysicalLayer& receiver) const
{
	assert(m_pathLossModel.get() != 0);
	double recvdStrength = 
		m_pathLossModel->getRecvdStrength(signal, receiver);

	double debugRecvdStrength = recvdStrength;

	// Account for fading if such a model exists
	if(m_fadingModel.get() != 0) {
		recvdStrength *= m_fadingModel->fadingFactor(signal, 
			receiver.getNodeId());
	}

	if(m_DEBUG_SIGNAL_STRENGTH) {
		ostringstream debugStream;
		debugStream << __FUNCTION__ << " recvStrength1: " <<
			debugRecvdStrength;
		debugStream << " recvStrength2: " << recvdStrength;
		LogStreamManager::instance()->logDebugItem(debugStream.str());
	}

	return recvdStrength;
}

bool WirelessChannel::signalHasError(double signalSinr,
	const WirelessCommSignal& signal) const
{
	// This is just a placeholder function for now.
	// In the future, there should be a PacketLoss object
	// contained in WirelessChannel that computes this
	// based on some model.
	return false;
}


