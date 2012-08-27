#pragma once

#include <stdexcept>

#include "cf/score_units.h"

namespace cf {

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

	typedef boost::units::quantity<score::bar_duration> bar_duration_t;
	bar_duration_t BarDuration() const
	{
		double fullNotes = static_cast<double>(count_) / division_;
		return bar_duration_t((fullNotes * score::full_notes) / score::bar);
	}

public: // For ordering
	bool operator< (TimeSignature const & other) const
	{
		return (division_ != other.division_) ? 
			division_ < other.division_ :
			count_ < other.count_;
	}

private:
	unsigned count_;
	unsigned division_;
};

} // namespace cf