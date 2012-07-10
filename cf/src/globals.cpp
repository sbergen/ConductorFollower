#include "globals.h"

namespace cf {
namespace ScoreFollower {

boost::atomic<int> Globals::refCount_(0);
FileLogger * Globals::logger_(nullptr);

void Globals::Ref()
{
	if (refCount_.fetch_add(1) > 0) { return; }

	logger_ = new FileLogger("ScoreFollower.log");
	LOG("Initialized!");
}

void Globals::Unref()
{
	if (refCount_.fetch_sub(1) > 1) { return; }

	LOG("Deinitializing!");
	delete logger_;
	logger_ = nullptr;
}

FileLogger & Globals::Logger()
{
	assert(logger_ != nullptr);
	return *logger_;
}

} // namespace ScoreFollower
} // namespace cf
