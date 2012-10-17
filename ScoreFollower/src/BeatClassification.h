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
		: type_(NotClassified)
		, position_(ScorePosition())
		, offset_(0.0 * score::beats)
		, quality_(std::numeric_limits<double>::lowest())
	{}

	BeatClassification(ScorePosition position)
		: type_(NotClassified)
		, position_(position)
		, offset_(0.0 * score::beats)
		, quality_(std::numeric_limits<double>::lowest())
	{}

	BeatClassification(ScorePosition position, Type type, beat_pos_t offset, double quality)
		: type_(type)
		, position_(position)
		, offset_(offset)
		, quality_(quality)
	{}

	Type type() const { return type_; }
	ScorePosition const & position() const { return position_; }
	beat_pos_t offset() const { return offset_; }
	double quality() const { return quality_; }

	beat_pos_t IntendedPosition() { return position_.position() - offset_; }

	operator bool() const { return type_ != NotClassified; }

private:
	// non-const for assignability
	Type type_;
	ScorePosition position_;
	beat_pos_t offset_;
	double quality_;
};


} // namespace ScoreFollower
} // namespace cf
