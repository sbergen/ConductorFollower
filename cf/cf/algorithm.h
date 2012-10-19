#pragma once

#include <algorithm>
#include <functional>
#include <iterator>

namespace cf {

// lower_bound_inclusive is "last less than or equal" (lower_bound is "first not less than")

template<typename ForwardIterator, typename T, typename Compare>
ForwardIterator lower_bound_inclusive(ForwardIterator first, ForwardIterator last, T const & value, Compare compare)
{
	if (first == last) { return last; }

	ForwardIterator it = std::lower_bound(first, last, value, compare);
	
	if (it == last) { return --it; } // All items are smaller, return last valid one
	// *it >= value at this stage
	if (!compare(value, *it)) { return it; } // Equal (use only operator<)
	if (it == first) { return last; } // None less than or equal found
	return --it; // This is the "default" case
}

template<typename ForwardIterator, typename T>
ForwardIterator lower_bound_inclusive(ForwardIterator first, ForwardIterator last, T const & value)
{
	return lower_bound_inclusive(first, last, value,
		std::less<typename std::iterator_traits<ForwardIterator>::value_type>());
}

// linear nearest neighbour search
// Distance::operator() params: (itemBeingEvaluated, targetItem)
template<typename ForwardIterator, typename T, typename Distance>
ForwardIterator nearest_neighbour_linear(ForwardIterator first, ForwardIterator last, T const & value, Distance distance)
{
	if (first == last) { return last; }

	// Init
	ForwardIterator it = first;
	auto leastDistance = distance(*it, value);
	ForwardIterator best = it;

	// Go through rest
	while (++it != last) {
		auto dist = distance(*it, value);
		if (dist < leastDistance) {
			best = it;
			leastDistance = dist;
		}
	}

	return best;
}

template<typename FwdIt, typename Func>
std::pair<FwdIt, double>
max_score(FwdIt first, FwdIt last, Func evaluator)
{
	double bestScore = std::numeric_limits<double>::lowest();
	FwdIt best = last;

	if (first == last) { return std::make_pair(best, bestScore); }

	for (auto it = first; it != last; ++it) {
		auto score = evaluator(*it);
		if (score > bestScore) {
			bestScore = score;
			best = it;
		}
	}

	return std::make_pair(best, bestScore);
}

} // namespace cf
