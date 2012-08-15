#pragma once

#include <vector>

#include "cf/EventBuffer.h"

#include "ScoreFollower/types.h"
#include "ScoreFollower/ScoreReader.h"

#include "TempoMap.h"
#include "BeatClassification.h"
#include "BeatClassifier.h"

#include <boost/utility.hpp>
#include <boost/units/cmath.hpp>

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

	BeatClassifier beatClassifier_;

	TempoMap tempoMap_;
	BeatHistoryBuffer beatHistory_;

	bool newBeats_;
	speed_t speed_;


	class SpeedFunction
	{
	public:
		typedef boost::units::quantity<score::speed_change> SpeedChangeRate;

		void SetParameters(speed_t const & reference_speed, real_time_t const & reference_time, SpeedChangeRate const & changePerTimeUnit)
		{
			reference_speed_ = reference_speed;
			reference_time_ = reference_time;
			changePerTimeUnit_ = changePerTimeUnit;
		}

		speed_t NewSpeed(real_time_t const & time)
		{
			auto timeDiff = time::quantity_cast<time_quantity>(time - reference_time_);
			return reference_speed_ + (timeDiff * changePerTimeUnit_);
		}

	private:
		speed_t reference_speed_;
		real_time_t reference_time_;
		SpeedChangeRate changePerTimeUnit_;
	};

	SpeedFunction acceleration_;
	real_time_t accelerateUntil_;

};

} // namespace ScoreFollower
} // namespace cf
