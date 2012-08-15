#pragma once

#include <vector>

#include "cf/EventBuffer.h"
#include "cf/units_math.h"

#include "ScoreFollower/types.h"
#include "ScoreFollower/ScoreReader.h"

#include "TempoMap.h"
#include "BeatClassification.h"
#include "BeatClassifier.h"
#include "SpeedFunction.h"

#include <boost/utility.hpp>

namespace cf {
namespace ScoreFollower {

class TimeWarper;
class Follower;

class TempoFollower : public boost::noncopyable
{
public:
	TempoFollower(TimeWarper const & timeWarper, Follower & parent);

	void ReadScore(ScoreReader & reader) { tempoMap_.ReadScore(reader); }

	void RegisterPreparatoryBeat(real_time_t const & time);
	void RegisterBeat(real_time_t const & beatTime, double prob);
	speed_t SpeedEstimateAt(real_time_t const & time);

private:
	typedef EventBuffer<BeatClassification, real_time_t> BeatHistoryBuffer;

private:
	BeatClassification ClassifyBeatAt(real_time_t const & time, double prob);
	
	double ClassificationQuality(BeatClassification const & latestBeat) const;
	double ClassificationSelector(BeatClassification const & latestBeat, double quality) const;

	beat_pos_t BeatOffsetEstimate() const;
	beat_pos_t BeatOffsetEstimate(BeatClassification const & latestBeat) const;

private:
	TimeWarper const & timeWarper_;
	Follower & parent_;

	TempoMap tempoMap_;
	BeatHistoryBuffer beatHistory_;

	bool newBeats_;
	speed_t speed_;

	SpeedFunction acceleration_;
};

} // namespace ScoreFollower
} // namespace cf
