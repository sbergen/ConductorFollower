#include "globals.h"

namespace cf {
namespace ScoreFollower {

bool Globals::initialized_ = false;
FileLogger * Globals::logger_ = 0;

void Globals::Initialize()
{
	assert(!initialized_);
	logger_ = new FileLogger("ScoreFollower.log");
	initialized_ = true;

	LOG("Initialized!");
}

void Globals::CleanUp()
{
	assert(initialized_);
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
