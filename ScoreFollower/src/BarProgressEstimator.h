#pragma once

#include <vector>

#include "Data/BeatPattern.h"

#include "ScoreFollower/types.h"

#include "ScorePosition.h"
#include "BeatScorer.h"

namespace cf {
namespace ScoreFollower {

class BeatClassification;

class BarProgressEstimator
{
public:
	BarProgressEstimator(Data::BeatPattern const & pattern);

	BeatClassification ClassifyBeat(timestamp_t const & timestamp,
		ScorePosition const & position, beat_pos_t beginningOfBar, beat_pos_t offsetEstimate);

private:
	struct Beat
	{
		explicit Beat(Data::BeatPattern const & pattern, unsigned index)
			: scorer(pattern, index)
			, used(false)
		{}
		
		BeatScorer scorer;
		bool used;

		bool operator==(Beat const & other) const { return scorer == other.scorer; }
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
	OffsetPair OffsetsForNextBar(OffsetPair const & positions);

private:
	Data::BeatPattern pattern_;
	BeatList beats_;
	beat_pos_t previousBeginningOfBar_;

	double qualityForThisBar_;
};


} // namespace ScoreFollower
} // namespace cf
