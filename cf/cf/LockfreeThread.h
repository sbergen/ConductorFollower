#pragma once

#include "cf/ButlerThread.h"

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/atomic.hpp>
#include <boost/thread.hpp>
#include <boost/utility.hpp>

namespace cf {


// Class for starting and stopping a thread in a lock free way
// The thread will be started and stopped from the butler thread.

// hint: use boost::bind and boost::make_shared for RunnableFactory

// The type runnable should initialize stuff in it's constructor.
// After this operator() will be called until the thead is stopped,
// or it returns false.
// After this the destructor is called.
// Runnable needs to be default-constructible

template<typename Runnable>
class LockfreeThread : public boost::noncopyable
{
public:
	typedef boost::function<boost::shared_ptr<Runnable>()> RunnableFactory;

	LockfreeThread(RunnableFactory factory, ButlerThread & butler)
		: runnableFactory_(factory)
		, shouldRun_(false)
		
	{
		butlerCallbackHandle_ = butler.AddCallback(boost::bind(
			&LockfreeThread<Runnable>::CheckState, this));
	}

	~LockfreeThread()
	{
		if (thread_) {
			thread_->interrupt();
			thread_->join();
		}
	}

	void RequestStart() { shouldRun_.store(true); }
	void RequestStop() { shouldRun_.store(false); }
	bool IsRunning() { return thread_ && thread_->joinable(); }

private:
	
	struct Runner
	{
		Runner(RunnableFactory & runnableFactory)
			: runnableFactory_(runnableFactory)
		{}

		void operator() ()
		{
			auto r = runnableFactory_();
			while ((*r)()) {
				boost::this_thread::interruption_point();
			}
		}

		RunnableFactory runnableFactory_;
	};

private:
	// Called from butler
	void CheckState()
	{
		if (shouldRun_.load())
		{
			if (!IsRunning()) {
				thread_ = boost::make_shared<boost::thread>(Runner(runnableFactory_));
			}
		} else {
			if (IsRunning()) {
				thread_->interrupt();
				thread_->join();
			}
		}
	}

	RunnableFactory runnableFactory_;
	boost::atomic<bool> shouldRun_;
	boost::shared_ptr<boost::thread>  thread_;
	ButlerThread::CallbackHandle butlerCallbackHandle_;
};

} // namespace cf