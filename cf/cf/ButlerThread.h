#pragma once

#include <list>

#include <boost/function.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/utility.hpp>

#include "cf/cf.h"

namespace cf {

class ButlerThread : public boost::noncopyable
{
public:
	typedef boost::function<void()> Callback;

public:
	ButlerThread(milliseconds_t runInterval);
	~ButlerThread();

	void AddCallback(Callback const & callback);

private:
	void Loop();

private:
	typedef boost::unique_lock<boost::mutex> unique_lock;

	milliseconds_t runInterval_;
	boost::mutex callbackMutex_;
	std::list<Callback> callbacks_;
	boost::scoped_ptr<boost::thread> thread_;
};

} // namespace cf