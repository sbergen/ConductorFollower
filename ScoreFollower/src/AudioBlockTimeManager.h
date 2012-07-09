#pragma once

#include <utility>

#include <boost/utility.hpp>

#include "ScoreFollower/types.h"

namespace cf {
namespace ScoreFollower {

class AudioBlockTimeManager : public boost::noncopyable
{
public:
	AudioBlockTimeManager(unsigned samplerate, unsigned blockSize);
	
	std::pair<real_time_t, real_time_t> GetRangeForNow();
	unsigned ToSampleOffset(real_time_t const & time) const;

	real_time_t const & CurrentBlockStart() const { return currentBlockStart_; }
	real_time_t const & CurrentBlockEnd() const { return currentBlockEnd_; }

private:

	std::pair<real_time_t, real_time_t> EstimateBlock();
	void UpdateStretchFactor();

	unsigned const samplerate_;
	unsigned const blockSize_;

	seconds_t const theoreticalBlockDuration_;

	real_time_t currentBlockStart_;
	real_time_t currentBlockEnd_;
	seconds_t::rep currentBlockStretch_;
};

} // namespace ScoreFollower
} // namespace cf
