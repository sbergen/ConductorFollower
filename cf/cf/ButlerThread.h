#pragma once

#include <list>

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/utility.hpp>

#include "cf/time.h"

namespace cf {

class ButlerThread : public boost::noncopyable
{
public:
	typedef boost::function<void()> Callback;

private:
	class CallbackRef;

	typedef std::list<Callback> CallbackList;
	typedef boost::unique_lock<boost::mutex> unique_lock;

public:
	class CallbackHandle
	{
	public:
		CallbackHandle() {}
		void Cancel() { ref_.reset(); }

	private:
		friend class ButlerThread;
		CallbackHandle(ButlerThread & parent, CallbackList::iterator & callback);
		boost::shared_ptr<CallbackRef> ref_;
	};

public:
	ButlerThread(milliseconds_t runInterval);
	~ButlerThread();

	CallbackHandle AddCallback(Callback const & callback);

private:
	void RemoveCallback(CallbackList::iterator & callback);
	void Loop();

private:
	milliseconds_t runInterval_;
	boost::mutex callbackMutex_;
	CallbackList callbacks_;
	boost::shared_ptr<boost::thread> thread_;
};

} // namespace cf