
#include <boost/python.hpp>
#include "sim_time.hpp"

using namespace boost::python;

BOOST_PYTHON_MODULE(sim_time)
{

	class_<SimTime>("SimTime",init<double>())
		.def("setTimeInSeconds", &SimTime::setTimeInSeconds)
		.def("getTimeInMilliSeconds", &SimTime::getTimeInMilliSeconds)
	;
}

