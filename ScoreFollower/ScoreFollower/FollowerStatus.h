#pragma once

#include "cf/ChangeTracked.h"

namespace cf {
namespace ScoreFollower {

class FollowerStatus
{
public:
	FollowerStatus()
		: running(false, someChanged_)
		, speed(0.0, someChanged_)
		, someChanged_(false) // Nothing wrong with the init order :)
	{}

	// Checks whether or not something has changed
	bool SomethingChanged(bool reset = true)
	{
		bool ret = someChanged_;
		if (reset) { someChanged_ = false; }
		return ret;
	}

	ChangeTracked<bool> running;
	ChangeTracked<double> speed;

private:
	bool someChanged_;
};

} // namespace ScoreFollower
} // namespace cf
