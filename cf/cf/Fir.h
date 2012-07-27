#pragma once

#include <numeric>

#include <boost/array.hpp>
#include <boost/circular_buffer.hpp>

namespace cf {

template<int Order, typename T = double>
class Fir
{
public:
	Fir()
		: sampleBuffer_(Order)
	{
		sampleBuffer_.assign(Order, T());
		coefs_.assign(T());
	}

	Fir(T const & coefValue)
		: sampleBuffer_(Order)
	{
		sampleBuffer_.assign(Order, T());
		coefs_.assign(coefValue);
	}

	T Run(T const & nextValue)
	{
		sampleBuffer_.push_back(nextValue);
		return std::inner_product(std::begin(coefs_), std::end(coefs_), std::begin(sampleBuffer_), T());
	}

	T & operator[](std::size_t i)
	{
		return coefs_[i];
	}

private:
	boost::circular_buffer<T> sampleBuffer_;
	boost::array<T, Order> coefs_;
};

template<int Order, typename T = double>
class AveragingFir
{
public:
	AveragingFir()
		: sampleBuffer_(Order)
	{
		sampleBuffer_.assign(Order, T());
	}

	T Run(T const & nextValue)
	{
		sampleBuffer_.push_back(nextValue);
		T sum = std::accumulate(std::begin(sampleBuffer_), std::end(sampleBuffer_), T());
		return sum / T(Order);
	}

private:
	boost::circular_buffer<T> sampleBuffer_;
};

} // namespace cf
