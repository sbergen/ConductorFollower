#include "AudioBlockTimeManager.h"


namespace cf {
namespace ScoreFollower {

AudioBlockTimeManager::AudioBlockTimeManager(samplerate_t samplerate, samples_t blockSize)
	: samplerate_(samplerate)
	, blockSize_(blockSize)
	, currentBlockStart_(real_time_t::min())
	, currentBlockEnd_(real_time_t::min())
	, theoreticalBlockDuration_(blockSize_ / samplerate_)
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

samples_t
AudioBlockTimeManager::ToSampleOffset(real_time_t const & time) const
{
	assert(time >= currentBlockStart_);
	assert(time <= currentBlockEnd_);

	time_quantity offset = time::quantity_cast<time_quantity>(time - currentBlockStart_);
	offset *= currentBlockStretch_;

	return offset * samplerate_;
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
	time_quantity duration = time::quantity_cast<time_quantity>(currentBlockEnd_ - currentBlockStart_);
	// Stretch or squeeze the duration to fit into the theoretical length
	currentBlockStretch_ =  duration / theoreticalBlockDuration_;
}

} // namespace ScoreFollower
} // namespace cf
