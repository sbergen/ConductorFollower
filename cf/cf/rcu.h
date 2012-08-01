#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include <boost/bind.hpp>

#include "cf/LockfreeRefCount.h"
#include "cf/ButlerDeleter.h"

namespace cf {

namespace {

template<typename T>
class RCUBase : public boost::noncopyable, public LockfreeRefCountProvider
{
public:
	typedef boost::shared_ptr<ButlerThread> ButlerPtr;

	typedef T value_type;
	typedef boost::shared_ptr<T> ptr_type;
	typedef boost::shared_ptr<const T> const_ptr_type;

protected:
	RCUBase(ButlerPtr butler, value_type const & data)
		: butler_(butler)
	{
		read_data_ = butler_deletable_copy(data, *butler_);
		write_data_ = butler_deletable_copy(data, *butler_);
	}

	const_ptr_type read() const { return read_data_; }
	const_ptr_type read_copy() const { return butler_deletable_copy(*read_data_, *butler_); }

	ptr_type write() { return write_data_; }
	ptr_type write_copy() const { return butler_deletable_copy(*write_data_, *butler_); }

public: // These need to be public to work
	void update(ptr_type write_data)
	{
		assert(write_data == write_data_);
		read_data_.swap(write_data_);
		*write_data_ = *read_data_;
	}

	void updateFromCopy(ptr_type write_data)
	{
		read_data_.swap(write_data);
		*write_data_ = *read_data_;
	}

private:
	ButlerPtr butler_;
	ptr_type read_data_;
	ptr_type write_data_;
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

	value_type * operator->() const { return data_.get(); }
	value_type & operator* () const { return *data_; }

private:
	ptr_type data_;
};

// RCU providing lock-free read
template<typename T>
class RTReadRCU : public RCUBase<T>
{
public:
	typedef RCUWriterHandle<RCUBase> WriterHandle;

	RTReadRCU(ButlerPtr butler, value_type const & data)
		: RCUBase(butler, data)
	{}

	using RCUBase::read;
	WriterHandle writer()
	{
		ptr_type copy = write_copy();
		return WriterHandle(*this, &RCUBase::updateFromCopy, copy);
	}
};

// RCU providing lock-free write
template<typename T>
class RTWriteRCU : public RCUBase<T>
{
public:
	typedef RCUWriterHandle<RCUBase> WriterHandle;

	RTWriteRCU(ButlerPtr butler, value_type const & data)
		: RCUBase(butler, data)
	{}

	const_ptr_type read() { return read_copy(); }
	WriterHandle writer()
	{
		return WriterHandle(*this, &RCUBase::update, write());
	}
};

} // namespace cf