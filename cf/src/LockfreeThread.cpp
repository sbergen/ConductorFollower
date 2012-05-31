#include "cf/LockfreeThread.h"

namespace cf {

LockfreeThread::LockfreeThread(boost::function<bool()> initFunction, boost::function<bool()> loopFunction)
	: state_(NotRunning)
	, init_(initFunction)
	, loop_(loopFunction)
{
	// Keep lock until ctor exits, just to be sure...
	boost::lock_guard<boost::mutex> lock(waitMutex_);
	thread_.reset(new boost::thread(boost::bind(&LockfreeThread::Run, this)));
}

LockfreeThread::~LockfreeThread()
{
	state_.store(StopRequested);
	thread_->interrupt();
	thread_->join();
}

bool
LockfreeThread::RequestStart()
{
	State expected = NotRunning;
	if (!state_.compare_exchange_strong(expected, RunRequested)) {
		// We are not in a NotRunning state, check all other states
		switch(expected)
		{
		case Running:
		case RunRequested:
			return false;
		case StopRequested:
			// Try to cancel the stop request before it is read
			if (state_.compare_exchange_strong(expected, Running))
			{
				// Cancelling the request succeeded!
				assert(expected == StopRequested);
				return true;
			}
			
			// Otherwise the stop request just got executed...
			assert(expected == NotRunning);
			state_.store(RunRequested);
			break;
		default:
			// Should never be reached!
			assert(false);
			break;
		}
	}

	waitCond_.notify_one();
	return true;
}

bool
LockfreeThread::RequestStop()
{
	State expected = Running;
	return state_.compare_exchange_strong(expected, StopRequested);
}

void
LockfreeThread::Run()
{
	// The mutex is locked always but when waiting
	unique_lock lock(waitMutex_);

	while (true)
	{
		if (!WaitForRunOrInterrupt(lock)) { return; }

		if (!init_()) {
			state_.store(NotRunning);
			continue;
		}

		while (true) {
			State expected = StopRequested;
			if (state_.compare_exchange_strong(expected, NotRunning)) {
				continue;
			}

			if (!loop_()) {
				state_.store(NotRunning);
				continue;
			}
		}
	}
}

bool
LockfreeThread::WaitForRunOrInterrupt(unique_lock & lock)
{
	try {
		State expected = RunRequested;
		while (!state_.compare_exchange_strong(expected, Running)) {
			waitCond_.wait(lock);
		}
		return true;
	} catch(boost::thread_interrupted) {
		return false;
	}
}

} // namespace cf