#pragma once

#include <valarray>


namespace cf {

template<int Length, typename T, T (std::valarray<T>::*Op)() const>
class Holder
{
public:
	Holder()
		: pos_(0)
		, buffer_(Length)
	{}

	T Run(T const & value)
	{
		buffer_[pos_] = value;
		pos_ = (pos_ + 1) % Length;
		return (buffer_.*Op)();
	}

private:
	std::size_t pos_;
	std::valarray<T> buffer_;
};

template<int Length, typename T = double>
class PeakHolder : public Holder<Length, T, &std::valarray<T>::max>
{};

template<int Length, typename T = double>
class DipHolder : public Holder<Length, T, &std::valarray<T>::min>
{};

} // namespace cf
