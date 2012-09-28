#pragma once

namespace cf {

template<typename Coef, typename Val>
struct coef_multiply
{
	Val operator()(Coef const & coef, Val const & val) const
	{
		return coef * val;
	}
};

} // namespace cf
