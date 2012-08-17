#include "TempoMap.h"

#include <typeinfo>

#include <boost/variant/apply_visitor.hpp>

#include "cf/globals.h"
#include "cf/TimeSignature.h"

#include "ScoreFollower/TrackReader.h"

namespace cf {
namespace ScoreFollower {

namespace {

typedef boost::variant<TimeSignature, tempo_t> EventVariant;
typedef std::pair<score_time_t, EventVariant> EventPair;
typedef std::vector<EventPair> EventVector;

struct EventComparer
{
	bool operator() (EventPair const & lhs, EventPair const & rhs)
	{
		if (lhs.first < rhs.first) {
			return true;
		} else if (rhs.first < lhs.first) {
			return false;
		} else {
			// Meter before tempo
			return (lhs.second.type() == typeid(TimeSignature));
		}
	}
};

template<typename Event, typename ReaderPtr>
void ReadFromReader(ReaderPtr reader, EventVector & vector)
{
	Event event;
	score_time_t timestamp;

	while (reader->NextEvent(timestamp, event)) {
		vector.push_back(EventPair(timestamp, event));
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
	EventVector vector;
	ReadFromReader<TimeSignature>(reader.MeterTrack(), vector);
	ReadFromReader<tempo_t>(reader.TempoTrack(), vector);
	std::sort(std::begin(vector), std::end(vector), EventComparer());

	if (vector.front().first > 0.0 * score::seconds) {
		EnsureChangesNotEmpty();
	}

	EventVector::iterator previous = vector.begin();
	for (auto it = previous + 1; it != vector.end(); ++it) {
		if (it->first > previous->first) {
			boost::apply_visitor(
				MakeChangeBuilder(changes_, previous->first),
				previous->second);
		} else if (it == vector.end() - 1) {
			boost::apply_visitor(
				MakeChangeBuilder(changes_, it->first),
				it->second);
		} else {
			boost::apply_visitor(
				MakeChangeBuilder(changes_, previous->first),
				previous->second,
				it->second);
		}

		previous = it;
	}
}

ScorePosition
TempoMap::GetScorePositionAt(score_time_t const & time) const
{
	auto range = changes_.EventsSinceInclusive(time);
	assert(!range.Empty());
	return range[0].data.ScorePositionAt(time);
}

TimeSignature
TempoMap::GetMeterAt(score_time_t const & time) const
{
	auto range = changes_.EventsSinceInclusive(time);
	assert(!range.Empty());
	return range[0].data.meter();
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
