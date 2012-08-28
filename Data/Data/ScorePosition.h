#pragma once

namespace cf {
namespace Data {

struct ScorePosition
{
	int bar;
	double beat;
};

inline bool operator< (ScorePosition const & lhs, ScorePosition const & rhs)
{
	if (lhs.bar != rhs.bar) { return lhs.bar < rhs.bar; }
	return lhs.beat < rhs.beat;
}

} // namespace Data
} // namespace cf
