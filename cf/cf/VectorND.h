#pragma once

#include <boost/units/quantity.hpp>
#include <boost/numeric/ublas/storage.hpp>
#include <boost/numeric/ublas/vector.hpp>

#include "vector_helpers.h"

namespace cf {

namespace ublas = boost::numeric::ublas;
namespace mpl = boost::mpl;

namespace coord
{
	enum
	{
		X = 0,
		Y = 1,
		Z = 2
	};

} // namespace coord

// N-dimensional vector
template<unsigned Dim, typename Unit, typename Rep = double>
class VectorND
{
public:
	enum { dimension = Dim };
	typedef Rep raw_type;
	typedef boost::units::quantity<Unit, Rep> quantity;
	typedef ublas::vector<raw_type, ublas::bounded_array<raw_type, Dim> > data_type;

	template<typename T>
	struct transformed
	{
		typedef VectorND<Dim, T, Rep> type;
	};

public: // ctors
	VectorND() : data_(3) { }

	// Shorthand for 3D
	VectorND(quantity x, quantity y, quantity z)
		 : data_(3)
	{
		set<coord::X>(x);
		set<coord::Y>(y);
		set<coord::Z>(z);
	}

	VectorND(data_type const & data)
		: data_(data) {}

public:
	// Quantity
	template<unsigned N>
	quantity get() const { return quantity::from_value(get_raw<N>()); }

	template<unsigned N>
	void set(quantity const & val) { set_raw<N>(val.value()); }

	// Raw
	template<unsigned N>
	raw_type get_raw() const { return data_(N); }

	template<unsigned N>
	void set_raw(raw_type const & val) { data_(N) = val; }

public: // Access to iteratable data
	data_type const & data() const { return data_; }
	data_type & data() { return data_; }

public: // Dimensionally safe transforms and other operations

	template<typename T, typename F>
	void transform(T & result, F f) const
	{
		mpl::for_each<mpl::range_c<unsigned, 0, Dim> , detail::indexer<mpl::_1> >
			(detail::transformer<VectorND, T, F>(*this, result, f));
	}

	quantity accumulate(quantity initial = quantity()) const
	{
		quantity result(initial);
		mpl::for_each<mpl::range_c<unsigned, 0, Dim> , detail::indexer<mpl::_1> >
			(detail::accumulator<VectorND, quantity>(*this, result));
		return result;
	}

private:
	data_type data_;
};

} // namespace cf
