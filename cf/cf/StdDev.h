#pragma once

#include <numeric>
#include <valarray>

namespace cf {

template<int Order, typename T = double>
class StdDev
{
public:
	StdDev()
		: sampleBuffer_(Order)
		, pos_(0)
		, sum_(T())
	{}

	T Run(T const & nextValue)
	{
		// Update sum
		sum_ -= sampleBuffer_[pos_];
		sum_ += nextValue;

		// Update buffer
		sampleBuffer_[pos_] = nextValue;
		pos_ = (pos_ + 1) % Order;

		auto avg = sampleBuffer_.sum() / T(Order);
		auto sum = std::accumulate(std::begin(sampleBuffer_), std::end(sampleBuffer_), T(),
			[avg](T const & sum, T const &value ) -> T
			{
				auto diff = value - avg;
				return sum + (diff * diff);
			});
		return std::sqrt(sum / T(Order));
	}

private:
	std::valarray<T> sampleBuffer_;
	std::size_t pos_;
	T sum_;
};

} // namespace cf
