#pragma once

#include <boost/atomic.hpp>
#include <boost/function.hpp>

namespace cf {

class LockfreeRefCountProvider
{
protected:
	LockfreeRefCountProvider()
		: ref_count_(0)
	{}

private:
	// TODO make more specific once you get the syntax right...
	template<typename T>
	friend class LockfreeRefCounted;

	boost::atomic<int> ref_count_;
};

template<typename CountProvider>
class LockfreeRefCounted
{
public:
	typedef boost::function<void()> CleanupFunction;

	LockfreeRefCounted(CountProvider & provider, CleanupFunction const & cleanup_function)
		: provider_(provider)
		, cleanup_function_(cleanup_function)
	{
		provider_.ref_count_.fetch_add(1);
	}

	LockfreeRefCounted(LockfreeRefCounted const & other)
		: provider_(other.provider_)
		, cleanup_function_(other.cleanup_function_)
	{
		provider_.ref_count_.fetch_add(1);
	}

	~LockfreeRefCounted()
	{
		if (provider_.ref_count_.fetch_sub(1) == 1)
		{
			cleanup_function_();
		}
	}

private:
	CountProvider & provider_;
	CleanupFunction cleanup_function_;
};

} // namespace cf