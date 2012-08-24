#include "TempoFollower.h"

#include <boost/bind.hpp>
#include <boost/variant/apply_visitor.hpp>

#include "cf/globals.h"
#include "cf/math.h"

#include "ScoreFollower/Follower.h"
#include "ScoreFollower/FollowerStatus.h"

#include "TimeWarper.h"

namespace cf {
namespace ScoreFollower {

namespace si = boost::units::si;

class TempoFollower::ScoreEventBuilder
{
public:
	typedef void result_type;

	ScoreEventBuilder(TempoFollower & parent)
		: parent_(parent) {}

	void operator() (Data::TempoSensitivityChange const & change) const
	{
		auto scoreTime = parent_.tempoMap_.TimeAt(change.position);
		parent_.tempoSensitivities_.RegisterEvent(scoreTime, change);
	}

private:
	TempoFollower & parent_;
};

TempoFollower::TempoFollower(TimeWarper const & timeWarper, Follower & parent)
	: timeWarper_(timeWarper)
	, parent_(parent)
	, tempoMap_()
	, startTempoEstimator_()
	, beatClassifier_(tempoMap_)
	, beatHistory_(100) // Arbitrary length, should be long enough...
	, newBeats_(false)
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
	auto speed = startTempoEstimator_.SpeedEstimate();
	acceleration_.SetConstantSpeed(speed);
}

void
TempoFollower::RegisterBeat(real_time_t const & beatTime, double clarity)
{
	assert(beatTime > beatHistory_.AllEvents().LastTimestamp());

	auto classification = ClassifyBeatAt(beatTime, clarity);
	if (true /* TODO rejection! */) {
		beatHistory_.RegisterEvent(beatTime, classification);
		LOG("Classified with offset: %1%", classification.offset());
		newBeats_ = true;
	}
}


speed_t
TempoFollower::SpeedEstimateAt(real_time_t const & time)
{
	if (newBeats_) {
		newBeats_ = false;

		score_time_t scoreTime = timeWarper_.WarpTimestamp(time);
		tempo_t tempoNow = tempoMap_.GetScorePositionAt(scoreTime).tempo();

		// TODO Move to estimator
		auto offsetEstimate = -BeatOffsetEstimate();
		auto accelerationTime = AccelerationTimeAt(scoreTime);
		auto tempoChange = offsetEstimate / boost::units::pow<2>(accelerationTime);
		SpeedFunction::SpeedChangeRate acceleration(tempoChange / tempoNow);

		//LOG("Beat offset: %1%, tempo change: %2%, acceleartion: %3%",
		//	offsetEstimate, tempoChange.value(), acceleration.value());
		
		auto speed = acceleration_.SpeedAt(time);
		acceleration_.SetParameters(speed, time, acceleration, accelerationTime);
	}

	return acceleration_.SpeedAt(time);
}

BeatClassification
TempoFollower::ClassifyBeatAt(real_time_t const & time, double clarity)
{
	// TODO allow different estimation modes
	score_time_t beatScoreTime = timeWarper_.WarpTimestamp(time);
	ScorePosition position = tempoMap_.GetScorePositionAt(beatScoreTime);

	LOG("Got beat at: %1% | %2%", position.bar(), position.beat());
	return beatClassifier_.ClassifyBeat(position, acceleration_.FractionAt(time));
}

beat_pos_t
TempoFollower::BeatOffsetEstimate() const
{
	auto lastBeat = beatHistory_.AllEvents().Back().data;
	return lastBeat.offset();
}

score_time_t
TempoFollower::AccelerationTimeAt(score_time_t time)
{
	score_time_t const minAccelerationTime = time_quantity(0.1 * score::seconds);
	score_time_t const accelerationTimeRange = time_quantity(3.4 * score::seconds);
	score_time_t const defaultAccelerationTime = time_quantity(1.8 * score::seconds);

	auto changes = tempoSensitivities_.EventsSinceInclusive(time);
	if (changes.Empty()) { return defaultAccelerationTime; }

	auto sensitivity = changes.Front().data.sensitivity;
	auto factor = 1.0 - sensitivity;
	return minAccelerationTime + (factor * accelerationTimeRange);
}

} // namespace ScoreFollower
} // namespace cf
