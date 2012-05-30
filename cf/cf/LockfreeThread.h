#pragma once

#include <boost/scoped_ptr.hpp>
#include <boost/atomic.hpp>
#include <boost/thread.hpp>
#include <boost/function.hpp>

namespace cf {

class LockfreeThread
{
public:
	LockfreeThread();
	~LockfreeThread();

	bool RequestStart(boost::function<bool()> initFunction, boost::function<bool()> loopFunction);
	bool RequestStop();

	inline bool IsRunning() { return state_ == Running; }

private:
	void Run();

	enum State
	{
		NotRunning,
		Running,
		RunRequested,
		StopRequested
	};

	boost::atomic<State> state_;
	boost::scoped_ptr<boost::thread> thread_;

	boost::function<bool ()> init_;
	boost::function<bool ()> loop_;
};

} // namespace cf