#pragma once

#include <stdexcept>
#include <string>

#include <boost/format.hpp>

#include "ScoreFollower/types.h"

namespace cf {
namespace ScoreFollower {

class TimeSignature
{
public:
	// Default to 4/4
	TimeSignature()
		: count_(4)
		, division_(4)
	{}

	TimeSignature(unsigned count, unsigned division)
		: count_(count)
		, division_(division)
	{
		// we don't support anything too funky
		if (division % 2 != 0) {
			throw std::runtime_error("Invalid division in TimeSignature");
		}
	}

	unsigned count() const { return count_; }
	unsigned division() const { return division_; }

	bar_duration_t BarDuration() const
	{
		return bar_duration_t(((count_ * score::full_notes) / division_) / score::bar);
	}

	std::string toString() const
	{
		return (boost::format("%1%/%2%") % count_ % division_).str();
	}

private:
	unsigned count_;
	unsigned division_;
};


} // namespace ScoreFollower
} // namespace cf