#include "ScoreHelper.h"

#include <boost/bind.hpp>

#include "ScoreFollower/ScoreReader.h"
#include "ScoreFollower/TrackReader.h"

#include "TimeHelper.h"

namespace cf {
namespace ScoreFollower {

ScoreHelper::ScoreHelper(boost::shared_ptr<TimeHelper> timeHelper, PatchMapper::ConductorContext const & conductorContext)
	: timeHelper_(timeHelper)
	, conductorContext_(conductorContext)
	, velocity_(0.5)
{}

void
ScoreHelper::LearnScore(boost::shared_ptr<ScoreReader> scoreReader)
{
	LOG("Reading %1% tracks of midi", (int)scoreReader->TrackCount());

	// Keep reference to this
	scoreReader_ = scoreReader;

	// Read data
	trackBuffers_.clear();
	score_time_t timestamp;
	ScoreEventPtr data;
	for (int i = 0; i < scoreReader->TrackCount(); ++i) {
		trackBuffers_.push_back(new TrackBuffer(0));
		auto reader = scoreReader->Track(i);
		while (reader->NextEvent(timestamp, data)) {
			trackBuffers_[i].RegisterEvent(timestamp, data);
		}
	}
}

void
ScoreHelper::LearnInstruments(Data::InstrumentMap const & instruments, Data::TrackList const & tracks)
{
	LOG("Learning instruments for %1% tracks", (int)tracks.size());

	trackInstruments_.clear();
	trackInstruments_.reserve(tracks.size());

	for(auto it = tracks.begin(); it != tracks.end(); ++it) {
		auto instrumentIt = instruments.find(it->instrument);
		if (instrumentIt == instruments.end()) {
			throw std::runtime_error("Instrument not found!");	
		}

		trackInstruments_.push_back(new InstrumentPatchSwitcher(instrumentIt->second, conductorContext_));
	}
}

void
ScoreHelper::GetTrackEventsForBlock(unsigned track, Follower::BlockBuffer & events)
{
	int instrument = track - 1;

	assert(track < trackBuffers_.size());
	assert(instrument >= 0);

	// If this channel is not mapped, ignore it...
	if (instrument >= trackInstruments_.size()) { return; }

	auto scoreRange = timeHelper_->CurrentScoreTimeBlock();
	auto ev = trackBuffers_[track].EventsBetween(scoreRange.first, scoreRange.second);
	auto & switcher = trackInstruments_[instrument];

	ev.ForEach([&, this](score_time_t const & time, ScoreEventPtr data)
		{
			CopyEventToBuffer(time, data, events, switcher);
		});
}

void
ScoreHelper::CopyEventToBuffer(score_time_t const & time, ScoreEventPtr data,
	Follower::BlockBuffer & events, InstrumentPatchSwitcher & patchSwitcher) const
{
	samples_t frameOffset = timeHelper_->ScoreTimeToFrameOffset(time);
	double velocity = NewVelocityAt(data->GetVelocity(), time);
	
	data->ApplyVelocity(velocity);
	patchSwitcher.InsertEventAndPatchSwitchesToBuffer(events, data, frameOffset, timeHelper_->CurrentSpeed());
}

double
ScoreHelper::NewVelocityAt(double oldVelocity, score_time_t const & time) const
{
	// TODO use time and something fancier :)
	auto velocity = (oldVelocity + (velocity_ - 0.5));
	velocity = std::max(velocity, 0.05);
	velocity = std::min(velocity, 1.00);
	return velocity;
}
} // namespace ScoreFollower
} // namespace cf