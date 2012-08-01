#pragma once

#include <list>

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/utility.hpp>
#include <boost/lockfree/ringbuffer.hpp>

#include "cf/time.h"

namespace cf {

class ButlerDeletable;

class ButlerThread : public boost::noncopyable
{
public:
	typedef boost::function<void()> Callback;

private:
	class CallbackRef;

	typedef std::list<Callback> CallbackList;
	typedef CallbackList::const_iterator CallbackIterator;
	typedef boost::unique_lock<boost::mutex> unique_lock;

public:
	class CallbackHandle
	{
	public:
		CallbackHandle() {}
		void Cancel() { ref_.reset(); }

	private:
		friend class ButlerThread;
		CallbackHandle(ButlerThread & parent, CallbackIterator const & callback);
		boost::shared_ptr<CallbackRef> ref_;
	};

public:
	ButlerThread(milliseconds_t runInterval);
	~ButlerThread();

	CallbackHandle AddCallback(Callback const & callback);
	
	void ScheduleDelete(ButlerDeletable * ptr)
	{
		bool success = deleteList_.enqueue(ptr);
		assert(success);
	}

private:
	void RemoveCallback(CallbackIterator const & callback);
	void Loop();
	void RunDeleteQueue();
	void RunCallbacks();

private:
	typedef boost::lockfree::ringbuffer<ButlerDeletable *, 0> DeleteList;

	milliseconds_t runInterval_;
	boost::mutex callbackMutex_;
	CallbackList callbacks_;
	boost::shared_ptr<boost::thread> thread_;
	DeleteList deleteList_;
};

} // namespace cf