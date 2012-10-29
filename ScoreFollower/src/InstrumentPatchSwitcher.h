#pragma once

#include <utility>
#include <vector>

#include <boost/utility.hpp>

#include "Data/Instrument.h"
#include "PatchMapper/ConductorContext.h"
#include "PatchMapper/InstrumentContext.h"
#include "PatchMapper/SynthesisParameters.h"
#include "ScoreFollower/Follower.h"

namespace cf {
namespace ScoreFollower {

class InstrumentPatchSwitcher : public boost::noncopyable
{
public:
	// Use IgnoreChannel as channel to ignore instrument
	enum { IgnoreChannel = -1 };
	InstrumentPatchSwitcher(Data::Instrument const & instrument, int channel, PatchMapper::ConductorContext const & conductorContext);

	void InsertEventAndPatchSwitchesToBuffer(Follower::BlockBuffer & events, ScoreEventPtr data, samples_t position, double currentSpeed);

private:
	void SwitchPathIfNecessary(Follower::BlockBuffer & events, ScoreEventPtr data, samples_t position, double currentSpeed);

private:
	typedef std::pair<PatchMapper::SynthesisParameters, int> PatchKeyswitchPair;
	class PatchDistance;

private:
	PatchMapper::InstrumentContext instrumentContext_;
	PatchMapper::ConductorContext const & conductorContext_;
	int const channel_;

	int currentPatch_;
	std::vector<PatchKeyswitchPair> patches_;
};

} // namespace ScoreFollower
} // namespace cf