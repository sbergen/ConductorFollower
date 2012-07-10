#pragma once

namespace cf {

template<class Archive>
struct PairArchiver
{
	PairArchiver(Archive & ar) : ar_(ar) {}

	template <typename Pair>
	void operator()(Pair & data) const
	{
		ar_ & data.second;
	}

private:
	Archive & ar_;
};

} // namespace cf
