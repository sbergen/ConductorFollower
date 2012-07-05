#pragma once

#include <boost/shared_ptr.hpp>

#include "ScoreFollower/types.h"

namespace cf {
namespace ScoreFollower {

class ScoreEvent;
typedef boost::shared_ptr<ScoreEvent> ScoreEventPtr;

class ScoreEvent
{
public:
	virtual ~ScoreEvent() {}

	virtual double GetVelocity() = 0;
	virtual score_time_t GetNoteLength() = 0;

	virtual void ApplyVelocity(double velocity) = 0;

	// These would normally be static, but since they are virtual...
	virtual ScoreEventPtr MakeKeyswitch(int note) = 0;
};

} // namespace ScoreFollower
} // namespace cf
