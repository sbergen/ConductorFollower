#pragma once

#include <utility>

#include <boost/array.hpp>
#include <boost/function.hpp>

#include "cf/globals.h"
#include "cf/units_math.h"

#include "ScoreFollower/types.h"

namespace cf {
namespace ScoreFollower {

class BeatClassification
{
public:
	BeatClassification(timestamp_t const & timestamp, double const & clarity, beat_pos_t const & beatPosition)
		: timestamp_(timestamp)
		, clarity_(clarity)
	{
		UpdatedEstimate(beatPosition);
	}

	void UpdatedEstimate(beat_pos_t const & beatPosition)
	{
		negativeOffset_ = beatPosition - boost::units::ceil(beatPosition);
		positiveOffset_ = beatPosition - boost::units::floor(beatPosition);
	}

	timestamp_t timestamp() const { return timestamp_; }
	double clarity() const { return clarity_; }

	beat_pos_t MostLikelyOffset() const
	{
		using boost::units::abs;
		return (abs(negativeOffset_) < abs(positiveOffset_)) ?
			negativeOffset_ : positiveOffset_;
	}

private:
	// non-const for assignability
	real_time_t timestamp_;
	double clarity_;

	beat_pos_t negativeOffset_;
	beat_pos_t positiveOffset_;
};


} // namespace ScoreFollower
} // namespace cf
