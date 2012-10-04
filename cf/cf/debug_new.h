#pragma once

#include <cstdlib>
#include <new>
#include <map>

#include <boost/thread.hpp>

#include <boost/atomic.hpp>

#define CF_MS_MEMORY_DEBUG 0
#define CF_VLD 1

#if CF_VLD && CF_MS_MEMORY_DEBUG
#include <vld.h>
#endif

namespace cf {
namespace detail {

struct DebugAllocationState
{
	bool allowed;
};

inline DebugAllocationState & GetDebugAllocationState()
{
	// boosts thread local storage can't be used within
	// operator new and delete properly :(

	// Special case for VS2010 (which I'm working on)
	// otherwise use the C++11 version
	// Feel free to implement other special cases,
	// if c++11 support is lacking :)
#if defined(_MSC_VER) && (_MSC_VER <= 1600)
	__declspec( thread ) static DebugAllocationState state = { true };
#else
	thread_local static DebugAllocationState state = { true };
#endif

	return state;
}

inline bool new_allowed()
{
	return GetDebugAllocationState().allowed;
}

inline void disallow_new()
{
	GetDebugAllocationState().allowed = false;
}

inline void allow_new()
{
	GetDebugAllocationState().allowed = true;
}

} // namespace detail
} // namespace cf

inline void* operator new( size_t size )
{
	assert(cf::detail::new_allowed());
#if CF_MS_MEMORY_DEBUG
	void * ret = _malloc_dbg(size, _NORMAL_BLOCK, __FILE__, __LINE__);
#else
	void * ret = malloc(size);
#endif
	if (!ret) { throw std::bad_alloc(); }
	return ret;
}

inline void operator delete( void* ptr )
{
	assert(cf::detail::new_allowed());
#if CF_MS_MEMORY_DEBUG
	_free_dbg(ptr, _NORMAL_BLOCK);
#else
	free(ptr);
#endif
}
