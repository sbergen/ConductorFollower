#include <boost/test/unit_test.hpp>

#include "src/BarProgressEstimator.h"
#include "src/BeatClassification.h"


using namespace cf;
using namespace cf::ScoreFollower;

inline real_time_t make_time(real_time_t ref, time_quantity offset)
{
	return ref + time_cast<real_time_t::duration>(offset);
}

inline Data::BeatPattern make_quarter_pattern()
{
	Data::BeatPattern pattern;
	pattern.meter = TimeSignature(4, 4);
	
	Data::Beat beat;

	beat.time = 0.0;
	pattern.beats.push_back(beat);

	beat.time = 1.0;
	pattern.beats.push_back(beat);

	beat.time = 2.0;
	pattern.beats.push_back(beat);

	beat.time = 3.0;
	pattern.beats.push_back(beat);

	return pattern;
}

inline Data::BeatPattern make_half_pattern()
{
	Data::BeatPattern pattern;
	pattern.meter = TimeSignature(4, 4);
	
	Data::Beat beat;

	beat.time = 0.0;
	pattern.beats.push_back(beat);

	beat.time = 2.0;
	pattern.beats.push_back(beat);

	return pattern;
}
