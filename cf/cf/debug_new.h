#pragma once

#include <cstdlib>
#include <new>

#include <boost/thread.hpp>

namespace {

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

} // anon namespace

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

inline void* operator new( size_t size ) throw()
{
	assert(!new_disallowed());
	void * ret = malloc(size);
	if (!ret) { throw std::bad_alloc(); }
	return ret;
}

inline void operator delete( void* ptr ) throw()
{
	//assert(!new_disallowed());
	free(ptr);
}