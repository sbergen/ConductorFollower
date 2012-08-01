#pragma once

#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include <boost/bind.hpp>

#include "cf/LockfreeRefCount.h"

namespace cf {

namespace {

template<typename T>
class RCUBase : public boost::noncopyable
{
public:
	typedef T value_type;
	typedef boost::shared_ptr<T> ptr_type;
	typedef boost::shared_ptr<const T> const_ptr_type;

protected:
	RCUBase(ptr_type data)
		: read_data_(data)
		, write_data_(boost::make_shared<T>(*data))
	{ assert(data); }

	const_ptr_type read() const { return read_data_; }
	const_ptr_type read_copy() const { return boost::make_shared<T>(*read_data_); }

	ptr_type write() { return write_data_; }
	ptr_type write_copy() const { return boost::make_shared<T>(*write_data_); }

public: // These need to be public to work with bind...
	void update()
	{
		read_data_.swap(write_data_);
		*write_data_ = *read_data_;
	}

	void updateFromCopy(ptr_type write_copy)
	{
		read_data_.swap(write_copy);
		*write_data_ = *read_data_;
	}

private:
	ptr_type read_data_;
	ptr_type write_data_;
};

} // anon namespace


// RAII write handle
template<typename Parent>
class RCUWriterHandle : public LockfreeRefCounted<Parent>
{
public:
	typedef typename Parent::ptr_type ptr_type;
	typedef typename Parent::value_type value_type;
	typedef typename LockfreeRefCounted::CleanupFunction CleanupFunction;

	RCUWriterHandle(Parent & parent, CleanupFunction cleanup_function, ptr_type data)
		: LockfreeRefCounted(parent, cleanup_function)
		, data_(data)
	{}

	value_type * operator->() const { return data_.get(); }
	value_type & operator* () const { return *data_; }

private:
	ptr_type data_;
};

// RCU providing lock-free read
template<typename T>
class RTReadRCU : public RCUBase<T>, public LockfreeRefCountProvider
{
public:
	typedef RCUWriterHandle<RTReadRCU> WriterHandle;

	RTReadRCU(ptr_type data)
		: RCUBase(data)
	{}

	using RCUBase::read;
	WriterHandle writer()
	{
		ptr_type copy = write_copy();
		return WriterHandle(
			*this,
			boost::bind(&RCUBase::updateFromCopy, this, copy),
			copy);
	}
};

// RCU providing lock-free write
template<typename T>
class RTWriteRCU : public RCUBase<T>, public LockfreeRefCountProvider
{
public:
	typedef RCUWriterHandle<RTWriteRCU> WriterHandle;

	RTWriteRCU(ptr_type data)
		: RCUBase(data)
	{}

	const_ptr_type read() { return read_copy(); }
	WriterHandle writer()
	{
		return WriterHandle(
			*this,
			boost::bind(&RCUBase::update, this),
			write());
	}
};

} // namespace cf