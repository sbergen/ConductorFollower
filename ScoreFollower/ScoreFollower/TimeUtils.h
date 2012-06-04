#pragma once

#include <utility>

#include "ScoreFollower/types.h"

namespace cf {
namespace ScoreFollower {

class TimeUtils
{
public:
	static std::pair<real_time_t, real_time_t>
	GetTimeSpanEstimateForAudioBlock(unsigned samples, unsigned samplerate)
	{
		real_time_t start(time::now());
		seconds_t blockDuration(static_cast<seconds_t::rep>(samples) / samplerate);
		real_time_t end = start + time::duration_cast<real_time_t::duration>(blockDuration);

		return std::make_pair(start, end);
	}

	template<typename TDuration>
	static unsigned DurationToSamples(TDuration duration, unsigned samplerate)
	{
		return static_cast<unsigned>(time::duration_cast<seconds_t>(duration).count() * samplerate);
	}
};

} // namespace ScoreFollower
} // namespace cf
