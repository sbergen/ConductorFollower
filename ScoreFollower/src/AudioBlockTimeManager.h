#pragma once

#include <utility>

#include <boost/utility.hpp>

#include "ScoreFollower/types.h"

namespace cf {
namespace ScoreFollower {

class AudioBlockTimeManager : public boost::noncopyable
{
public:
	AudioBlockTimeManager(samplerate_t samplerate, samples_t blockSize);
	
	std::pair<real_time_t, real_time_t> GetRangeForNow();
	samples_t ToSampleOffset(real_time_t const & time) const;

	real_time_t const & CurrentBlockStart() const { return currentBlockStart_; }
	real_time_t const & CurrentBlockEnd() const { return currentBlockEnd_; }

private:

	std::pair<real_time_t, real_time_t> EstimateBlock();
	void UpdateStretchFactor();

	samplerate_t const samplerate_;
	samples_t const blockSize_;

	time_quantity const theoreticalBlockDuration_;

	real_time_t currentBlockStart_;
	real_time_t currentBlockEnd_;
	double currentBlockStretch_;
};

} // namespace ScoreFollower
} // namespace cf
