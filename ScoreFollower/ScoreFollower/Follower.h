#pragma once

#include <vector>

#include <boost/scoped_ptr.hpp>

#include "cf/EventBuffer.h"

#include "ScoreFollower/FollowerPrivate.h"
#include "ScoreFollower/ScoreReader.h"
#include "ScoreFollower/TrackReader.h"

namespace cf {
namespace ScoreFollower {

template<typename TData>
class Follower
{
public:
	typedef EventBuffer<TData, score_time_t, std::vector> TrackBuffer;

	Follower() {}

	void CollectData(ScoreReader<TData> & scoreReader)
	{
		{
			boost::scoped_ptr<TrackReader<bool> > tempoReader(scoreReader.TempoTrack());
			tiImpl_.ReadTempoTrack(*tempoReader);
		}
		ReadNormalTracks(scoreReader);
	}

	// After this call, every real time >= realTime has to return score times >= scoreTime
	void FixTimeMapping(real_time_t const & realTime, score_time_t const & scoreTime)
	{
		tiImpl_.FixTimeMapping(realTime, scoreTime);
	}

	void RegisterBeat(real_time_t const & beatTime)
	{
		tiImpl_.RegisterBeat(beatTime);
	}

	typename TrackBuffer::DataRange GetEventsBetween(
		int track, real_time_t const & from, real_time_t const & to)
	{
		score_time_t sFrom(tiImpl_.WarpTimestamp(from));
		score_time_t sTo(tiImpl_.WarpTimestamp(to));

		return trackBuffers[track].GetDataBetween(sFrom, sTo);
	}

private:
	void ReadNormalTracks(ScoreReader<TData> & scoreReader)
	{
		score_time_t timestamp;
		TData data;
		for (int i = 0; i < scoreReader.TrackCount(); ++i) {
			trackBuffers.push_back(TrackBuffer());
			boost::scoped_ptr<TrackReader<TData> > reader(scoreReader.Track(i));
			while (reader->NextEvent(timestamp, data)) {
				trackBuffers_[i].RegisterEvent(timestamp, data);
			}
		}
	}

private:
	std::vector<TrackBuffer> trackBuffers;

	// Type independent part of implementation
	FollowerTypeIndependentImpl tiImpl_;
};

} // namespace ScoreFollower
} // namespace cf
