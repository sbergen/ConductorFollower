#pragma once

#include "cf/cf.h"

namespace cf {
namespace MotionTracker {

class HandObserver
{
public:
	virtual ~HandObserver() {}

	virtual void HandFound() = 0;
	virtual void HandLost() = 0;
	virtual void NewHandPosition(float time, Point3D const & pos) = 0;
};

} // namespace cf
} // namespace MotionTracker
