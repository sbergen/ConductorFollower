#pragma once

#include <utility>

#include <boost/array.hpp>
#include <boost/function.hpp>

#include "cf/globals.h"
#include "cf/units_math.h"

#include "ScoreFollower/types.h"

#include "ScorePosition.h"

namespace cf {
namespace ScoreFollower {

class BeatClassification
{
public:
	enum Type
	{
		NotClassified,
		CurrentBar,
		NextBar
	};

public:
	BeatClassification()
		: timestamp_(timestamp_t::min())
		, type_(NotClassified)
		, position_(ScorePosition())
		, offset_(0.0 * score::beats)
		, quality_(std::numeric_limits<double>::lowest())
	{}

	BeatClassification(timestamp_t timestamp, ScorePosition position)
		: timestamp_(timestamp)
		, type_(NotClassified)
		, position_(position)
		, offset_(0.0 * score::beats)
		, quality_(std::numeric_limits<double>::lowest())
	{}

	BeatClassification(timestamp_t timestamp, ScorePosition position, Type type, beat_pos_t offset, double quality)
		: timestamp_(timestamp)
		, type_(type)
		, position_(position)
		, offset_(offset)
		, quality_(quality)
	{}

	timestamp_t timestamp() const { return timestamp_; }
	Type type() const { return type_; }
	ScorePosition const & position() const { return position_; }
	beat_pos_t offset() const { return offset_; }
	double quality() const { return quality_; }

	beat_pos_t IntendedPosition() const { return position_.position() - offset_; }
	
	beat_pos_t IntendedBeginningOfBar() const
	{
		// Prevent getting position backwards in time
		auto positionDefinitelyInBar = (offset_.value() > 0.0) ? position_.position() : IntendedPosition();
		return position_.ScorePositionAt(positionDefinitelyInBar, ScorePosition::RoundToBeat).BeginningOfThisBar();
	}

	bars_t IntendedBar() const
	{
		// Prevent getting position backwards in time
		if (offset_.value() > 0.0) { return position_.bar(); }
		return position_.ScorePositionAt(IntendedPosition(), ScorePosition::RoundToBeat).bar();
	}

	operator bool() const { return type_ != NotClassified; }

private:
	// non-const for assignability
	timestamp_t timestamp_;
	Type type_;
	ScorePosition position_;
	beat_pos_t offset_;
	double quality_;
};


} // namespace ScoreFollower
} // namespace cf
