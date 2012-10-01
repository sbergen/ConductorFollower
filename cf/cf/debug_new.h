#pragma once

#include <cstdlib>
#include <new>

#include <boost/thread.hpp>

#define CF_MS_MEMORY_DEBUG 0
#define CF_VLD 1

#if CF_VLD && CF_MS_MEMORY_DEBUG
#include <vld.h>
#endif

// Header only hack (static variable init)
inline bool disallow_new_headeronly(boost::thread::id const & thread_id, bool check)
{
	static boost::thread::id disallowed_thread;
	if (check) {
		return (disallowed_thread == thread_id);
	} else {
		disallowed_thread = thread_id;
		return false;
	}
}

namespace cf {

inline bool new_disallowed()
{
	return disallow_new_headeronly(boost::this_thread::get_id(), true);
}

inline void disallow_new()
{
	disallow_new_headeronly(boost::this_thread::get_id(), false);
}

inline void allow_new()
{
	disallow_new_headeronly(boost::thread::id(), false);
}

} // namespace cf

inline void* operator new( size_t size )
{
	assert(!cf::new_disallowed());
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
	assert(!cf::new_disallowed());
#if CF_MS_MEMORY_DEBUG
	_free_dbg(ptr, _NORMAL_BLOCK);
#else
	free(ptr);
#endif
}
