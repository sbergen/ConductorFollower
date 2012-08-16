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
	ReadMeter(reader.MeterTrack());
	ReadTempo(reader.TempoTrack());
}

ScorePosition
TempoMap::GetScorePositionAt(score_time_t const & time) const
{
	auto range = changes_.EventsSinceInclusive(time);
	assert(!range.Empty());
	return range[0].data.GetScorePositionAt(time);
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
		if (first && timestamp > 0.0 * score::seconds) {
			EnsureChangesNotEmpty();
			previousChange = changes_.AllEvents()[0].data;
			first = false;
		}

		// Then continue normally...
		beat_pos_t pos = first ? (0.0 * score::beats) : previousChange.GetScorePositionAt(timestamp).position();
		first = false;

		LOG("Tempo change, timestamp: %1%, pos: %2%, tempo: %3%", timestamp, pos, tempo);
		TimeSignature meter = GetMeterAt(timestamp);
		previousChange = TempoChange(timestamp, ScorePosition(timestamp, pos, tempo, meter));
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
		tempo_t tempo(120.0 * score::beats_per_minute);
		TimeSignature meter = TimeSignature(4, 4);
		ScorePosition tp(0.0 * score::seconds, 0.0 * score::beats, tempo, meter);
		changes_.RegisterEvent(0.0 * score::seconds, TempoChange(0.0 * score::seconds, tp));
	}
}

/****** TempoChange ******/

TempoMap::TempoChange::TempoChange(score_time_t const & timestamp, ScorePosition const & position)
	: timestamp_(timestamp)
	, position_(position)
{
}

ScorePosition
TempoMap::TempoChange::GetScorePositionAt(score_time_t const & time) const
{
	beat_pos_t position = position_.BeatPositionAt(time);
	return ScorePosition(time, position, position_.tempo(), position_.meter());
}

} // namespace ScoreFollower
} // namespace cf
