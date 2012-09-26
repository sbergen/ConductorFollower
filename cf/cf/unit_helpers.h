#include <boost/units/quantity.hpp>
#include <boost/units/derived_dimension.hpp>

namespace cf
{

template<typename U1, typename U2, typename Rep>
struct multiplied_quantity
{
	typedef boost::units::quantity<typename boost::units::multiply_typeof_helper<U1, U2>::type, Rep> type;
};

template<typename U1, typename U2, typename Rep, typename Func>
typename multiplied_quantity<U1, U2, Rep>::type
unit_integral(boost::units::quantity<U2, Rep> const & from, boost::units::quantity<U2, Rep> const & to, Func const & f)
{
	Rep l1 = f(from.value());
	Rep l2 = f(to.value());
	return multiplied_quantity<U1, U2, Rep>::type::from_value(l2 - l1);
}

template<typename U1, typename U2, typename Rep, typename Func>
typename multiplied_quantity<U1, U2, Rep>::type
unit_integral(boost::units::quantity<U2, Rep> const & to, Func const & f)
{
	Rep l2 = f(to.value());
	return multiplied_quantity<U1, U2, Rep>::type::from_value(l2);
}

} // namespace cf