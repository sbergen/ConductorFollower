#include "MidiReader.h"

using namespace cf;
using namespace cf::ScoreFollower;

MidiReader::MidiReader(String const & filename)
{
	File file(filename);
	FileInputStream stream(file);
	file_.readFrom(stream);
	file_.convertTimestampTicksToSeconds();
}

MidiReader::TrackReaderPtr
MidiReader::Track(int index)
{
	assert(index < TrackCount());
	return TrackReaderPtr(new TrackReaderImpl(*file_.getTrack(index), events_));
}

MidiReader::TempoReaderPtr
MidiReader::TempoTrack()
{
	return TempoReaderPtr(new TempoReaderImpl(file_));
}

MidiReader::TempoReaderImpl::TempoReaderImpl(MidiFile const & file)
	: timeFormat_(file.getTimeFormat())
	, current_(0)
{
	file.findAllTempoEvents(sequence_);
	count_ = sequence_.getNumEvents();
}

bool
MidiReader::TempoReaderImpl::NextEvent(cf::ScoreFollower::score_time_t & timestamp, tempo_t & data)
{
	assert(current_ < count_);
	
	seconds_t seconds(sequence_.getEventTime(current_));
	timestamp = time::duration_cast<score_time_t>(seconds);
	
	auto ePtr = sequence_.getEventPointer(current_);
	double dqDuration = ePtr->message.getTempoSecondsPerQuarterNote();
	seconds_t qDuration(dqDuration);
	data = time::duration_cast<tempo_t>(qDuration);

	++current_;
	return current_ < count_;
}

MidiReader::TrackReaderImpl::TrackReaderImpl(MidiMessageSequence const & sequence, EventContainer & events)
	: events_(events)
	, sequence_(sequence)
	, count_(sequence.getNumEvents())
	, current_(0)
{}

bool
MidiReader::TrackReaderImpl::NextEvent(cf::ScoreFollower::score_time_t & timestamp, sf::ScoreEventHandle & data)
{
	assert(current_ < count_);
	
	seconds_t seconds(sequence_.getEventTime(current_));
	timestamp = time::duration_cast<score_time_t>(seconds);

	events_.push_back(sequence_.getEventPointer(current_)->message);
	data = ScoreEventHandle(events_.back());

	++current_;
	return current_ < count_;
}