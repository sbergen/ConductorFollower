#pragma once

#include <vector>

#include "cf/EventBuffer.h"

#include "ScoreFollower/types.h"
#include "ScoreFollower/ScoreReader.h"

#include "TempoMap.h"
#include "BeatClassifier.h"

#include <boost/utility.hpp>
#include <boost/function.hpp>

namespace cf {
namespace ScoreFollower {

class TimeWarper;
class Follower;

class TempoFollower : public boost::noncopyable
{
public:
	TempoFollower(TimeWarper const & timeWarper, Follower & parent);

	void ReadScore(ScoreReader & reader) { tempoMap_.ReadScore(reader); }
	void RegisterBeat(real_time_t const & beatTime, double prob);

	speed_t SpeedEstimateAt(real_time_t const & time);

private:

	struct BeatClassification
	{
		BeatClassification(beat_pos_t offset, double probability)
			: offset(offset), probability(probability) {}

		beat_pos_t offset;
		double probability;
	};

	typedef EventBuffer<BeatClassification, real_time_t> BeatHistoryBuffer;

private:
	BeatClassification ClassifyBeatAt(real_time_t const & time, double prob);
	
	beat_pos_t BeatOffsetEstimate() const;
	beat_pos_t BeatOffsetHypothesis(BeatClassification const & latestBeat) const;
	beat_pos_t BeatOffsetHypothesis(BeatClassification const & latestBeat,
		BeatHistoryBuffer::Range const & otherBeats) const;

	speed_t SpeedFromBeatCatchup(TempoPoint const & tempoNow, beat_pos_t catchupTime) const;

private:
	TimeWarper const & timeWarper_;
	Follower & parent_;

	BeatClassifier beatClassifier_;

	TempoMap tempoMap_;
	BeatHistoryBuffer beatHistory_;

	bool newBeats_;
	speed_t speed_;


	/* New state */
	boost::function<double(real_time_t const &)> acceleration_;
	double targetSpeed_; // TODO units
	real_time_t accelerateUntil_;

};

} // namespace ScoreFollower
} // namespace cf
