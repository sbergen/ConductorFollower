#include "FeatureExtractor.h"

namespace cf {
namespace FeatureExtractor {


FeatureExtractor::FeatureExtractor()
	: positionBuffer_(100)
	, dimExtractor_(positionBuffer_)
	, speedExtractor_(positionBuffer_)
	, eventBuffer_(1000)
{
	trackerThread_.reset(new LockfreeThread(
		boost::bind(&FeatureExtractor::Init, this),
		boost::bind(&FeatureExtractor::EventLoop, this),
		boost::bind(&FeatureExtractor::Cleanup, this)));
}

FeatureExtractor::~FeatureExtractor()
{
	// Stop the thread before anything else gets destructed
	trackerThread_.reset();
}

bool FeatureExtractor::StartProduction()
{
	return trackerThread_->RequestStart();
}

bool FeatureExtractor::StopProduction()
{
	return trackerThread_->RequestStop();
}

bool FeatureExtractor::DequeueEvent(Event & result)
{
	return eventBuffer_.dequeue(result);
}

void
FeatureExtractor::HandFound()
{
	eventBuffer_.enqueue(Event(time::now(), Event::TrackingStarted));
}

void
FeatureExtractor::HandLost()
{
	eventBuffer_.enqueue(Event(time::now(), Event::TrackingEnded));
}

void
FeatureExtractor::NewHandPosition(float time, Point3D const & pos)
{
	positionBuffer_.RegisterEvent(time::now(), pos);
	dimExtractor_.CalculateStuff(eventBuffer_);
	speedExtractor_.CalculateStuff(eventBuffer_);
}

bool
FeatureExtractor::Init()
{
	tracker_.reset(MotionTracker::HandTracker::Create());

	if (!tracker_->Init())
	{
		std::cout << "Failed to init!" << std::endl;
		return false;
	}

	return tracker_->StartTrackingHand(MotionTracker::GestureWave, *this);
}

bool
FeatureExtractor::EventLoop()
{
	return tracker_->WaitForData();
}

void
FeatureExtractor::Cleanup()
{
	tracker_.reset();
}

} // namespace FeatureExtractor
} // namespace cf
