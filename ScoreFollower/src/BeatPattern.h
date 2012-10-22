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

class BeatClassification;

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
	bool IsConfidentEstimate(BeatClassification const & classification) const;

private:
	struct IterationHelper
	{
		IterationHelper(BeatPattern const & parent)
			: parent(parent)
			, it(parent.scorers_.begin())
			, barOffset(0.0 * score::beats)
			, score(0.0)
			, first(true)
		{}

		void Advance(beat_pos_t const & pos);

		BeatPattern const & parent;
		scorer_array::const_iterator it;
		beat_pos_t barOffset;
		double score;
		bool first;
	};

private:
	beat_pos_t ScaleWithPivot(beat_pos_t pos, beat_pos_t pivot, double scale) const;
	double MatchScaled(beat_array const & beats) const;
	
	template<typename It>
	void WrappingAdvance(It & it, int count) const
	{
		while (count > 0) {
			++it; --count;
			if (it == scorers_.end()) { it = scorers_.begin(); }
		}
		
		while (count < 0) {
			if (it == scorers_.begin()) { it = scorers_.end(); }
			--it; ++count;
		}
	}

private:
	TimeSignature meter_;
	scorer_array scorers_;
};
	
} // namespace ScoreFollower
} // namespace cf
