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

class DebugAllocationState
{
private:
	DebugAllocationState()
		: allowed(true)
	{}

public:
	static DebugAllocationState * Get()
	{
		// This is thread safe in c++11,
		// not sure about current compiler implementations...
		static boost::recursive_mutex mutex;
		static boost::atomic<bool> initializing(false);
		static boost::thread_specific_ptr<DebugAllocationState> ptr;

		if (!ptr.get()) {
			// Initializing will be a blocking operation.
			boost::recursive_mutex::scoped_lock lock(mutex);

			bool expected = false;
			if (!initializing.compare_exchange_strong(expected, true)) {
				return nullptr;
			}
			ptr.reset(new DebugAllocationState());
			initializing.store(false);
		}
		
		return ptr.get();
	}

	bool allowed;
};

inline bool new_allowed()
{
	DebugAllocationState * state = DebugAllocationState::Get();
	if (!state) { return true; }
	return state->allowed;
}

inline void disallow_new()
{
	DebugAllocationState * state = DebugAllocationState::Get();
	assert(state != nullptr);
	state->allowed = false;
}

inline void allow_new()
{
	DebugAllocationState * state = DebugAllocationState::Get();
	assert(state != nullptr);
	state->allowed = true;
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
