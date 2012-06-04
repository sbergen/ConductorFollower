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

TrackReader<MidiMessage> *
MidiReader::Track(int index)
{
	assert(index < TrackCount());
	return new TrackReaderImpl(*file_.getTrack(index));
}

TrackReader<tempo_t> *
MidiReader::TempoTrack()
{
	return new TempoReaderImpl(file_);
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
	double tickLength = ePtr->message.getTempoMetaEventTickLength(timeFormat_);
	data = tempo_t(static_cast<tempo_t::rep>(tickLength));

	++current_;
	return current_ < count_;
}

MidiReader::TrackReaderImpl::TrackReaderImpl(MidiMessageSequence const & sequence)
	: sequence_(sequence)
	, count_(sequence.getNumEvents())
	, current_(0)
{}

bool
MidiReader::TrackReaderImpl::NextEvent(cf::ScoreFollower::score_time_t & timestamp, MidiMessage & data)
{
	assert(current_ < count_);
	
	seconds_t seconds(sequence_.getEventTime(current_));
	timestamp = time::duration_cast<score_time_t>(seconds);

	data = sequence_.getEventPointer(current_)->message;

	++current_;
	return current_ < count_;
}