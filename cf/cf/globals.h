#pragma once

#include <boost/atomic.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>

#include "cf/Logger.h"
#include "cf/ButlerThread.h"

#ifdef NDEBUG
	#define LOG(...)
#else
	#define LOG(fmt, ...) GlobalsRef().Logger()->Log((LogItem(fmt), __VA_ARGS__))
#endif

namespace cf {

class Globals
{
protected:
	static void Ref();
	static void Unref();

	static boost::shared_ptr<ButlerThread> butler_;
	static boost::shared_ptr<FileLogger> logger_;

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
	boost::shared_ptr<ButlerThread> Butler() { return Globals::butler_; }
	boost::shared_ptr<FileLogger> Logger() { return Globals::logger_; }

};

} // namespace cf
