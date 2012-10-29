#pragma once

#include <boost/atomic.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>

#include <boost/lockfree/ringbuffer.hpp>

#include "cf/Logger.h"
#include "cf/ButlerThread.h"

#define ALWAYS_LOG

#if !defined(ALWAYS_LOG) && defined(NDEBUG)
	#define LOG(...)
#else
	#define LOG(fmt, ...) cf::GlobalsRef().Logger()->Log((cf::LogItem(fmt), __VA_ARGS__))
#endif

namespace cf {

class Globals
{
public:
	typedef boost::lockfree::ringbuffer<std::string, 0> ErrorBuffer;

protected:
	static void Ref();
	static void Unref();

	static boost::shared_ptr<ButlerThread> butler_;
	static boost::shared_ptr<FileLogger> logger_;
	static boost::shared_ptr<ErrorBuffer> errorBuffer_;

private:
	static boost::atomic<int> refCount_;
};

class GlobalsRef : private Globals
{
	// We are using inheritance only for access control, so make sure Globals is purely static
	BOOST_STATIC_ASSERT(boost::is_empty<Globals>::value);

public:
	GlobalsRef() { Globals::Ref(); }
	~GlobalsRef() { Globals::Unref(); }

public:
	boost::shared_ptr<ButlerThread> Butler() const { return Globals::butler_; }
	boost::shared_ptr<FileLogger> Logger() const { return Globals::logger_; }
	boost::shared_ptr<ErrorBuffer> ErrorBuffer() const { return Globals::errorBuffer_; }

};

} // namespace cf
