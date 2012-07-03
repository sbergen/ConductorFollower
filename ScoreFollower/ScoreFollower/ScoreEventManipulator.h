#pragma once

namespace cf {
namespace ScoreFollower {

class ScoreEventHandle;

// Interface for manipulating ScoreEvents via ScoreEventHandle
class ScoreEventManipulator
{
public:
	virtual ~ScoreEventManipulator() {}

	virtual double GetVelocity(ScoreEventHandle const & ev) = 0;
	virtual void ApplyVelocity(ScoreEventHandle & ev, double velocity) = 0;
};

} // namespace ScoreFollower
} // namespace cf
