#include "TempoFollower.h"

#include <boost/bind.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/units/systems/si/prefixes.hpp>

#include "cf/globals.h"
#include "cf/math.h"

#include "ScoreFollower/FollowerStatus.h"
#include "ScoreFollower/FollowerOptions.h"

#include "TimeWarper.h"
#include "FollowerImpl.h"
#include "ProgressFollowingBeatClassifier.h"
#include "PatternMatchingBeatClassifier.h"

#include "TempoFollowerScoreEventBuilder.h"

#define DEBUG_TEMPO_FOLLOWER 1

namespace cf {
namespace ScoreFollower {

namespace si = boost::units::si;

TempoFollower::TempoFollower(TimeWarper const & timeWarper, FollowerImpl & parent)
	: timeWarper_(timeWarper)
	, parent_(parent)
	, tempoMap_()
	, startTempoEstimator_()
	//, beatClassifier_(new ProgressFollowingBeatClassifier(tempoMap_))
	, beatClassifier_(new PatternMatchingBeatClassifier(tempoMap_))
	, tempoFilter_(32, 2.5 * si::seconds)
{
	beatClassifier_->SetClassificationCallback(boost::bind(&TempoFollower::BeatClassified, this, _1));
}

void
TempoFollower::ReadScore(ScoreReader & reader)
{
	tempoMap_.ReadScore(reader);
	auto start = tempoMap_.GetScorePositionAt(0.0 * score::seconds);
	startTempoEstimator_.SetStartTempo(start.tempo());
	previousScoreTempo_ = start.tempo();
}

void
TempoFollower::LearnScoreEvents(Data::ScoreEventList const & events)
{
	tempoSensitivities_.Clear();

	// Collect all events
	ScoreEventBuilder visitor(*this);
	std::for_each(std::begin(events), std::end(events),
		[visitor](Data::ScoreEvent const & event)
		{
			boost::apply_visitor(visitor, event);
		});
}

void
TempoFollower::RegisterStartGesture(MotionTracker::StartGestureData const & data)
{
	startTempoEstimator_.RegisterStartGesture(data);
	auto tempo = startTempoEstimator_.TempoEstimate();
	tempoFilter_.Reset(startTempoEstimator_.StartTimeEstimate(), tempo);
	tempoFunction_.SetConstantTempo(tempo);
}

void
TempoFollower::RegisterBeat(real_time_t const & beatTime, double clarity)
{
	score_time_t beatScoreTime = timeWarper_.WarpTimestamp(beatTime);
	ScorePosition position = tempoMap_.GetScorePositionAt(beatScoreTime);

	LOG("Got beat at: %1% | %2%", position.bar(), position.beat());
	beatClassifier_->RegisterBeat(beatTime, position, tempoFunction_.OffsetAt(beatTime));
}

void
TempoFollower::BeatClassified(BeatClassification const & classification)
{
	// Ignore duplicates
	if (classification.IntendedPosition() == previousClassification_.IntendedPosition()) { return; }

	auto & options = parent_.OptionsReader();
	double tempoFilterCutoffMs = options->at<Options::TempoFilterTime>();
	tempoFilter_.SetCutoffPoint(time_quantity(tempoFilterCutoffMs * si::milli * si::seconds));

	auto const & position = classification.position();
	double offsetFraction = classification.offset() / (position.meter().BarDuration() * score::bar);
	auto const & beatTime = classification.timestamp();

	// Tempo
	auto timeNow = time::now();
	tempo_t tempoNow = tempoFunction_.TempoAt(timeNow);
	time_quantity beatTimeDiff = 1.0 * score::beats / tempoNow;
	tempo_t tempoChange = 0.0 * score::beats_per_second;
	if (previousClassification_) {
		auto beatPosDiff = classification.IntendedPosition() - previousClassification_.IntendedPosition();
		beatTimeDiff = time_cast<time_quantity>(beatTime - previousBeatTime_);
		tempo_t targetTempo = beatPosDiff / beatTimeDiff;
		tempoFilter_.AddValue(beatTime, targetTempo);
		targetTempo = tempoFilter_.ValueAt(beatTime);
		tempoChange = targetTempo - tempoNow;
	}

	// Maximum acceleration interval is a little under two beats
	score_time_t scoreTime = classification.position().time();
	auto accelerationInterval= bu::min(beatTimeDiff, time_quantity(1.8 * score::beats / tempoNow));
	auto accelerationTime = AccelerationTimeAt(scoreTime, accelerationInterval);

#if DEBUG_TEMPO_FOLLOWER
	LOG("Classified with position %1% and offset %2%", classification.position().position(), classification.offset());
	LOG("Intended position: %1%", classification.IntendedPosition());
	LOG("Beat offset: %1%, tempo change: %2%, tempoNow: %3%", classification.offset(), tempoChange, tempoNow);
#endif
		
	double offsetFactor = options->at<Options::CatchupFraction>();
	tempoFunction_.SetParameters(
		time::now(), accelerationTime,
		tempoNow, tempoChange,
		-classification.offset(), offsetFactor / 100.0);

	// TODO clean up
	previousClassification_ = classification;
	previousBeatTime_ = beatTime;

	parent_.statusEventProviderImpl().buffer_.enqueue(
		StatusEvent(classification.timestamp(), StatusEvent::Beat,
		            BeatEvent(position.FractionOfBar(), offsetFraction)));
}

speed_t
TempoFollower::SpeedEstimateAt(real_time_t const & time)
{
	score_time_t scoreTime = timeWarper_.WarpTimestamp(time);

	// Tempo change
	auto scoreTempo = ScorePositionAt(time).tempo();
	if (scoreTempo != previousScoreTempo_) {
		auto statusReader = parent_.status().GetUnsafeReader();

		double factor = scoreTempo / previousScoreTempo_;
		if (statusReader->at<Status::State>() == FollowerState::Playback) {
			tempoFunction_.ScaleToRelativeTempoChange(time, factor);
		}
		beatClassifier_->RegisterTempoChange(factor);
		
		// Reset filter to some time before now,
		// so that it reacts faster
		auto diffToUnity = std::abs(factor - 1.0);
		auto timeToSubtract = diffToUnity * tempoFilter_.GetCutoffPoint();
		auto resetTime = time - time_cast<timestamp_t::duration>(timeToSubtract);
		tempoFilter_.Reset(resetTime, tempoFilter_.ValueAt(time));

		previousScoreTempo_ = scoreTempo;
	}

	auto tempo = tempoFunction_.TempoAt(time);
	return tempo / ScorePositionAt(time).tempo();
}

ScorePosition
TempoFollower::ScorePositionAt(real_time_t const & time) const
{
	auto scoreTime = timeWarper_.WarpTimestamp(time);
	return tempoMap_.GetScorePositionAt(scoreTime);
}

void
TempoFollower::StartAtDefaultTempo()
{
	auto pos = tempoMap_.GetScorePositionAt(score_time_t::from_value(0.0));
	auto fakeDuration = time_cast<duration_t>(0.5 * score::beats / pos.tempo());
	auto fakeStart = time::now();
	RegisterStartGesture(MotionTracker::StartGestureData(fakeStart, fakeDuration));
}

score_time_t
TempoFollower::AccelerationTimeAt(score_time_t time, time_quantity beatInterval)
{
	// first beats can be before 0, but we want the sensitivity from the beginning
	time = boost::units::max(time, 0.0 * score::seconds);

	double sensitivity = 0.5;
	auto changes = tempoSensitivities_.EventsSinceInclusive(time);
	if (!changes.Empty()) {
		sensitivity = changes.Front().data.sensitivity;
	}

	// sensitivity of 0.5 means catchup in one beat
	return (1.0 - sensitivity) * 2.0 * beatInterval;
}

} // namespace ScoreFollower
} // namespace cf
