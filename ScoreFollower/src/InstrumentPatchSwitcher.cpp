#include "InstrumentPatchSwitcher.h"

namespace cf {
namespace ScoreFollower {

InstrumentPatchSwitcher::InstrumentPatchSwitcher(Data::Instrument const & instrument)
{
	auto const & patches = instrument.patches;
	for (auto it = patches.begin(); it != patches.end(); ++it)
	{
		// TODO do some magic here!
	}
}

void
InstrumentPatchSwitcher::InsertEventAndPatchSwitchesToBuffer(Follower::BlockBuffer & events, ScoreEventPtr data, unsigned position)
{
	score_time_t noteLength = data->GetNoteLength();
	double velocity = data->GetVelocity();
	
}

} // namespace ScoreFollower
} // namespace cf
