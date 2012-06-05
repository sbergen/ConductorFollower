#include "AudioBlockTimeManager.h"


namespace cf {
namespace ScoreFollower {

AudioBlockTimeManager::AudioBlockTimeManager(unsigned samplerate, unsigned blockSize)
	: samplerate_(samplerate)
	, blockSize_(blockSize)
	, currentBlockStart_(real_time_t::min())
	, currentBlockEnd_(real_time_t::min())
	, theoreticalBlockDuration_(static_cast<seconds_t::rep>(blockSize_) / samplerate_)
{
}
	
std::pair<real_time_t, real_time_t>
AudioBlockTimeManager::GetRangeForNow()
{
	auto estimate = EstimateBlock();

	// On first run fake the previous-end-estimate
	if (currentBlockEnd_ == real_time_t::min()) {
		currentBlockEnd_ = estimate.first;
	}

	// Update state
	currentBlockStart_ = currentBlockEnd_;
	currentBlockEnd_ = estimate.second;
	UpdateStretchFactor();

	return std::make_pair(currentBlockStart_, currentBlockEnd_);
}

unsigned
AudioBlockTimeManager::ToSampleOffset(real_time_t const & time)
{
	assert(time >= currentBlockStart_);
	assert(time <= currentBlockEnd_);

	real_time_t::duration offset = time - currentBlockStart_;
	seconds_t offsetInSeconds = time::duration_cast<seconds_t>(offset);
	offsetInSeconds *= currentBlockStretch_;

	// TODO change rounding policy?
	// The timing difference is so small it shouldn't matter...
	return static_cast<unsigned>(offsetInSeconds.count() * blockSize_);
}

std::pair<real_time_t, real_time_t>
AudioBlockTimeManager::EstimateBlock()
{
	real_time_t begin = time::now();
	real_time_t end = begin + time::duration_cast<real_time_t::duration>(theoreticalBlockDuration_);
	return std::make_pair(begin, end);
}

void
AudioBlockTimeManager::UpdateStretchFactor()
{
	seconds_t duration = time::duration_cast<seconds_t>(currentBlockEnd_ - currentBlockStart_);
	currentBlockStretch_ = duration.count() / theoreticalBlockDuration_.count();
}

} // namespace ScoreFollower
} // namespace cf
