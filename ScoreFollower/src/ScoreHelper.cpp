#include "ScoreHelper.h"

#include "ScoreFollower/ScoreReader.h"
#include "ScoreFollower/TrackReader.h"

namespace cf {
namespace ScoreFollower {

void
ScoreHelper::CollectData(boost::shared_ptr<ScoreReader> scoreReader)
{
	// Keep reference to this
	scoreReader_ = scoreReader;

	// Read data
	score_time_t timestamp;
	ScoreEventPtr data;
	for (int i = 0; i < scoreReader->TrackCount(); ++i) {
		trackBuffers_.push_back(new TrackBuffer(0));
		auto reader = scoreReader->Track(i);
		while (reader->NextEvent(timestamp, data)) {
			trackBuffers_[i].RegisterEvent(timestamp, data);
		}
	}
}

ScoreHelper::TrackBuffer &
ScoreHelper::operator[](unsigned track)
{
	assert(track < trackBuffers_.size());
	return trackBuffers_[track];
}

} // namespace ScoreFollower
} // namespace cf