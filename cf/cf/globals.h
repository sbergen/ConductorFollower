#pragma once

#include <boost/atomic.hpp>

#include "cf/Logger.h"

#ifdef NDEBUG
	#define LOG(...)
#else
	#define LOG(fmt, ...) Globals::Logger().Log((LogItem(fmt), __VA_ARGS__))
#endif

namespace cf {

class Globals
{
private:
	friend class GlobalsRef;

	static void Ref();
	static void Unref();

public:
	static FileLogger & Logger();

private:
	static boost::atomic<int> refCount_;
	static FileLogger * logger_;
};

class GlobalsRef
{
public:
	GlobalsRef() { Globals::Ref(); }
	~GlobalsRef() { Globals::Unref(); }
};

} // namespace cf
