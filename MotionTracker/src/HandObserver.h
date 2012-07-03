#pragma once

#include <boost/utility.hpp>

#include "cf/cf.h"

namespace cf {
namespace MotionTracker {

class HandObserver : public boost::noncopyable
{
public:
	virtual ~HandObserver() {}

	virtual void HandFound() = 0;
	virtual void HandLost() = 0;
	virtual void NewHandPosition(float time, Point3D const & pos) = 0;
};

} // namespace MotionTracker
} // namespace cf
