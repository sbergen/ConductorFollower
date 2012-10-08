#include "cf/ButlerThread.h"

#include <boost/make_shared.hpp>

#include "cf/ButlerDeletable.h"

namespace cf {

// CallbackRef

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

// CallbackHandle

ButlerThread::CallbackHandle::CallbackHandle(ButlerThread & parent, CallbackIterator const & callback)
	: ref_(boost::make_shared<CallbackRef>(parent, callback))
{}

// DeleteEntry

bool
ButlerThread::DeleteEntry::Exprired(counter_t counterNow) const
{
	// Check only these two to make it overflow-safe
	return (counter != counterNow) &&
			(counter != counterNow - 1);
}

void
ButlerThread::DeleteEntry::Destroy()
{
	delete ptr;
	ptr = nullptr;
}

ButlerThread::DeleteEntry
ButlerThread::MakeDeleteEntry(ButlerDeletable * ptr)
{
	DeleteEntry entry;
	entry.ptr = ptr;
	entry.counter = loopCounter_.load();
	return entry;
}

// ButlerThread

ButlerThread::ButlerThread(milliseconds_t runInterval)
	: runInterval_(runInterval)
	, deleteQueue_(512) // reasonable size
	, loopCounter_(0)
{
	pendingDelete_ = MakeDeleteEntry(nullptr);
	thread_= boost::make_shared<boost::thread>(boost::bind(&ButlerThread::Loop, this));
}

ButlerThread::~ButlerThread()
{
	thread_->interrupt();
	thread_->join();
	RunDeleteQueue(loopCounter_.load() + 2);
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

	if (callback == callbackIt_) {
		callbackIt_ = callbacks_.erase(callback);
		callbackItIncremented_ = true;
	} else {
		callbacks_.erase(callback);
	}
}

void
ButlerThread::Loop()
{
	while (true) {
		boost::system_time wakeUpTime = boost::get_system_time();
		wakeUpTime += boost::posix_time::milliseconds(runInterval_.count());

		// This is the only place this is incremented, it's only read in other locations...
		counter_t currentRound = loopCounter_.fetch_add(1) + 1;
		RunDeleteQueue(currentRound);
		RunCallbacks(currentRound);

		try {
			boost::thread::sleep(wakeUpTime);
		} catch (boost::thread_interrupted) {
			return;
		}
	}
}

void
ButlerThread::RunDeleteQueue(counter_t currentRound)
{
	do
	{
		if (pendingDelete_ && !pendingDelete_.Exprired(currentRound)) { return; }
		pendingDelete_.Destroy();
	} while (deleteQueue_.dequeue(pendingDelete_));
}

void
ButlerThread::RunCallbacks(counter_t currentRound)
{
	// Must block during execution to make this safe
	// (think about destruction time...)
	unique_lock lock(callbackMutex_);

	callbackIt_ = callbacks_.begin();
	while (callbackIt_ != callbacks_.end()) {
		callbackItIncremented_ = false;
		(*callbackIt_)();
		if (!callbackItIncremented_) { ++callbackIt_; }
	}
}

} // namespace cf
