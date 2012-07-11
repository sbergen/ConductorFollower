#pragma once

#include <utility>
#include <vector>

#include <boost/utility.hpp>

#include "Data/Instrument.h"
#include "PatchMapper/InstrumentContext.h"
#include "PatchMapper/SynthesisParameters.h"
#include "ScoreFollower/Follower.h"

namespace cf {
namespace ScoreFollower {

class InstrumentPatchSwitcher : public boost::noncopyable
{
public:
	InstrumentPatchSwitcher(Data::Instrument const & instrument);

	void InsertEventAndPatchSwitchesToBuffer(Follower::BlockBuffer & events, ScoreEventPtr data, unsigned position, double currentSpeed);

private:
	void SwitchPathIfNecessary(Follower::BlockBuffer & events, ScoreEventPtr data, unsigned position, double currentSpeed);

private:
	typedef std::pair<PatchMapper::SynthesisParameters, int> PatchKeyswitchPair;
	class PatchDistance;

private:
	PatchMapper::InstrumentContext instrumentContext_;

	int currentPatch_;
	std::vector<PatchKeyswitchPair> patches_;
};

} // namespace ScoreFollower
} // namespace cf