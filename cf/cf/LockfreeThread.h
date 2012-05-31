#pragma once

#include <boost/scoped_ptr.hpp>
#include <boost/atomic.hpp>
#include <boost/thread.hpp>
#include <boost/function.hpp>

namespace cf {


// Class for starting and stopping a thread in a lock free way
// The threads lifetime matches the lifetime of the LockfreeThread,
// but the thread will be blocked unless requested to start.
//
// Once started, the thread executes an initialization function.
// If the init function returns true, it then enters a while loop
// which executes a loop function until it returns false
// or a stop is requested.
// The thread execution is also lock free, until stopped.

class LockfreeThread
{
public:
	LockfreeThread(boost::function<bool()> initFunction, boost::function<bool()> loopFunction);
	~LockfreeThread();

	bool RequestStart();
	bool RequestStop();

	inline bool IsRunning() { return state_ == Running; }

private:
	typedef boost::unique_lock<boost::mutex> unique_lock;

	// These functions are all called from thread_
	void Run();
	bool WaitForRunOrInterrupt(unique_lock & lock);

	enum State
	{
		NotRunning,
		Running,
		RunRequested,
		StopRequested,
	};

	boost::atomic<State> state_;

	// Initialized in ctor, not changed aftwewards
	boost::function<bool ()> const init_;
	boost::function<bool ()> const loop_;

	boost::mutex waitMutex_;
	boost::condition_variable waitCond_;
	boost::scoped_ptr<boost::thread>  thread_;
};

} // namespace cf