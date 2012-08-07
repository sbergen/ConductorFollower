#pragma once

#include <list>
#include <algorithm>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/utility.hpp>
#include <boost/bind.hpp>
#include <boost/thread/mutex.hpp>

#include "cf/LockfreeRefCount.h"
#include "cf/ButlerDeleter.h"

namespace cf {

namespace {

template<typename T>
class RCUBase : public boost::noncopyable, public LockfreeRefCountProvider
{
public:
	typedef T value_type;
	typedef boost::shared_ptr<T> ptr_type;
	typedef boost::shared_ptr<T const> const_ptr_type;

protected:
	inline ptr_type copy(ptr_type ptr) { return boost::make_shared<T>(*ptr); }
};

} // anon namespace


// RAII write handle
template<typename Parent>
class RCUWriterHandle : public LockfreeRefCounted<Parent, typename Parent::ptr_type>
{
public:
	typedef typename Parent::ptr_type ptr_type;
	typedef typename Parent::value_type value_type;
	typedef typename LockfreeRefCounted::CleanupFunction CleanupFunction;

	RCUWriterHandle(Parent & parent, CleanupFunction cleanup_function, ptr_type data)
		: LockfreeRefCounted(parent, cleanup_function, data)
		, data_(data)
	{}

	value_type * operator->() { return data_.get(); }
	value_type & operator* () { return *data_; }

	value_type const * operator->() const { return data_.get(); }
	value_type const & operator* () const { return *data_; }

private:
	ptr_type data_;
};

/* RCU providing lock-free read
   Updates happen in whatever order they do,
   multithreaded access to writers is a bit sketchy...
*/
template<typename T, typename MutexType = boost::mutex>
class RTReadRCU : public RCUBase<T>
{
public:
	typedef RCUWriterHandle<RTReadRCU> WriterHandle;

	RTReadRCU(value_type const & data)
		: currentValue_(new value_type(data))
	{}

	const_ptr_type read()
	{
		return currentValue_;
	}

	WriterHandle writer()
	{
		boost::unique_lock<MutexType> lock(oldValuesMutex_);
		CleanOld();
		return WriterHandle(*this, &RTReadRCU::Update, copy(currentValue_));
	}

public: // Needs to be public for binding, would otherwise be private...
	void Update(ptr_type write_data)
	{
		boost::unique_lock<MutexType> lock(oldValuesMutex_);
		PushOld(); // Ensure the old value is not deleted in an RT context
		currentValue_.swap(write_data);
	}

private:
	void CleanOld()
	{
		auto newEnd = std::remove_if(std::begin(oldValues_), std::end(oldValues_),
			[](ptr_type const & ptr) { return ptr.unique(); });
		oldValues_.erase(newEnd, std::end(oldValues_));
	}

	void PushOld()
	{
		oldValues_.push_back(currentValue_);
	}

private:
	ptr_type currentValue_;
	
	MutexType oldValuesMutex_;
	std::list<ptr_type> oldValues_;

};

/* RCU providing lock-free write
   There must be only one writer thread!
*/
template<typename T>
class RTWriteRCU : public RCUBase<T>
{
public:
	typedef RCUWriterHandle<RTWriteRCU> WriterHandle;

	RTWriteRCU(value_type const & data)
		: readData_(new value_type(data))
		, writeData_(new value_type(data))
	{}

	const_ptr_type read()
	{
		return copy(readData_);
	}

	WriterHandle writer()
	{
		return WriterHandle(*this, &RTWriteRCU::update, writeData_);
	}

public: // Needs to be public for binding, would otherwise be private...
	void update(ptr_type writeData)
	{
		assert(writeData == writeData_);
		readData_.swap(writeData_);
		*writeData_ = *readData_;
	}

private:
	ptr_type readData_;
	ptr_type writeData_;
};

} // namespace cf