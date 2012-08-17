#include "MidiReader.h"

#include <boost/make_shared.hpp>

#include "cf/globals.h"

#include "ScoreFollower/ScoreEvent.h"

using namespace cf;
using namespace cf::ScoreFollower;

#define DEBUG_SCORE 0

MidiReader::MidiReader()
{
}

MidiReader::~MidiReader()
{
}

void
MidiReader::OpenFile(std::string const & filename)
{
	File file(String(filename.c_str()));
	FileInputStream stream(file);
	file_.readFrom(stream);
	file_.convertTimestampTicksToSeconds();
}

sf::TrackReaderPtr
MidiReader::Track(int index)
{
#if DEBUG_SCORE
	LOG("Getting track %1%", index);
#endif // DEBUG_SCORE

	assert(index < TrackCount());
	return boost::make_shared<TrackReaderImpl>(*file_.getTrack(index));
}

sf::TempoReaderPtr
MidiReader::TempoTrack()
{
	return boost::make_shared<TempoReaderImpl>(file_);
}

sf::MeterReaderPtr
MidiReader::MeterTrack()
{
	return boost::make_shared<MeterReaderImpl>(file_);
}

// Tempo reader

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
	
	timestamp = sequence_.getEventTime(current_) * score::seconds;
	
	auto ePtr = sequence_.getEventPointer(current_);
	auto rawTempo = (ePtr->message.getTempoSecondsPerQuarterNote() * score::seconds) / (1.0 * score::quarter_note);
	// The two tempo representations are each others inverses
	data = tempo_t(1.0 / rawTempo);
	
	++current_;
	return current_ < count_;
}

// Meter reader

MidiReader::MeterReaderImpl::MeterReaderImpl(MidiFile const & file)
	: current_(0)
{
	file.findAllTimeSigEvents(sequence_);
	count_ = sequence_.getNumEvents();
}

bool
MidiReader::MeterReaderImpl::NextEvent(sf::score_time_t & timestamp, cf::TimeSignature & data)
{
	assert(current_ < count_);
	
	timestamp = sequence_.getEventTime(current_) * score::seconds;
	
	auto ePtr = sequence_.getEventPointer(current_);
	int numerator = 0, denominator = 0;
	ePtr->message.getTimeSignatureInfo(numerator, denominator);
	assert(numerator > 0 && denominator > 0);
	data = TimeSignature(numerator, denominator);

	++current_;
	return current_ < count_;
}

// Track reader

MidiReader::TrackReaderImpl::TrackReaderImpl(MidiMessageSequence const & sequence)
	: sequence_(sequence)
	, count_(sequence.getNumEvents())
	, current_(0)
{}

bool
MidiReader::TrackReaderImpl::NextEvent(cf::ScoreFollower::score_time_t & timestamp, sf::ScoreEventPtr & data)
{
	assert(current_ < count_);
	
	timestamp = sequence_.getEventTime(current_) * score::seconds;

	auto offTimestamp = sequence_.getTimeOfMatchingKeyUp(current_) * score::seconds;
	
	score_time_t noteLength = 0.0 * score::seconds;
	if (offTimestamp > 0.0 * score::seconds) {
		noteLength = offTimestamp - timestamp;
	}

	auto eventPtr = sequence_.getEventPointer(current_);
	data = boost::make_shared<MidiEvent>(eventPtr->message, noteLength);

#if DEBUG_SCORE
	if (current_ < 10) {
		LOG("Read event with channel %1%", eventPtr->message.getChannel());
	}
#endif // DEBUG_SCORE

	++current_;
	return current_ < count_;
}