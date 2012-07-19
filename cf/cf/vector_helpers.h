#pragma once

#include <boost/mpl/range_c.hpp>
#include <boost/mpl/for_each.hpp>

namespace cf {
namespace detail {

template<typename I>
struct indexer {
	template<typename T, typename U, typename F>
	static void CopyTransformed(T const & from, U & to, F f)
	{ 
		to.set<I::value>(f(from.get<I::value>()));
	}

	template<typename T>
	static typename T::quantity Get(T const & t) { return t.get<I::value>(); }
};

template<typename From, typename To, typename F>
struct transformer {
	transformer(From const & from, To & to, F f)
		: from(from), to(to), f(f) {}

	template<typename Indexer>
	void operator() (Indexer const &) const
	{
		Indexer::CopyTransformed(from, to, f);
	}

	From const & from;
	To & to;
	F f;
};

template<typename From, typename To>
struct accumulator {
	accumulator(From const & from, To & to)
		: from(from), to(to) {}

	template<typename Indexer>
	void operator() (Indexer const &) const
	{
		to += Indexer::Get(from);
	}

	From const & from;
	To & to;
};


} // namespace detail
} // namespace cf