#include <algorithm>

namespace cf {

// lower_bound is "first not less than", this is "last less than or equal"
template<typename ForwardIterator, typename T>
ForwardIterator lower_bound_inclusive(ForwardIterator first, ForwardIterator last, T const & value)
{
	ForwardIterator it = std::lower_bound(first, last, value);
	
	if (it == last) { return --it; } // All items are smaller, return last valid one
	// *it >= value at this stage
	if (!(value < *it)) { return it; } // Equal (use only operator<)
	if (it == first) { return last; } // None less than or equal found
	return --it; // This is the "default" case
}

} // namespace cf
