#include "cf/globals.h"

#include <boost/make_shared.hpp>

namespace cf {

boost::atomic<int> Globals::refCount_(0);
boost::shared_ptr<ButlerThread> Globals::butler_;
boost::shared_ptr<FileLogger> Globals::logger_;

void Globals::Ref()
{
	if (refCount_.fetch_add(1) > 0) { return; }

	butler_ = boost::make_shared<ButlerThread>(milliseconds_t(50));
	logger_ = boost::make_shared<FileLogger>("ScoreFollower.log", butler_);
	LOG("Initialized!");
}

void Globals::Unref()
{
	if (refCount_.fetch_sub(1) > 1) { return; }

	LOG("Deinitializing!");
	logger_.reset();
	butler_.reset();
}

} // namespace cf
