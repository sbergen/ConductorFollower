#include "globals.h"

namespace cf {
namespace ScoreFollower {

bool Globals::initialized_ = false;
FileLogger * Globals::logger_ = 0;

void Globals::Initialize()
{
	if (initialized_) { return; }
	logger_ = new FileLogger("ScoreFollower.log");
	initialized_ = true;

	LOG("Initialized!");
}

void Globals::CleanUp()
{
	if (!initialized_) { return; }
	initialized_ = false;
	delete logger_;
}

FileLogger & Globals::Logger()
{
	assert(initialized_);
	return *logger_;
}

} // namespace ScoreFollower
} // namespace cf
