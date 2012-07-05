#pragma once

#include <boost/utility.hpp>

#include "Data/Instrument.h"

#include "ScoreFollower/Follower.h"

namespace cf {
namespace ScoreFollower {

class InstrumentPatchSwitcher : public boost::noncopyable
{
public:
	InstrumentPatchSwitcher(Data::Instrument const & instrument);

	void InsertEventAndPatchSwitchesToBuffer(Follower::BlockBuffer & events, ScoreEventPtr data, unsigned position);

private:
	int currentPatch_;
};

} // namespace ScoreFollower
} // namespace cf