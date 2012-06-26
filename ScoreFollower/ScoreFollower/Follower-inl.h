template<typename TData>
Follower<TData>::Follower(unsigned samplerate, unsigned blockSize)
	: private_(samplerate, blockSize)
{

}

template<typename TData>
void Follower<TData>::CollectData(ScoreReader<TData> & scoreReader)
{
	{
		boost::scoped_ptr<TrackReader<tempo_t> > tempoReader(scoreReader.TempoTrack());
		private_.ReadTempoTrack(*tempoReader);
	}
	ReadNormalTracks(scoreReader);
}

template<typename TData>
void Follower<TData>::StartNewBlock()
{
	private_.StartNewBlock(currentScoreBlock_);
}

template<typename TData>
void Follower<TData>::GetTrackEventsForBlock(unsigned track, MidiManipulator<TData> & manipulator, BlockBuffer & events)
{
	assert(track < trackBuffers_.size());
	auto ev = trackBuffers_[track].EventsBetween(currentScoreBlock_.first, currentScoreBlock_.second);

	events.Clear();
	if (!private_.Rolling()) { return; }

	ev.ForEach(boost::bind(&Follower<TData>::CopyEventToBuffer, this, _1, _2, boost::ref(manipulator), boost::ref(events)));
}

template<typename TData>
void Follower<TData>::CopyEventToBuffer(score_time_t const & time, TData data, MidiManipulator<TData> & manipulator, BlockBuffer & events)
{
	unsigned frameOffset = private_.ScoreTimeToFrameOffset(time);
	double velocity = private_.NewVelocityAt(manipulator.GetVelocity(data), time);
	manipulator.ApplyVelocity(data, velocity);
	events.RegisterEvent(frameOffset, data);
}

template<typename TData>
void Follower<TData>::ReadNormalTracks(ScoreReader<TData> & scoreReader)
{
	score_time_t timestamp;
	TData data;
	for (int i = 0; i < scoreReader.TrackCount(); ++i) {
		trackBuffers_.push_back(TrackBuffer(100));
		boost::scoped_ptr<TrackReader<TData> > reader(scoreReader.Track(i));
		while (reader->NextEvent(timestamp, data)) {
			trackBuffers_[i].RegisterEvent(timestamp, data);
		}
	}
}
