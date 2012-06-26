#include <iostream>
#include <string>
#include <boost/format.hpp>
#include <boost/scoped_ptr.hpp>

#include <boost/thread.hpp>

#include "MotionTracker/EventProvider.h"
#include "MotionTracker/Event.h"

#include "FeatureExtractor/Extractor.h"

namespace cf {

using namespace FeatureExtractor;
using namespace MotionTracker;

int main(int argc, char * argv[])
{
	boost::scoped_ptr<EventProvider> eventProvider(EventProvider::Create());
	boost::scoped_ptr<Extractor> featureExtractor(Extractor::Create());
	timestamp_t prevBeat = timestamp_t::min();
	Extractor::GestureBuffer buffer(16);

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
			case Event::Position:
				featureExtractor->RegisterPosition(e.timestamp(), e.data<Point3D>());
				break;
			}
		}

		featureExtractor->GetBeatsSince(prevBeat, buffer);
		auto beats = buffer.AllEvents();
		beats.ForEach([&prevBeat](timestamp_t const & timestamp, double data)
		{
			std::cout << timestamp << std::endl;
			prevBeat = timestamp + milliseconds_t(1);
		});

		
		boost::thread::sleep(boost::get_system_time() + boost::posix_time::milliseconds(10));
	}

	return 0;
}

} // namespace cf

