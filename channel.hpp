
#ifndef CHANNEL_H
#define CHANNEL_H

#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

class Signal;
typedef boost::shared_ptr<Signal> SignalPtr;
class PhysicalLayer;
typedef boost::shared_ptr<PhysicalLayer> PhysicalLayerPtr;
class SimTime;

/**
 * A channel class is one over which a signal travels
 * according to some path loss model.
 * In the future, this could be the superclass for sensed
 * events as well (e.g., temperature, light), but for now
 * it is just use for the wireless communication channel.
 */
class Channel : boost::noncopyable {
public:
	/// Smart pointer that clients should use.
	typedef boost::shared_ptr<Channel> ChannelPtr;
	
	/// A destructor
	virtual ~Channel();

	/**
	 * Compute the propagation delay between two physical
	 * layer objects.
	 * @param sender the sending physical layer.
	 * @param receiver the receving physical layer.
	 * @return the propagation delay between the two objects.
	 */
	virtual SimTime propagationDelay(const PhysicalLayer& sender,
		const PhysicalLayer& receiver) const;

protected:

	/// A constructor.
	/// This is protected to ensure that all objects
	/// are created via \c new since we are using smart pointers. 
	Channel();

	/// A copy constructor.
	/// This is protected to ensure that all objects
	/// are created via \c new since we are using smart pointers. 
	Channel(const Channel& rhs);

private:

	Channel& operator= (const Channel& rhs);

};
typedef boost::shared_ptr<Channel> ChannelPtr;

/////////////////////////////////////////////////
// Inline Functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
// Overloaded Operators
/////////////////////////////////////////////////

#endif // CHANNEL_H

