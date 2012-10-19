#pragma once

#include <list>

#include <boost/units/io.hpp>
#include <boost/utility.hpp>

#include <cf/bounded_vector.h>

#include "Data/BeatPattern.h"

#include "ScoreFollower/types.h"

#include "BeatScorer.h"

namespace cf {
namespace ScoreFollower {

class BeatPattern
{
public:
	enum { MaxBeats = 16 };
	typedef bounded_vector<beat_pos_t, MaxBeats> beat_array;

private:
	typedef std::list<BeatScorer> scorer_array; // Scorer not default constructible

public:
	BeatPattern(Data::BeatPattern const & pattern);
	
	double MatchQuality(beat_array const & beats, double scale) const;
	beat_pos_t OffsetToBest(beat_pos_t const & pos) const;

private:
	beat_pos_t ScaleWithPivot(beat_pos_t pos, beat_pos_t pivot, double scale) const;
	double MatchScaled(beat_array const & beats) const;
	void AdvanceScorer(scorer_array::const_iterator & it,
		beat_pos_t const & pos, beat_pos_t & barOffset,
		double & score, bool first) const;

private:
	TimeSignature meter_;
	scorer_array scorers_;
};
	
} // namespace ScoreFollower
} // namespace cf
