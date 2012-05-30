#include "cf/LockfreeThread.h"

namespace cf {

LockfreeThread::LockfreeThread()
	: state_(NotRunning)
{}

LockfreeThread::~LockfreeThread()
{
	if (!thread_) { return; }
	if (thread_->joinable()) {
		RequestStop();
		thread_->join();
	}
}

bool
LockfreeThread::RequestStart(boost::function<bool()> initFunction, boost::function<bool()> loopFunction)
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

	init_ = initFunction;
	loop_ = loopFunction;
	thread_.reset(new boost::thread(boost::bind(&LockfreeThread::Run, this)));
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
	if (!init_()) {
		state_.store(NotRunning);
		return;
	}

	while (true) {
		State expected = StopRequested;
		if (state_.compare_exchange_strong(expected, NotRunning)) {
			return;
		}

		if (!loop_()) {
			state_.store(NotRunning);
			return;
		}
	}
}

} // namespace cf