#pragma once

#include <functional>
#include <algorithm>
#include <numeric>
#include <exception>

#include <boost/circular_buffer.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/utility.hpp>

#include "cf/algorithm.h"

namespace cf {

template<typename TData, typename TTimestamp,
	template<typename, typename> class TContainer = boost::circular_buffer, template<typename> class TAlloc = std::allocator>
class EventBuffer : public boost::noncopyable
{
public: // typedefs

	typedef TContainer<TData, typename TAlloc<TData> > DataBuffer;
	typedef TContainer<TTimestamp, typename TAlloc<TTimestamp> > TimestampBuffer;

	typedef typename DataBuffer::const_iterator DataIterator;
	typedef typename TimestampBuffer::const_iterator TimestampIterator;

	typedef boost::iterator_range<DataIterator> DataRange;
	typedef boost::iterator_range<TimestampIterator> TimestampRange;

public: // Range class

	class Range
	{
	private:
		friend class EventBuffer;

		Range(EventBuffer const & parent, TimestampIterator const & begin, TimestampIterator const & end)
			: timestampRange_(begin, end)
			, dataRange_(parent.ToDataIterator(begin), parent.ToDataIterator(end))
		{}

	public:
		struct DataPair
		{
			DataPair(TTimestamp const & timestamp, TData const & data)
				: timestamp(timestamp), data(data) {}

			TTimestamp const & timestamp;
			TData const & data;
		};

	public: // Checks

		typename DataBuffer::size_type Size() const { return  dataRange_.size(); }
		
		bool Empty() const { return dataRange_.empty(); }

		TTimestamp LastTimestamp() const
		{
			if (timestampRange_.empty()) { return TTimestamp(); }
			return timestampRange_.back();
		}

	public: // Data access

		DataPair operator[](size_t i) const 
		{
			return DataPair(timestampRange_[i], dataRange_[i]);
		}

		DataPair Front() const 
		{
			return DataPair(timestampRange_.front(), dataRange_.front());
		}

		DataPair Back() const 
		{
			return DataPair(timestampRange_.back(), dataRange_.back());
		}

	public: // Modifiers

		DataPair PopFront()
		{
			DataPair ret = Front();
			timestampRange_.advance_begin(1);
			dataRange_.advance_begin(1);
			return ret;
		}

		DataPair PopBack()
		{
			DataPair ret = Back();
			timestampRange_.advance_end(-1);
			dataRange_.advance_end(-1);
			return ret;
		}

	public: // Traversal

		// Call func for each (timestamp, data) pair
		template<typename Func>
		void ForEach(Func func) const
		{
			ForEachWhile([&func] (TTimestamp const & ts, TData const & data) -> bool
				{ func(ts, data); return true; });
		}

		// Call func for each (timestamp, data) pair, until it returns a value that converts to false
		template<typename Func>
		void ForEachWhile(Func func) const
		{
			ForEachWhile(func, timestampRange_.begin(), timestampRange_.end(), dataRange_.begin());
		}

		// Reverse versions of the above

		template<typename Func>
		void ReverseForEach(Func & func) const
		{
			ReverseForEachWhile([&func] (TTimestamp const & ts, TData const & data) -> bool
				{ func(ts, data); return true; });
		}

		template<typename Func>
		void ReverseForEachWhile(Func & func) const
		{
			auto tRange = timestampRange_ | boost::adaptors::reversed;
			auto dRange = dataRange_ | boost::adaptors::reversed;
			ForEachWhile(func, tRange.begin(), tRange.end(), dRange.begin());
		}

		// Data container conversion utilities

		template<typename T>
		T DataAs() const { return T(dataRange_.begin(), dataRange_.end()); }
		
		template<template<typename> class T>
		T<DataIterator> DataAs() const { return T<DataIterator>(dataRange_.begin(), dataRange_.end()); }

	private:
		template<typename Func, typename TIterator, typename DIterator>
		void ForEachWhile(Func & func, TIterator tBegin, TIterator tEnd, DIterator dBegin) const
		{
			TIterator tIt = tBegin;
			DIterator dIt = dBegin;
			for(/* inited above */; tIt != tEnd; ++tIt, ++dIt) {
				if (!func(*tIt, *dIt)) { return; }
			}
		}

		TimestampRange timestampRange_;
		DataRange dataRange_;
	};

public: // Main interface

	EventBuffer() {}

	EventBuffer(size_t size)
		: data_(size)
		, timestamps_(size)
	{
	}

	EventBuffer & operator=(Range const & range)
	{
		Clear();
		DataRange data = range.dataRange_;
		TimestampRange timestamps = range.timestampRange_;
		std::copy(data.begin(), data.end(), std::back_inserter(data_));
		std::copy(timestamps.begin(), timestamps.end(), std::back_inserter(timestamps_));
		return *this;
	}

	operator Range () const { return AllEvents(); }

	// Adding, reading data

	void RegisterEvent(TTimestamp const & time, TData const & data)
	{
		timestamps_.push_back(time);
		data_.push_back(data);
	}
	
	Range AllEvents() const
	{
		return Range(*this, timestamps_.begin(), timestamps_.end());
	}

	Range EventsSince(TTimestamp const & time) const
	{
		return Range(*this, LowerBound(time), timestamps_.end());
	}

	Range EventsSinceInclusive(TTimestamp const & time) const
	{
		return Range(*this, LowerBoundInclusive(time), timestamps_.end());
	}

	Range EventsBetween(TTimestamp const & from, TTimestamp const & to) const
	{
		assert(from <= to);
		return Range(*this, LowerBound(from), UpperBound(to));
	}

	Range EventsBetweenInclusive(TTimestamp const & from, TTimestamp const & to) const
	{
		assert(from <= to);
		auto lowerBound = LowerBoundInclusive(from);
		if (lowerBound == timestamps_.end()) {
			return Range(*this, timestamps_.end(), timestamps_.end());
		}
		return Range(*this, lowerBound, UpperBound(to));
	}

	Range LastNumEvnets(int num)
	{
		assert(num > 0);
		assert(timestamps_.size() >= num);
		return Range(*this, timestamps_.end() - num, timestamps_.end());
	}

	// Other stuff...

	bool ContainsDataAfter(TTimestamp const & since) const
	{
		if (timestamps_.empty()) { return false; }
		return (timestamps_.front()) <= since && (timestamps_.back() >= since);
	}

	void Clear()
	{
		data_.clear();
		timestamps_.clear();
	}

private: // Private functions

	TimestampIterator LowerBound(TTimestamp const & time) const
	{
		return std::lower_bound(timestamps_.begin(), timestamps_.end(), time);
	}

	TimestampIterator UpperBound(TTimestamp const & time) const
	{
		return std::upper_bound(timestamps_.begin(), timestamps_.end(), time);
	}

	// LowerBound is "after or at", this is "before or at"
	TimestampIterator LowerBoundInclusive(TTimestamp const & time) const
	{
		return cf::lower_bound_inclusive(timestamps_.begin(), timestamps_.end(), time);
	}

	// Iterator "conversion"

	DataIterator ToDataIterator(TimestampIterator const & it) const
	{
		if (it == timestamps_.end()) { return data_.end(); }
		typename TimestampBuffer::difference_type diff = it - timestamps_.begin();
		return data_.begin() + diff;
	}

	TimestampIterator ToTimestampIterator(DataIterator const & it) const
	{
		typename DataBuffer::difference_type diff = it - data_.begin();
		return timestamps_.begin() + diff;
	}

private: // Data
	DataBuffer data_;
	TimestampBuffer timestamps_;

};

} // namespace cf
