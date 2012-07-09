#pragma once

#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

namespace cf {

namespace {

template<typename T>
class RCUBase : public boost::noncopyable
{
public:
	typedef boost::shared_ptr<T> ptr_type;
	typedef boost::shared_ptr<const T> const_ptr_type;

protected:
	RCUBase(ptr_type data) : data_(data) { assert(data); }

	ptr_type ref() { return data_; }
	const_ptr_type const_ref() const { return data_; }

	ptr_type copy() { return boost::make_shared<T>(*data_); }
	const_ptr_type const_copy() const { return boost::make_shared<T const>(*data_); }

	void set_data(ptr_type data) { data_ = data; }

private:
	ptr_type data_;
};

} // anon namespace


// RCU providing lock-free read
template<typename T>
class RTReadRCU : public RCUBase<T>
{
private:
	// RAII writer for RCUs
	class ScopedRCUWriter
	{
	public:
		ScopedRCUWriter(RTReadRCU & parent, ptr_type data)
			: parent_(parent), data_(data) { }

		~ScopedRCUWriter() { parent_.update(data_); }

	private:
		friend class WriterHandle;
		RTReadRCU & parent_;
		ptr_type data_;
	};

	// Make access possible for nested type
	void update(ptr_type data) { set_data(data); }

public:
	// Provide a direct operator-> and prevent unnecessary calls to
	// ~ScopedRCUWriter() with this handle class, which can be freely copied.
	class WriterHandle
	{
	public:
		WriterHandle(RTReadRCU & parent, ptr_type data)
			: writer_(boost::make_shared<ScopedRCUWriter>(parent, data)) {}

		T * operator->() const { return writer_->data_.get(); }

	private:
		boost::shared_ptr<ScopedRCUWriter> writer_;
	};

public:

	RTReadRCU(ptr_type data) : RCUBase(data) {}

	const_ptr_type read() { return const_ref(); }
	WriterHandle   writer() { return WriterHandle(*this, copy()); }
};

// RCU providing lock-free write
template<typename T>
class RTWriteRCU : public RCUBase<T>
{
public:
	RTWriteRCU(ptr_type data) : RCUBase(data) {}

	ptr_type       write() { return ref(); }
	const_ptr_type read_copy() { return const_copy(); }
};

} // namespace cf