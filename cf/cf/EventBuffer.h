#pragma once

#include <functional>
#include <algorithm>
#include <numeric>
#include <exception>

#include <boost/circular_buffer.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/iterator_range.hpp>

namespace cf {

template<typename TData, typename TTimestamp,
	template<typename, typename> class TContainer = boost::circular_buffer, template<typename> class TAlloc = std::allocator>
class EventBuffer
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
	public:
		struct DataPair
		{
			DataPair(TTimestamp const & timestamp, TData const & data)
				: timestamp(timestamp), data(data) {}

			TTimestamp const & timestamp;
			TData const & data;
		};

	public:
		Range(EventBuffer const & parent, TimestampIterator const & begin, TimestampIterator const & end)
			: timestampRange_(begin, end)
			, dataRange_(parent.ToDataIterator(begin), parent.ToDataIterator(end))
		{ Rewind(); }

		// Checks

		typename DataBuffer::size_type Size() { return  dataRange_.size(); }
		
		bool Empty() { return dataRange_.empty(); }

		TTimestamp LastTimestamp() const
		{
			if (timestampRange_.empty()) { return TTimestamp(); }
			return timestampRange_.back();
		}

		// Data access
		DataPair operator[](size_t i)
		{
			return DataPair(timestampRange_[i], dataRange_[i]);
		}

		// Traversal

		// Call func for each (timestamp, data) pair
		template<typename Func>
		void ForEach(Func & func) const
		{
			ForEachWhile([&func](TTimestamp const & ts, TData const & data) ->
				bool { func(ts, data); return true; });
		}

		// Call func for each (timestamp, data) pair, until it returns a value that converts to false
		template<typename Func>
		void ForEachWhile(Func & func) const
		{
			auto tIt = timestampRange_.begin();
			auto dIt = dataRange_.begin();
			for(/* inited above */; tIt != timestampRange_.end(); ++tIt, ++dIt) {
				if (!func(*tIt, *dIt)) { return; }
			}
		}

		// Iterating interface
		void Rewind() { tIt_ = timestampRange_.begin(); dIt_ = dataRange_.begin(); }
		bool Next() { ++tIt_; ++dIt_;  return !AtEnd(); }
		bool AtEnd() const { return dIt_ == dataRange_.end(); }

		TTimestamp const & timestamp() const { return *tIt_; }
		TData const & data() const { return *dIt_; }

		// direct range access
		TimestampRange const & timestampRange() const { return timestampRange_; }
		DataRange const & dataRange() const { return dataRange_; }

		// Data container conversion utilities
		template<typename T>
		T DataAs() const { return T(dataRange_.begin(), dataRange_.end()); }
		
		template<template<typename> class T>
		T<DataIterator> DataAs() const { return T<DataIterator>(dataRange_.begin(), dataRange_.end()); }

	private:
		TimestampRange timestampRange_;
		DataRange dataRange_;

		TimestampIterator tIt_;
		DataIterator dIt_;
	};

public: // Main interface

	EventBuffer(size_t size)
		: data_(size)
		, timestamps_(size)
	{
	}

	EventBuffer & operator=(Range const & range)
	{
		Clear();
		DataRange data = range.dataRange();
		TimestampRange timestamps = range.timestampRange();
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
		// Sanity check
		if (timestamps_.empty()) { return timestamps_.end(); }

		TimestampIterator it = std::lower_bound(timestamps_.begin(), timestamps_.end(), time);
		
		if (it == timestamps_.end()) { return --it; } // All items are smaller, return last

		// *it >= time at this stage
		if (*it == time) { return it; } // Equal
		if (it == timestamps_.begin()) { return timestamps_.end(); } // Greater, but the first one
		return --it;
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

private: // Data
	DataBuffer data_;
	TimestampBuffer timestamps_;

};

} // namespace cf
