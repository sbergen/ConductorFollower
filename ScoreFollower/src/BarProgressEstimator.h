#pragma once

#include <vector>

#include "Data/BeatPattern.h"

#include "ScoreFollower/types.h"

#include "ScorePosition.h"

namespace cf {
namespace ScoreFollower {

class BeatClassification;

class BarProgressEstimator
{
public:
	BarProgressEstimator(Data::BeatPattern const & pattern);

	BeatClassification ClassifyBeat(
		ScorePosition const & position, beat_pos_t beginningOfBar, beat_pos_t offsetEstimate);

private:
	struct Beat
	{
		explicit Beat(beat_pos_t const & position)
			: position(position), used(false) {}

		beat_pos_t position;
		bool used;

		bool operator==(Beat const & other) const
			{ return position == other.position; }
	};

	struct OffsetPair
	{
		OffsetPair(beat_pos_t const & absolute, beat_pos_t const & estimation)
			: absolute(absolute), estimation(estimation) {}

		beat_pos_t absolute;
		beat_pos_t estimation;
	};

private:
	typedef std::vector<Beat> BeatList;

	void ResetIfBarChanged(beat_pos_t const & beginningOfBar);
	BeatList::iterator ClassifyBeat(beat_pos_t const & estimationPosition);
	void AddPenaltyForUnusedBeats(BeatList::iterator currentBeat);
	OffsetPair EstimateForNextBar(OffsetPair const & positions);

private:
	Data::BeatPattern pattern_;
	BeatList beats_;
	beat_pos_t previousBeginningOfBar_;

	double qualityForThisBar_;
};


} // namespace ScoreFollower
} // namespace cf
