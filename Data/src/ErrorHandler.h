#pragma once

//#include "foo.h" // LOG()

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
		//LOG(ss.str());
		std::cout << ss.str() << std::endl;
	}
};

} // namespace Data
} // namespace cf
