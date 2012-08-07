#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
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
	typedef T * raw_ptr_type;
	typedef boost::shared_ptr<T> smart_ptr_type;

protected:
	RCUBase(ButlerPtr butler, value_type const & data)
		: butler_(butler)
	{
		read_data_ = new T(data);
		write_data_ = new T(data);
	}

	~RCUBase()
	{
		delete read_data_;
		delete write_data_;
	}

protected:
	inline smart_ptr_type managed_copy(raw_ptr_type ptr) { return boost::make_shared<T>(*ptr); }
	inline raw_ptr_type unmanaged_copy(raw_ptr_type ptr) { return new T(*ptr); }

protected:
	ButlerPtr butler_;
	raw_ptr_type read_data_;
	raw_ptr_type write_data_;
};

} // anon namespace


// RAII write handle
template<typename Parent>
class RCUWriterHandle : public LockfreeRefCounted<Parent, typename Parent::raw_ptr_type>
{
public:
	typedef typename Parent::raw_ptr_type ptr_type;
	typedef typename Parent::value_type value_type;
	typedef typename LockfreeRefCounted::CleanupFunction CleanupFunction;

	RCUWriterHandle(Parent & parent, CleanupFunction cleanup_function, ptr_type data)
		: LockfreeRefCounted(parent, cleanup_function, data)
		, data_(data)
	{}

	value_type * operator->() { return data_; }
	value_type & operator* () { return *data_; }

	value_type const * operator->() const { return data_; }
	value_type const & operator* () const { return *data_; }

private:
	ptr_type data_;
};

/* RCU providing lock-free read
    - Only one writer thread allowed
    - pointer returned by read() not guaranteed to be valid
	  any longer than the butler interval
*/
template<typename T>
class RTReadRCU : public RCUBase<T>
{
public:
	typedef RCUWriterHandle<RTReadRCU> WriterHandle;
	typedef T const * read_ptr_type;

	RTReadRCU(ButlerPtr butler, value_type const & data)
		: RCUBase(butler, data)
	{}

	read_ptr_type read()
	{
		return read_data_;
	}

	WriterHandle writer()
	{
		return WriterHandle(*this, &RTReadRCU::updateFromCopy, unmanaged_copy(write_data_));
	}

public: // Needs to be public for binding, would otherwise be private...
	void updateFromCopy(raw_ptr_type write_data)
	{
		butler_->ScheduleDelete(read_data_);
		read_data_ = write_data;
		*write_data_ = *read_data_;
	}
};

/* RCU providing lock-free write
    - Only one writer thread allowed
*/
template<typename T>
class RTWriteRCU : public RCUBase<T>
{
public:
	typedef RCUWriterHandle<RTWriteRCU> WriterHandle;
	typedef boost::shared_ptr<T const> read_ptr_type;

	RTWriteRCU(ButlerPtr butler, value_type const & data)
		: RCUBase(butler, data)
	{}

	read_ptr_type read()
	{
		return managed_copy(read_data_);
	}

	WriterHandle writer()
	{
		return WriterHandle(*this, &RTWriteRCU::update, write_data_);
	}

public: // Needs to be public for binding, would otherwise be private...
	void update(raw_ptr_type write_data)
	{
		assert(write_data == write_data_);
		std::swap(read_data_, write_data_);
		*write_data_ = *read_data_;
	}
};

} // namespace cf