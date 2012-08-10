#pragma once

#include <boost/utility.hpp>

namespace cf {

template<class Archive>
class PairArchiver : public boost::noncopyable
{
public:
	PairArchiver(Archive & ar) : ar_(ar) {}
	PairArchiver(PairArchiver const & other) : ar_(other.ar_) {}

	template <typename Pair>
	void operator()(Pair & data) const
	{
		ar_ & data.second;
	}

private:
	Archive & ar_;
};

} // namespace cf
