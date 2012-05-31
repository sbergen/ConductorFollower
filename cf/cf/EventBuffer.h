#pragma once

#include <functional>
#include <algorithm>
#include <numeric>
#include <exception>

#include <boost/circular_buffer.hpp>
#include <boost/range/iterator_range.hpp>

namespace cf {
namespace FeatureExtractor {

template<typename TData, typename TTimestamp,
	template<typename, typename> class TContainer = boost::circular_buffer, template<typename> class TAlloc = std::allocator>
class EventBuffer
{
public:

	typedef TContainer<TData, typename TAlloc<TData> > DataBuffer;
	typedef TContainer<TTimestamp, typename TAlloc<TTimestamp> > TimestampBuffer;

	typedef typename DataBuffer::const_iterator DataIterator;
	typedef typename TimestampBuffer::const_iterator TimestampIterator;

	typedef boost::iterator_range<DataIterator> DataRange;
	typedef boost::iterator_range<TimestampIterator> TimestampRange;

	EventBuffer(size_t size)
		: data_(size)
		, timestamps_(size)
	{
	}

	// Adding, reading data

	void RegisterEvent(TTimestamp const & time, TData data)
	{
		timestamps_.push_back(time);
		data_.push_back(data);
	}

	TData const & DataAt(TTimestamp const & time) const
	{
		auto range = std::equal_range(timestamps_.begin(), timestamps_.end(), time);
		if (range.first == timestamps_.end() || (range.first == range.second))
		{
			throw std::out_of_range("Invalid timestamp given to EventBuffer::DataAt()");
		}
		return *ToDataIterator(range.first);
	}
	
	DataRange
	DataSince(TTimestamp const & time) const
	{
		return DataSince<DataRange>(time);
	}

	template<typename T>
	T DataSince(TTimestamp const & time) const
	{
		return T(DataLowerBound(time), data_.end());
	}

	template<template<typename> class T>
	T<typename DataIterator> DataSince(TTimestamp const & time) const
	{
		return T<DataIterator>(DataLowerBound(time), data_.end());
	}

	DataRange
	DataBetween(TTimestamp const & from, TTimestamp const & to) const
	{
		return DataBetween<DataRange>(from, to);
	}

	template<typename T>
	T DataBetween(TTimestamp const & from, TTimestamp const & to) const
	{
		return T(DataLowerBound(from), DataUpperBound(to));
	}

	template<template<typename> class T>
	T<DataIterator> DataBetween(TTimestamp const & from, TTimestamp const & to) const
	{
		return T<DataIterator>(DataLowerBound(from), DataUpperBound(to));
	}

	// Timestamps

	TTimestamp LastTimestamp() const
	{
		if (timestamps_.empty()) { return TTimestamp(); }
		return timestamps_.back();
	}

	TimestampRange
	TimestampsSince(TTimestamp const & time) const
	{
		return TimestampsSince<TimestampRange>(time);
	}

	template<typename T>
	T TimestampsSince(TTimestamp const & time) const
	{
		return T(TimestampLowerBound(time), timestamps_.end());
	}

	TimestampRange
	TimestampsBetween(TTimestamp const & from, TTimestamp const & to) const
	{
		return TimestampsBetween<TimestampRange>(from, to);
	}

	template<typename T>
	T TimestampsBetween(TTimestamp const & from, TTimestamp const & to) const
	{
		return T(TimestampLowerBound(from), TimestampUpperBound(to));
	}

	// Iterator "conversion"

	DataIterator ToDataIterator(TimestampIterator const & it) const
	{
		typename TimestampBuffer::difference_type diff = it - timestamps_.begin();
		return data_.begin() + diff;
	}

	TimestampIterator ToTimestampIterator(DataIterator const & it) const
	{
		typename DataBuffer::difference_type diff = it - data_.begin();
		return timestamps_.begin() + diff;
	}

	// Checks

	bool ContainsDataAfter(TTimestamp const & since) const
	{
		if (timestamps_.empty()) { return false; }
		return (timestamps_.front()) <= since && (timestamps_.back() >= since);
	}

private:

	// Lower bound

	typename TimestampIterator TimestampLowerBound(TTimestamp const & since) const
	{
		return std::lower_bound(timestamps_.begin(), timestamps_.end(), since);
	}

	typename DataIterator DataLowerBound(TTimestamp const & since) const
	{
		return ToDataIterator(TimestampLowerBound(since));
	}

	// Upper bound

	typename TimestampIterator TimestampUpperBound(TTimestamp const & since) const
	{
		return std::upper_bound(timestamps_.begin(), timestamps_.end(), since);
	}

	typename DataIterator DataUpperBound(TTimestamp const & since) const
	{
		return ToDataIterator(TimestampUpperBound(since));
	}

private: // Data
	DataBuffer data_;
	TimestampBuffer timestamps_;

};

} // namespace cf
} // namespace FeatureExtractor
