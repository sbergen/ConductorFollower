#include "TempoFollower.h"

#include <boost/bind.hpp>
#include <boost/variant/apply_visitor.hpp>

#include "cf/globals.h"
#include "cf/math.h"

#include "ScoreFollower/Follower.h"
#include "ScoreFollower/FollowerStatus.h"

#include "TimeWarper.h"

#include "TempoFollowerScoreEventBuilder.h"

#define DEBUG_TEMPO_FOLLOWER 1

namespace cf {
namespace ScoreFollower {

namespace si = boost::units::si;

TempoFollower::TempoFollower(TimeWarper const & timeWarper, Follower & parent)
	: timeWarper_(timeWarper)
	, parent_(parent)
	, tempoMap_()
	, startTempoEstimator_()
	, beatClassifier_(tempoMap_)
	, tempoFilter_(16, 1.5 * si::seconds)
{
}

void
TempoFollower::ReadScore(ScoreReader & reader)
{
	tempoMap_.ReadScore(reader);
	auto start = tempoMap_.GetScorePositionAt(0.0 * score::seconds);
	startTempoEstimator_.SetStartTempo(start.tempo());
}

void
TempoFollower::LearnScoreEvents(Data::ScoreEventList const & events)
{
	tempoSensitivities_.Clear();
	fermatas_.Clear();

	// Collect all events
	ScoreEventBuilder visitor(*this);
	std::for_each(std::begin(events), std::end(events),
		[visitor](Data::ScoreEvent const & event)
		{
			boost::apply_visitor(visitor, event);
		});

	// Preparations for beginning, catch fermatas at 0 with negative value
	LookupNextFermata(-1.0 * score::seconds);
}

void
TempoFollower::RegisterStartGesture(MotionTracker::StartGestureData const & data)
{
	startTempoEstimator_.RegisterStartGesture(data);
	auto tempo = startTempoEstimator_.TempoEstimate();
	tempoFilter_.Reset(startTempoEstimator_.StartTimeEstimate(), tempo);
	tempoFunction_.SetConstantTempo(tempo);
}

BeatEvent
TempoFollower::RegisterBeat(real_time_t const & beatTime, double clarity)
{
	auto classification = ClassifyBeatAt(beatTime, clarity);
	auto const & position = classification.position();
	double offsetFraction = classification.offset() / (position.meter().BarDuration() * score::bar);
	BeatEvent ret(position.FractionOfBar(), offsetFraction);
	
	score_time_t scoreTime = timeWarper_.WarpTimestamp(beatTime);
	tempo_t tempoNow = tempoFunction_.TempoAt(beatTime);

	// Beat time diff for accelerationTime
	time_quantity beatTimeDiff = 1.0 * score::beats / tempoNow;

	// Tempo
	tempo_t tempoChange = 0.0 * score::beats_per_second;
	if (previousClassification_) {
		auto beatPosDiff = classification.IntendedPosition() - previousClassification_.IntendedPosition();
		beatTimeDiff = time_cast<time_quantity>(beatTime - previousBeatTime_);
		tempo_t targetTempo = beatPosDiff / beatTimeDiff;
		tempoFilter_.AddValue(beatTime, targetTempo);
		targetTempo = tempoFilter_.ValueAt(beatTime);
		LOG("beatPosDiff %1% beatTimeDiff %2% targetTempo %3%", beatPosDiff, beatTimeDiff, targetTempo);
		tempoChange = targetTempo - tempoNow;
	}

	auto accelerationTime = AccelerationTimeAt(scoreTime, beatTimeDiff);

#if DEBUG_TEMPO_FOLLOWER
	LOG("Classified with position %1% and offset %2%", classification.position().position(), classification.offset());
	LOG("Intended position: %1%", classification.IntendedPosition());
	LOG("Beat offset: %1%, tempo change: %2%, tempoNow: %3%", classification.offset(), tempoChange, tempoNow);
#endif
		
	// TODO clean up
	if (nextFermata_.IsInFermata() && beatTime > fermataEndTime_) {
		tempoFunction_.SetConstantTempo(fermataReferenceTempo_);
		nextFermata_.Reset();
	} else {
		//offsetFilter_.AddValue(beatTime, -classification.offset());
		//auto offsetToCompensate = 0.6 * offsetFilter_.ValueAt(beatTime);
		auto offsetToCompensate = 0.6 * -classification.offset();
		tempoFunction_.SetParameters(
			beatTime, accelerationTime,
			tempoNow, tempoChange,
			-classification.offset(), offsetToCompensate);
	}

	// TODO clean up
	previousClassification_ = classification;
	previousBeatTime_ = beatTime;

	return ret;
}


speed_t
TempoFollower::SpeedEstimateAt(real_time_t const & time)
{
	score_time_t scoreTime = timeWarper_.WarpTimestamp(time);

	// Fermata
	switch (nextFermata_.GetActionAtTime(scoreTime))
	{
	case FermataState::StoreReferenceTempo:
		fermataReferenceTempo_ = tempoFunction_.TempoAt(time);
		break;
	case FermataState::EnterFermata:
		EnterFermata(time, scoreTime);
		break;
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

BeatClassification
TempoFollower::ClassifyBeatAt(real_time_t const & time, double clarity)
{
	// TODO clean up (duplicate condition)
	if (nextFermata_.IsInFermata() && time > fermataEndTime_) {
		return beatClassifier_.ResetOffsetAndClassifyBeat(nextFermata_.FermataEnd());
	}

	score_time_t beatScoreTime = timeWarper_.WarpTimestamp(time);
	ScorePosition position = tempoMap_.GetScorePositionAt(beatScoreTime);

	LOG("Got beat at: %1% | %2%", position.bar(), position.beat());
	return beatClassifier_.ClassifyBeat(position, tempoFunction_.OffsetAt(time));
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

void
TempoFollower::EnterFermata(real_time_t const & realTime, score_time_t const & scoreTime)
{
	fermataEndTime_ = timeWarper_.InverseWarpTimestamp(nextFermata_.FermataEnd().time(), realTime);
	tempoFunction_.SetConstantTempo(0.0 * score::beats_per_second);
}

void
TempoFollower::LookupNextFermata(score_time_t const & timeNow)
{
	auto fermatas = fermatas_.EventsSince(timeNow);
	if (fermatas.Empty()) { return; }

	nextFermata_ = fermatas.Front().data;
}

} // namespace ScoreFollower
} // namespace cf
