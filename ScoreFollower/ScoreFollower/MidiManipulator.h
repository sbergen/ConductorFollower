#pragma once

namespace cf {
namespace ScoreFollower {

// Interface for manipulating individual midi events
template<typename TData>
class MidiManipulator
{
public:
	virtual void ApplyVelocity(TData & data, double velocity) = 0;

};

} // namespace ScoreFollower
} // namespace cf
