#include "cf/ButlerThread.h"

#include <boost/make_shared.hpp>

namespace cf {

class ButlerThread::CallbackRef
{
public:
	~CallbackRef() { parent_.RemoveCallback(callbackIt_); }
	CallbackRef(ButlerThread & parent, ButlerThread::CallbackIterator const & callback)
		: parent_(parent)
		, callbackIt_(callback)
	{}

private:
	ButlerThread & parent_;
	ButlerThread::CallbackIterator const callbackIt_;
};

ButlerThread::CallbackHandle::CallbackHandle(ButlerThread & parent, CallbackIterator const & callback)
	: ref_(boost::make_shared<CallbackRef>(parent, callback))
{}

ButlerThread::ButlerThread(milliseconds_t runInterval)
	: runInterval_(runInterval)
{
	thread_= boost::make_shared<boost::thread>(boost::bind(&ButlerThread::Loop, this));
}

ButlerThread::~ButlerThread()
{
	thread_->interrupt();
	thread_->join();
}

ButlerThread::CallbackHandle
ButlerThread::AddCallback(Callback const & callback)
{
	unique_lock lock(callbackMutex_);
	auto it = callbacks_.insert(callbacks_.end(), callback);
	return CallbackHandle(*this, it);
}

void
ButlerThread::RemoveCallback(CallbackIterator const & callback)
{
	unique_lock lock(callbackMutex_);
	callbacks_.erase(callback);
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
