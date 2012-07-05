#pragma once

#include <boost/shared_ptr.hpp>

namespace cf {
namespace ScoreFollower {

class ScoreEvent
{
public:
	virtual ~ScoreEvent() {}

	virtual double GetVelocity() = 0;

	virtual void ApplyVelocity(double velocity) = 0;
};

typedef boost::shared_ptr<ScoreEvent> ScoreEventPtr;

} // namespace ScoreFollower
} // namespace cf
