#include <iostream>
#include <string>
#include <boost/format.hpp>
#include <boost/scoped_ptr.hpp>

#include <boost/thread.hpp>

/*
#include "FeatureExtractor/EventProvider.h"
#include "FeatureExtractor/Event.h"
*/

namespace cf {

//using namespace FeatureExtractor;

int main(int argc, char * argv[])
{
	std::cout << "I don't do anything ATM!" << std::endl;
	return 0;
	/*

	boost::scoped_ptr<EventProvider> eventProvider(EventProvider::Create());

	if (!eventProvider->StartProduction())
	{
		std::cout << "Start failed!" << std::endl;
		return -1;
	}

	while(true)
	{
		Event e;
		while (eventProvider->DequeueEvent(e))
		{
			switch(e.type())
			{
			case Event::TrackingStarted:
				std::cout << "Tracking started" << std::endl;
				break;
			case Event::TrackingEnded:
				std::cout << "Tracking ended" << std::endl;
				return 0;
			case Event::Beat:
				std::cout << "-";
				break;
			}
		}
		
		boost::thread::sleep(boost::get_system_time() + boost::posix_time::milliseconds(10));
	}

	return 0;
	*/
}

} // namespace cf

