#pragma once

#include <valarray>


namespace cf {

template<int Length, typename T = double>
class PeakHolder
{
public:
	PeakHolder()
		: pos_(0)
		, buffer_(Length)
	{}

	T Run(T const & value)
	{
		buffer_[pos_] = value;
		pos_ = (pos_ + 1) % Length;
		return buffer_.max();
	}

private:
	std::size_t pos_;
	std::valarray<T> buffer_;
};

} // namespace cf
