#include "cf/ButlerThread.h"

namespace cf {

ButlerThread::ButlerThread(milliseconds_t runInterval)
	: runInterval_(runInterval)
{
	thread_.reset(new boost::thread(boost::bind(&ButlerThread::Loop, this)));
}

ButlerThread::~ButlerThread()
{
	thread_->interrupt();
	thread_->join();
}

void
ButlerThread::AddCallback(Callback const & callback)
{
	callbacks_.push_back(callback);
}

void
ButlerThread::Loop()
{
	while (true) {
		boost::system_time wakeUpTime = boost::get_system_time();
		wakeUpTime += boost::posix_time::milliseconds(runInterval_.count());

		std::list<Callback> callbacks;

		// Don't block during execution, just copy
		{
			unique_lock lock(callbackMutex_);
			callbacks = callbacks_;
		}

		for (auto it = callbacks.begin(); it != callbacks.end(); ++it) {
			(*it)();
		}

		try {
			boost::thread::sleep(wakeUpTime);
		} catch (boost::thread_interrupted) {
			return;
		}
	}
}

} // namespace cf
