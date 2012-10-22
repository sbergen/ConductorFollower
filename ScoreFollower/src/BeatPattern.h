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
private:
	typedef std::list<BeatScorer> scorer_array; // Scorer not default constructible

public:
	enum { MaxBeats = 16 };
	typedef bounded_vector<beat_pos_t, MaxBeats> beat_array;

	class MatchResult
	{
	public:
		MatchResult(BeatPattern const & parent, beat_array const & beats, double scale);

		double quality() const { return quality_; }
		beat_pos_t OffsetToBest(beat_pos_t const & pos) const;
		bool IsConfident(BeatClassification const & classification) const;

	private:
		beat_pos_t ScaleWithPivot(beat_pos_t pos) const;

	private:
		BeatPattern const & parent_;
		double scale_;
		beat_pos_t pivot_;
		double quality_;
	};

public:
	BeatPattern(Data::BeatPattern const & pattern);
	
	MatchResult Match(beat_array const & beats, double scale) const;
	double MatchQuality(beat_array const & beats, double scale) const { return Match(beats, scale).quality(); }

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
