#pragma once

#include "cf/globals.h"

namespace cf {
namespace Data {

template<typename Iterator>
struct error_handler_impl
{
	template <typename, typename, typename>
	struct result { typedef void type; };

	void operator()(qi::info const & what, Iterator const & whereBegin, Iterator const & whereEnd) const
	{
		// This need not be realtime safe
		std::ostringstream ss;
		ss << "* Parse error, expecting "
			<< what
			<< " here "
			<< std::string(whereBegin, whereEnd);
		LOG(ss.str().c_str());
	}
};

} // namespace Data
} // namespace cf
