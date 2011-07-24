
#include <iostream>
#include <vector>
using namespace std;
#include <boost/random.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include "simulator.hpp"
#include "utility.hpp"

#include "fading.hpp"
#include "path_loss.hpp"
#include "wireless_channel.hpp"
#include "link_layer.hpp"
#include "rfid_reader_phy.hpp"
#include "rfid_reader_mac.hpp"
#include "rfid_reader_app.hpp"
#include "rfid_tag_phy.hpp"
#include "rfid_tag_mac.hpp"
#include "rfid_tag_app.hpp"
#include "packet.hpp"
#include "rand_num_generator.hpp"
//#include "log_stream_manager.hpp"
//#include "signal.hpp"
//#include "wireless_comm_signal.hpp"
//#include "packet.hpp"

void unitTestEventQueue(SimulatorPtr sim);

void packetSendTest();

void randomTest();

//void copyTest(WirelessCommSignal sig);

/*
class Foo {
public:

	static boost::shared_ptr<Foo> create() {
		boost::shared_ptr<Foo> p(new Foo);
		p->m_internal_weak_this = p;
		return p;
	}

	boost::shared_ptr<Foo> get_this()
	{
		boost::shared_ptr<Foo> p(m_internal_weak_this);
		return p;
	}

private:
	Foo() {}
	boost::weak_ptr<Foo> m_internal_weak_this;
};
typedef boost::shared_ptr<Foo> FooPtr;
*/

int main(int argc, char *argv[])
{
	SimulatorPtr s = Simulator::instance();
	DummyEventPtr e1 = DummyEvent::create();
	SimTime st(2.0);
	s->scheduleEvent(e1, st);
	//s->runSimulation();
	s->reset();

	packetSendTest();
	//randomTest();

	/*
	unitTestEventQueue(s);
	*/

}

void packetSendTest()
{

	t_uint currentPowerLevel = 2;
	t_uint numTags = 50;
	t_uint numReaders = 1;
	t_uint numChannels = (numReaders + 1);
	bool doCollocation = false;
	t_uint allChannelId = 0;
	WirelessChannelPtr channels[numChannels];

	if(doCollocation)
		numReaders *= 2;

	ostringstream outputFileName;
	outputFileName << "out" << currentPowerLevel << ".txt";
	ostreamPtr outputStream(new ofstream(outputFileName.str().c_str()));
	LogStreamManager::instance()->setAllStreams(outputStream);

	ostringstream statsFileName;
	statsFileName << "stats" << currentPowerLevel << ".txt";
	ostreamPtr statsStream(new ofstream(statsFileName.str().c_str()));
	LogStreamManager::instance()->setStatsStream(statsStream);

	WirelessChannelManagerPtr channelManager = 
		WirelessChannelManager::create();
	for(t_uint i = 0; i < numChannels; ++i) {
		TwoRayPtr twoRay = TwoRay::create();
		RiceanPtr ricean = Ricean::create();
		//channels[i] = WirelessChannel::create(twoRay, ricean);
		channels[i] = WirelessChannel::create(twoRay);
		channelManager->addChannel(i, channels[i]);
	}

	RandNumGeneratorPtr rand = 
		Simulator::instance()->getRandNumGenerator();

	vector<RfidReaderAppPtr> readerAppVector;
	for(t_uint i = 0; i < numReaders; ++i) {
		double locationStep = 4.8;
		// Nodes cannot be collocated exactly or else the path loss
		// model will end up with a zero in its denominator.  Thus,
		// we add a small epsilon to "collocated" nodes.
		double epsilon = 0.000001;
		Location location((locationStep * i),0,0);
		if(doCollocation) {
			location.setCoordinates(
				(epsilon * i) + (locationStep * floor(i / 2.0)),0,0);
		}
		NodePtr readerNode = Node::create(location, NodeId(i));

		ostringstream userDefinedStream;
		userDefinedStream << "Reader ID: " << readerNode->getNodeId() <<
			" Location: " << readerNode->getLocation();
		LogStreamManager::instance()->logUserDefinedItem(
			userDefinedStream.str());
	
		RfidReaderPhyPtr readerPhy = 
			RfidReaderPhy::create(readerNode, channelManager);
		readerPhy->setAllSendersChannel(allChannelId);
		t_uint channelId = (i + 1);
		// Keep the same channel id for collocated nodes.
		if(doCollocation)
			channelId = static_cast<t_uint>(floor(i / 2.0) + 1);
		assert(channelId < numChannels);
		readerPhy->setRegularChannel(channelId);
	
		RfidReaderAppPtr readerApp = RfidReaderApp::create(
			readerNode, readerPhy);
		// The second collocated nodes should *not* send
		// a reset packet since it is reading the tags
		// that the first collocated node missed.
		if(doCollocation && ((i % 2) != 0)) {
			readerApp->setDoReset(false);
		}

		RfidReaderMacPtr readerMac = RfidReaderMac::create(readerNode, 
			readerApp);
		LinkLayerPtr readerLink = LinkLayer::create(readerNode, readerMac);
	
		readerApp->insertLowerLayer(readerLink);
		readerLink->insertLowerLayer(readerPhy);
	
		readerApp->setNumPowerControlLevels(currentPowerLevel);
		double startTime = 2.5;
		SimTime readerAppStartTime(startTime);
		// Start the collocated nodes a sufficiently long time
		// after the original nodes and stagger the collocated
		// nodes so that they are not reading at the same time.
		if(doCollocation && ((i % 2) != 0)) {
			double staggerStep = floor(i / 2.0);
			readerAppStartTime.setTime((startTime * 4.0) + 
				(staggerStep * startTime));
		}
		readerApp->start(readerAppStartTime);
	}

	vector<RfidTagAppPtr> tagAppVector;
	for(t_uint i = 0; i < numTags; ++i) {
		double locationFactor = numReaders;
		if(doCollocation)
			locationFactor /= 2;
		Location location(rand->uniformReal(0.0, (locationFactor * 2.4)),0,0);
		/*
		Location location(0,0,0.01);
		if(i == 1)
			location.setCoordinates(0,0,0.1);
		*/
		NodePtr tagNode = Node::create(location, NodeId(numReaders+i));

		ostringstream userDefinedStream;
		userDefinedStream << "Tag ID: " << tagNode->getNodeId() <<
			" Location: " << tagNode->getLocation();
		LogStreamManager::instance()->logUserDefinedItem(
			userDefinedStream.str());

		RfidTagPhyPtr tagPhy = RfidTagPhy::create(tagNode, channelManager);
		tagPhy->setAllListenersChannel(allChannelId);
	
		RfidTagAppPtr tagApp = RfidTagApp::create(tagNode);
	
		RfidTagMacPtr tagMac = RfidTagMac::create(tagNode, tagApp);
		LinkLayerPtr tagLink = LinkLayer::create(tagNode, tagMac);
	
		tagApp->insertLowerLayer(tagLink);
		tagLink->insertLowerLayer(tagPhy);
	
		tagApp->start(SimTime(0.0));
	}

	Simulator::instance()->runSimulation(SimTime(20.0));

}

void unitTestEventQueue(SimulatorPtr sim)
{

	vector<double> timeVals;
	timeVals.push_back(0.0);
	timeVals.push_back(2.0);
	timeVals.push_back(5.0);
	timeVals.push_back(1.5);
	timeVals.push_back(1.5);
	timeVals.push_back(1.5);
	timeVals.push_back(5.0);
	timeVals.push_back(10.0);
	timeVals.push_back(1.0);

	cout << "\n";
	vector<EventPtr> eventVals;
	for(t_uint i = 0; i < timeVals.size(); i++) {
		EventPtr newEvent = DummyEvent::create();
		eventVals.push_back(newEvent);
		SimTimePtr newSimTime(new SimTime(timeVals[i]));
		cout << "Adding event " << newEvent << " to fire at time " <<
			*newSimTime << ".\n";
		bool didSchedule = sim->scheduleEvent(newEvent, *newSimTime);
		assert(didSchedule);
	}

	typedef boost::lagged_fibonacci607 BaseGenerator;
	BaseGenerator baseGenerator(2004u);
	boost::uniform_int<> uni_int(0,(eventVals.size() - 1));
	boost::variate_generator<BaseGenerator&, 
		boost::uniform_int<> > rng(baseGenerator, uni_int);
	

	cout << "\n";
	EventPtr eventToRemove(eventVals[rng()]);
	cout << "Cancelling event " << eventToRemove << "\n";
	bool didRemove = sim->cancelEvent(eventToRemove);
	assert(didRemove);

	eventToRemove = eventVals[rng()];
	cout << "Cancelling event " << eventToRemove << "\n";
	didRemove = sim->cancelEvent(eventToRemove);
	assert(didRemove);

	/*
	eventToRemove.reset(new TestEvent());
	didRemove = sim->cancelEvent(eventToRemove);
	// should fail since event was never scheduled
	assert(didRemove);
	*/

	cout << "\n";
	sim->runSimulation(SimTime(100.0));
	sim->reset();
	cout << "\n";

}

void randomTest()
{

	RfidTagAppDataPtr tagAppData = RfidTagAppData::create();
	NodeId testNodeId(1460502);
	tagAppData->setTagId(testNodeId);
	cout << "Set ID: " << testNodeId << ", Retrieved ID: " <<
		tagAppData->getTagId() << endl;

	/*
	int i = 2;
	double d = 6.8;
	d = d - i * floor(d/i);
	d = d/i;
	cout << d << "\n";
	*/

	/*
	RandNumGeneratorPtr r = RandNumGenerator::create();
	cout << r.use_count() << "\n";
	*/

	/*
	FooPtr f = Foo::create();
	FooPtr f2 = f->get_this();
	cout << "f: " << f << ", use_count: " << f.use_count() << "\n";
	cout << "f2: " << f2 << ", use_count: " << f2.use_count() << "\n";
	*/

	/*
	PacketPtr pkt(new Packet);
	Location loc(1, 2, 3);
	WirelessCommSignalPtr sig(new WirelessCommSignal(loc, 1.0, pkt));
	cout << "sig: " << *sig << "\n\n";
	copyTest(*sig);
	*/

	/*
	Location loc;
	Signal sig(loc1, 5.0);
	Location loc2 = sig.getLocation();
	Location loc3(1, 2, 3);
	cout << *loc2 << "\n";
	loc2 = loc3;
	cout << *loc2 << "\n";
	SignalPtr sigp(&sig);
	*/

	/*
	LogStreamManagerPtr lsm = LogStreamManager::instance();
	lsm->setSimulator(s);
	lsm->logUserDefinedItem("Foo bar");
	*/

}

/*
void copyTest(WirelessCommSignal sig)
{
	cout << "foo sig: " << sig << "\n\n";
}
*/


