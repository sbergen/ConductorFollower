#pragma once

#include <string>

#include "HandObserver.h"

namespace cf {
namespace MotionTracker {

class DebugHandObserver : public HandObserver
{
public:
	DebugHandObserver(std::string const & name);

	void HandFound();
	void HandLost();
	void NewHandPosition(float time, Point3D const & pos);

private:
	std::string name_;
};

} // namespace MotionTacker
} // namespace cf
