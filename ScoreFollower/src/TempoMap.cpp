#include "TempoMap.h"

#include "cf/globals.h"

#include "ScoreFollower/TrackReader.h"
#include "ScoreFollower/TimeSignature.h"

namespace cf {
namespace ScoreFollower {

TempoMap::TempoMap()
{
}

void
TempoMap::ReadScore(ScoreReader & reader)
{
	ReadTempo(reader.TempoTrack());
	ReadMeter(reader.MeterTrack());	
}

TempoPoint
TempoMap::GetTempoAt(score_time_t const & time) const
{
	auto range = changes_.EventsSinceInclusive(time);
	assert(!range.Empty());
	return range[0].data.GetTempoAt(time);
}

TimeSignature
TempoMap::GetMeterAt(score_time_t const & time) const
{
	auto range = meters_.EventsSinceInclusive(time);
	assert(!range.Empty());
	return range[0].data;
}

void
TempoMap::ReadTempo(TempoReaderPtr reader)
{
	bool first = true;
	TempoChange previousChange;
	score_time_t timestamp;
	tempo_t tempo;

	while (reader->NextEvent(timestamp, tempo)) {
		// If the first tempo change is not at zero, insert default tempo at beginning
		if (first && timestamp > score_time_t::zero()) {
			EnsureChangesNotEmpty();
			previousChange = changes_.AllEvents()[0].data;
			first = false;
		}

		// Then continue normally...
		beat_pos_t pos = first ? 0.0 : previousChange.GetTempoAt(timestamp).position();
		first = false;

		LOG("Tempo change, timestamp: %1%, pos: %2%, tempo: %3%", timestamp, pos, tempo);
		previousChange = TempoChange(timestamp, TempoPoint(timestamp, pos, tempo));
		changes_.RegisterEvent(timestamp, previousChange);
	}

	EnsureChangesNotEmpty();
}

void
TempoMap::ReadMeter(MeterReaderPtr reader)
{
	score_time_t timestamp;
	TimeSignature signature;

	while (reader->NextEvent(timestamp, signature)) {
		LOG("Meter change, timestamp: %1%, %2%/%3%", timestamp, signature.count(), signature.division());
		meters_.RegisterEvent(timestamp, signature);
	}
}

void
TempoMap::EnsureChangesNotEmpty()
{
	if (changes_.AllEvents().Empty()) {
		seconds_t qDuration(60.0/*s*/ / 120 /*bpm*/);
		TempoPoint tp(score_time_t::zero(), 0.0, time::duration_cast<tempo_t>(qDuration));
		changes_.RegisterEvent(score_time_t::zero(), TempoChange(score_time_t::zero(), tp));
	}
}

/****** TempoChange ******/

TempoMap::TempoChange::TempoChange(score_time_t const & timestamp, TempoPoint const & tempo)
	: timestamp_(timestamp)
	, tempo_(tempo)
{
}

TempoPoint
TempoMap::TempoChange::GetTempoAt(score_time_t const & time) const
{
	beat_pos_t position = tempo_.position() + tempo_.wholeBeats(time) + tempo_.fraction(time);
	return TempoPoint(time, position, tempo_.tempo());
}

} // namespace ScoreFollower
} // namespace cf
