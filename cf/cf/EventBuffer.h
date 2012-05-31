#pragma once

#include <functional>
#include <algorithm>
#include <numeric>
#include <exception>

#include <boost/circular_buffer.hpp>
#include <boost/range/iterator_range.hpp>

namespace cf {
namespace FeatureExtractor {

template<typename TData, typename TTimestamp>
class EventBuffer
{
public:

	typedef boost::circular_buffer<TData> DataBuffer;
	typedef boost::circular_buffer<TTimestamp> TimestampBuffer;

	typedef boost::iterator_range<typename DataBuffer::const_iterator> DataRange;
	typedef boost::iterator_range<typename TimestampBuffer::const_iterator> TimestampRange;

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
		return *DataIterator(range.first);
	}
	
	DataRange
	DataSince(TTimestamp const & time) const
	{
		return DataSince<DataRange>(time);
	}

	template<typename T>
	T DataSince(TTimestamp const & time) const
	{
		return T(FirstDataForPeriod(time), data_.end());
	}

	template<template<typename> class T>
	T<typename DataBuffer::const_iterator> DataSince(TTimestamp const & time) const
	{
		return T<typename DataBuffer::const_iterator>(FirstDataForPeriod(time), data_.end());
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
		return T(FirstForPeriod(time), timestamps_.end());
	}

	// Timestamp for data iterator and data for timestamp iterator

	TTimestamp const & TimestampForData(typename DataBuffer::const_iterator const & it) const
	{
		return *TimestampIterator(it);
	}

	TData const &  DataForTimestamp(typename TimestampBuffer::const_iterator const & it) const
	{
		return *DataIterator(it);
	}

	// Checks

	bool ContainsDataAfter(TTimestamp const & since) const
	{
		if (timestamps_.empty()) { return false; }
		return (timestamps_.front()) <= since && (timestamps_.back() >= since);
	}

private:

	// First for period

	typename TimestampBuffer::const_iterator FirstForPeriod(TTimestamp const & since) const
	{
		return std::lower_bound(timestamps_.begin(), timestamps_.end(), since);
	}

	typename DataBuffer::const_iterator FirstDataForPeriod(TTimestamp const & since) const
	{
		return DataIterator(FirstForPeriod(since));
	}

	// Iterator "conversion"

	typename DataBuffer::const_iterator DataIterator(typename TimestampBuffer::const_iterator const & it) const
	{
		typename TimestampBuffer::difference_type diff = it - timestamps_.begin();
		return data_.begin() + diff;
	}

	typename TimestampBuffer::const_iterator TimestampIterator(typename DataBuffer::const_iterator const & it) const
	{
		typename DataBuffer::difference_type diff = it - data_.begin();
		return timestamps_.begin() + diff;
	}

private: // Data
	DataBuffer data_;
	TimestampBuffer timestamps_;

};

} // namespace cf
} // namespace FeatureExtractor
