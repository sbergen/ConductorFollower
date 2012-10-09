#include "TempoMap.h"

#include <typeinfo>
#include <vector>

#include <boost/variant/apply_visitor.hpp>

#include "cf/globals.h"
#include "cf/TimeSignature.h"

#include "ScoreFollower/TrackReader.h"

namespace cf {
namespace ScoreFollower {

namespace {

typedef boost::variant<TimeSignature, tempo_t> EventVariant;

struct EventPair {
	EventPair(score_time_t const & time, EventVariant const & event)
		: time(time), event(event)
	{}

	score_time_t time;
	EventVariant event;
};

typedef std::list<EventPair> EventList;

struct EventComparer
{
	bool operator() (EventPair const & lhs, EventPair const & rhs)
	{
		if (lhs.time < rhs.time) {
			return true;
		} else if (rhs.time < lhs.time) {
			return false;
		} else {
			// Meter before tempo
			return (lhs.event.type() == typeid(TimeSignature));
		}
	}
};

template<typename Event, typename ReaderPtr>
void ReadFromReader(ReaderPtr reader, EventList & list)
{
	Event event;
	score_time_t timestamp;

	while (reader->NextEvent(timestamp, event)) {
		list.push_back(EventPair(timestamp, event));
	}
}

template<typename Map>
class ChangeBuilder
{
public:
	typedef void result_type;

	ChangeBuilder(Map & map, score_time_t const & time, ScorePosition const & previousPosition)
		: map_(map)
		, time_(time)
		, previousPosition_(previousPosition)
	{}

	void operator() (TimeSignature const & meter) const
	{
		LOG("Meter change to %1%/%2% at %3%",
			meter.count(), meter.division(), time_);

		map_.RegisterEvent(
			time_, previousPosition_.ChangeAt(time_, meter));
	}

	void operator() (tempo_t const & tempo) const
	{
		LOG("Tempo change to %1% at %2%",
			tempo, time_);

		map_.RegisterEvent(
			time_, previousPosition_.ChangeAt(time_, tempo));
	}

	void operator() (TimeSignature const & meter, tempo_t const & tempo) const
	{
		LOG("Meter and tempo change to %1%/%2% @ %3% at %4%",
			meter.count(), meter.division(), tempo, time_);

		map_.RegisterEvent(
			time_, previousPosition_.ChangeAt(time_, meter, tempo));
	}

	void operator() (tempo_t const & tempo, TimeSignature const & meter) const
	{
		(*this)(meter, tempo);
	}

	void operator() (TimeSignature const & meter, TimeSignature const & meter2) const
	{
		assert(false);
	}

	void operator() (tempo_t const & tempo, tempo_t const & tempo2) const
	{
		assert(false);
	}

private:
	Map & map_;
	score_time_t time_;
	ScorePosition previousPosition_;
};

template<typename Map>
ChangeBuilder<Map> MakeChangeBuilder(Map & map, score_time_t const & time)
{
	ScorePosition previousPosition;
	if (!map.AllEvents().Empty()) {
		previousPosition = map.AllEvents().Back().data;
	}

	return ChangeBuilder<Map>(map, time, previousPosition);
}

} // anon namespace

TempoMap::TempoMap()
{
}

void
TempoMap::ReadScore(ScoreReader & reader)
{
	EventList list;
	ReadFromReader<TimeSignature>(reader.MeterTrack(), list);
	ReadFromReader<tempo_t>(reader.TempoTrack(), list);
	list.sort(EventComparer());

	// If empty set defaults and return
	if (list.empty()) {
		EnsureChangesNotEmpty();
		return;
	}

	// otherwise check the first time stamp
	// if it is not at the beginning, add defaults to beginning
	if (list.front().time > 0.0 * score::seconds) {
		EnsureChangesNotEmpty();
	}

	// Then iterate through the rest, note that begin() is 
	// guaranteed to be valid at this stage
	while (!list.empty()) {
		auto first = list.front();
		list.pop_front();

		// Peek at next one
		if (!list.empty() && list.front().time == first.time) {
			auto second = list.front();
			list.pop_front();
			boost::apply_visitor(
				MakeChangeBuilder(changes_, first.time),
				first.event, second.event);
		} else {
			boost::apply_visitor(
				MakeChangeBuilder(changes_, first.time),
				first.event);
		}
	}
}

ScorePosition
TempoMap::GetScorePositionAt(beat_pos_t const & absoluteBeatPosition, ScorePosition::Rounding rounding, score_time_t timeHint) const
{
	ScorePosition nearestChange;
	auto changes = changes_.EventsSinceInclusive(timeHint);
	changes.ForEachWhile(
		[&nearestChange, absoluteBeatPosition](score_time_t const & time, ScorePosition const & position) -> bool
		{
			if (position.position() > absoluteBeatPosition) { return false; }
			nearestChange = position;
			return true;
		});

	return nearestChange.ScorePositionAt(absoluteBeatPosition, rounding);
}

ScorePosition
TempoMap::GetScorePositionAt(score_time_t const & time) const
{
	if (changes_.AllEvents().Empty()) { return ScorePosition(); }

	// Scpecial case for before the start
	if (time < 0.0 * score::seconds) {
		return changes_.AllEvents().Front().data.ScorePositionAt(time);
	}

	auto range = changes_.EventsSinceInclusive(time);
	assert(!range.Empty());
	return range[0].data.ScorePositionAt(time);
}

TimeSignature
TempoMap::GetMeterAt(score_time_t const & time) const
{
	// Scpecial case for before the start
	if (time < 0.0 * score::seconds) {
		return changes_.AllEvents().Front().data.meter();
	}

	auto range = changes_.EventsSinceInclusive(time);
	assert(!range.Empty());
	return range[0].data.meter();
}

score_time_t
TempoMap::TimeAt(Data::ScorePosition const & pos) const
{
	return TranslatePosition(pos).time();
}

ScorePosition
TempoMap::TranslatePosition(Data::ScorePosition const & pos) const
{
	auto bar = pos.bar * score::bars;
	auto beat = pos.beat * score::beats;

	ScorePosition closestChange;
	changes_.AllEvents().ForEachWhile(
		[&closestChange, bar, beat](score_time_t const &, ScorePosition const & position) -> bool
		{
			if (position.bar() > bar ||
				(position.bar() == bar && position.beat() > beat))
			{
				return false;
			}
			closestChange = position;
			return true;

		});

	return closestChange.ScorePositionAt(bar, beat);
}

void
TempoMap::EnsureChangesNotEmpty()
{
	if (changes_.AllEvents().Empty()) {
		changes_.RegisterEvent( 0.0 * score::seconds, ScorePosition());
	}
}

} // namespace ScoreFollower
} // namespace cf
