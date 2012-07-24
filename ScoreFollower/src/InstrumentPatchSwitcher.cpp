#include "InstrumentPatchSwitcher.h"

#include "cf/algorithm.h"
#include "cf/globals.h"

#include "PatchMapper/SynthesisMapping.h"

namespace cf {
namespace ScoreFollower {

class InstrumentPatchSwitcher::PatchDistance
{
public:
	double operator() (PatchKeyswitchPair const & pair, PatchMapper::SynthesisParameters const & params)
	{
		return PatchMapper::ComparableDistance(pair.first, params);
	}
};

InstrumentPatchSwitcher::InstrumentPatchSwitcher(Data::Instrument const & instrument)
	: instrumentContext_(instrument)
	, currentPatch_(-1)
{
	auto const & patches = instrument.patches;
	std::transform(patches.begin(), patches.end(), std::back_inserter(patches_),
		[](Data::InstrumentPatch const & patch)
		{
			return std::make_pair(PatchMapper::SynthesisParameters(patch), patch.keyswitch);
		});
}

void
InstrumentPatchSwitcher::InsertEventAndPatchSwitchesToBuffer(Follower::BlockBuffer & events, ScoreEventPtr data, samples_t position, double currentSpeed)
{
	if (data->IsNoteOn()) {
		SwitchPathIfNecessary(events, data, position, currentSpeed);
	}
	events.RegisterEvent(position, data);
}

void
InstrumentPatchSwitcher::SwitchPathIfNecessary(Follower::BlockBuffer & events, ScoreEventPtr data, samples_t position, double currentSpeed)
{
	if (patches_.empty()) { return; }

	PatchMapper::NoteContext noteContext(data->GetNoteLength(), currentSpeed, data->GetVelocity());
	auto targetParams = PatchMapper::SynthParametersFromContexts(instrumentContext_, noteContext);
	auto best = nearest_neighbour_linear(patches_.begin(), patches_.end(), targetParams, PatchDistance());

	assert(best != patches_.end());
	int patch = best->second;

	if (patch != currentPatch_) {
		currentPatch_ = patch;
		events.RegisterEvent(position, data->MakeKeyswitch(currentPatch_));
	}
}

} // namespace ScoreFollower
} // namespace cf
