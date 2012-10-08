#pragma once

#include <list>

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/utility.hpp>
#include <boost/lockfree/fifo.hpp>

#include "cf/time.h"

namespace cf {

class ButlerDeletable;

class ButlerThread : public boost::noncopyable
{
public:
	typedef boost::function<void()> Callback;

private:
	class CallbackRef;

	// Needs to be list so that iterators are not invalidated on insertions
	typedef std::list<Callback> CallbackList;
	typedef CallbackList::const_iterator CallbackIterator;

	// Needs to be recursive, because callbacks can add and remove callbacks.
	// We are ok with insertions, but removals will need a different mechanism.
	// since it's a list!
	typedef boost::recursive_mutex list_mutex;
	typedef boost::unique_lock<list_mutex> unique_lock;

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
		bool success = deleteQueue_.enqueue(MakeDeleteEntry(ptr));
		assert(success);
	}

private:
	typedef unsigned counter_t;
	typedef boost::atomic<counter_t> atomic_counter_t;

	// Has to be POD to work with lockfree fifo
	struct DeleteEntry
	{
		operator bool () const { return ptr != nullptr; }
		bool Exprired(counter_t counterNow) const;
		void Destroy();

		ButlerDeletable * ptr;
		counter_t counter;
	};
	DeleteEntry MakeDeleteEntry(ButlerDeletable * ptr);

	typedef boost::lockfree::fifo<DeleteEntry> DeleteQueue;

private:
	void RemoveCallback(CallbackIterator const & callback);
	void Loop();
	void RunDeleteQueue(counter_t currentRound);
	void RunCallbacks(counter_t currentRound);

private:

	milliseconds_t runInterval_;
	
	list_mutex callbackMutex_;
	CallbackList callbacks_;
	CallbackIterator callbackIt_;
	bool callbackItIncremented_;

	boost::shared_ptr<boost::thread> thread_;
	
	DeleteQueue deleteQueue_;
	DeleteEntry pendingDelete_;

	atomic_counter_t loopCounter_;
};

} // namespace cf