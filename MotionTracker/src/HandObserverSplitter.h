#pragma once

#include "HandObserver.h"
#include "Point3D.h"

#include <list>
#include <functional>

#include <boost/shared_ptr.hpp>

namespace cf {
namespace MotionTracker {

class HandObserverSplitter : public HandObserver
{
public:
	typedef boost::shared_ptr<HandObserver> ObserverPtr;

	void AddObserver(ObserverPtr observer) { list_.push_back(observer); }
	void AddObserverAndTakeOwnership(HandObserver * observer) { list_.push_back(ObserverPtr(observer)); }

public: // HandObserver implementation
	void HandFound()
	{ std::for_each(list_.begin(), list_.end(), [](ObserverPtr o) { o->HandFound(); }); }
	
	void HandLost()
	{ std::for_each(list_.begin(), list_.end(), [](ObserverPtr o) { o->HandLost(); }); }

	void NewHandPosition(float time, Point3D const & pos)
	{ std::for_each(list_.begin(), list_.end(), [time, pos](ObserverPtr o) { o->NewHandPosition(time, pos); }); }

private:
	std::list<ObserverPtr> list_;
};

} // namespace MotionTracker
} // namespace cf
