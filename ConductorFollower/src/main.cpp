#include <iostream>
#include <string>
#include <boost/format.hpp>
#include <boost/scoped_ptr.hpp>

#include "MotionTracker/HandTracker.h"
#include "MotionTracker/DebugHandObserver.h"
#include "FeatureExtractor/FeatureExtractor.h"

namespace cf {

using namespace MotionTracker;

int main(int argc, char * argv[])
{
	boost::scoped_ptr<HandTracker> tracker(HandTracker::Create());

	if (!tracker->Init())
	{
		std::cout << "Failed to init!" << std::endl;
		return -1;
	}

	//Hand hand;
	//DebugHandObserver hand("foo");
	FeatureExtractor::FeatureExtractor hand;
	tracker->StartTrackingHand(GestureWave, hand);

	while(tracker->WaitForData()) {}

	return 0;
}

} // namespace cf

