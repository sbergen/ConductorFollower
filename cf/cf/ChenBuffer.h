#pragma once

#include <array>

#include <boost/atomic.hpp>
#include <boost/utility.hpp>

namespace cf {

/*
A lock free single writer multiple reader construct from
"A fully asynchronous reader/writer mechanism for multiprocessor real-time systems."
by Jing Chen and Alan Burns (1997)
*/

template<typename T, int Readers>
class ChenBuffer
{
private:
	typedef int reader_id;

public:

	// Use one instance per thread,
	// keep alive as long as it's going to be used.
	// May be used in a RAII fashion also
	class Reader : boost::noncopyable
	{
	public:
		Reader(ChenBuffer & parent)
			: parent_(parent)
			, id_(parent.AcquireReaderId())
		{}

		Reader(Reader && other)
			: parent_(other.parent_)
			, id_(other.id_)
		{
			other.id_ = -1;
		}

		~Reader()
		{
			if (id_ > -1) {
				parent_.ReleaseReaderId(id_);
			}
		}

		T const * operator->() const { return &(**this); }
		T const & operator* () const { return parent_.Read(id_); }

	private:
		ChenBuffer & parent_;
		reader_id id_;
	};

	// Unsafe reader to be used only from the writer thread
	// Note that the writer object has to be destructed before reading,
	// otherwise old values will be read!
	// This should be used from writer thread when reading,
	// otherwise we'll run out of the reader count.
	class UnsafeReader : boost::noncopyable
	{
	public:
		UnsafeReader(ChenBuffer & parent)
			: parent_(parent)
		{}

		UnsafeReader(ChenBuffer && other)
			: parent_(other.parent_)
		{}

		T const * operator->() const { return &(**this); }
		T const & operator* () const { return parent_.buffer_[parent_.latest_.load()]; }

	private:
		ChenBuffer & parent_;
	};

	// Use in a RAII fashion: acquire -> write -> destroy
	class Writer : boost::noncopyable
	{
	public:
		Writer(ChenBuffer & parent)
			: parent_(parent)
			, writeIndex_(parent.GetBuffer())
		{}

		Writer(Writer && other)
			: parent_(other.parent_)
			, writeIndex_(other.writeIndex_)
		{
			other.writeIndex_ = -1;
		}

		~Writer()
		{
			if (writeIndex_ > -1) {
				parent_.CompleteWrite(writeIndex_);
			}
		}

		T * operator->() { return &(**this); }
		T & operator* () { return parent_.buffer_[writeIndex_]; }

	private:
		ChenBuffer & parent_;
		int writeIndex_;
	};

public:
	ChenBuffer()
		: latest_(0)
	{
		std::for_each(std::begin(reading_), std::end(reading_),
			[](boost::atomic<int> & val) { val.store(-1); } );

		std::for_each(std::begin(reader_id_reserve_), std::end(reader_id_reserve_),
			[](boost::atomic<bool> & val) { val.store(false); } );
	}

	template<typename F>
	void Init(F initFunc)
	{
		std::for_each(std::begin(buffer_), std::end(buffer_),
			[&initFunc](T & val) { initFunc(val); } );
	}

	Reader GetReader() { return Reader(*this); }
	UnsafeReader GetUnsafeReader() { return UnsafeReader(*this); }
	Writer GetWriter() { return Writer(*this); }

private:

	reader_id AcquireReaderId()
	{
		for (reader_id i = 0; i < Readers; ++i) {
			bool expected = false;
			if (reader_id_reserve_[i].compare_exchange_strong(expected, true)) {
				return i;
			}
		}

		throw std::runtime_error("Too many readers for Chen buffer!");
	}

	void ReleaseReaderId(reader_id id)
	{
		reader_id_reserve_[id].store(false);
	}

	T const & Read(reader_id readerId)
	{
		assert(readerId < Readers);
		reading_[readerId].store(-1);
		int expected = -1;
		reading_[readerId].compare_exchange_strong(expected, latest_.load());
		int readIndex = reading_[readerId].load();
		return buffer_[readIndex];
	}

	int GetBuffer()
	{
		std::array<bool, Readers + 2> inUse;
		inUse.assign(false);
		inUse[latest_.load()] = true;
		for (int i = 0; i < Readers; ++i) {
			int j = reading_[i].load();
			if (j >= 0) { inUse[j] = true; }
		}

		for (int i = 0; i < Readers + 2; ++i) {
			if (!inUse[i]) { return i; }
		}

		throw std::runtime_error("Chen's algorithm (or you) failed!");
	}

	void CompleteWrite(int writeIndex)
	{
		latest_.store(writeIndex);
		for (int i = 0; i < Readers; ++i) {
			int expected = -1;
			reading_[i].compare_exchange_strong(expected, writeIndex);
		}
	}

private:
	std::array<T, Readers + 2> buffer_;
	std::array<boost::atomic<int>, Readers> reading_;
	boost::atomic<int> latest_;

	// Reader id usage
	std::array<boost::atomic<bool>, Readers> reader_id_reserve_;
};

} // namespace cf
