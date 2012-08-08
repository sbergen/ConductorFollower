#pragma once

#include <boost/atomic.hpp>

namespace cf {

class LockfreeRefCountProvider
{
protected:
	LockfreeRefCountProvider()
		: ref_count_(0)
	{}

private:
	template<typename T, typename A>
	friend class LockfreeRefCounted;

	boost::atomic<int> ref_count_;
};

template<typename CountProvider, typename CleanupArg>
class LockfreeRefCounted
{
public:
	typedef void (CountProvider::*CleanupFunction)(CleanupArg);

	LockfreeRefCounted(CountProvider & provider, CleanupFunction cleanup_function, CleanupArg cleanupArg)
		: provider_(provider)
		, cleanup_function_(cleanup_function)
		, cleanupArg_(cleanupArg)
	{
		provider_.ref_count_.fetch_add(1);
	}

	LockfreeRefCounted(LockfreeRefCounted const & other)
		: provider_(other.provider_)
		, cleanup_function_(other.cleanup_function_)
		, cleanupArg_(other.cleanupArg_)
	{
		provider_.ref_count_.fetch_add(1);
	}

	~LockfreeRefCounted()
	{
		if (provider_.ref_count_.fetch_sub(1) == 1)
		{
			(provider_.*cleanup_function_)(cleanupArg_);
		}
	}

private:
	CountProvider & provider_;
	CleanupFunction cleanup_function_;
	CleanupArg cleanupArg_;
};

} // namespace cf