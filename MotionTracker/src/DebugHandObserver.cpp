#include "DebugHandObserver.h"

#include <iostream>

#include <boost/format.hpp>
#include <boost/units/io.hpp>

namespace cf {
namespace MotionTracker {

DebugHandObserver::DebugHandObserver(std::string const & name)
	: name_(name)
{
}

void
DebugHandObserver::HandFound()
{
	std::cout << name_ << " Found!" << std::endl;
}

void
DebugHandObserver::HandLost()
{
	std::cout << name_ << " Lost!" << std::endl;
}

void
DebugHandObserver::NewHandPosition(float time, Point3D const & pos)
{
	std::cout
		<< boost::format("%1% at %2%, %3%, %4%")
		% name_
		% pos.get_x()
		% pos.get_y()
		% pos.get_z()
		<< std::endl;
}

} // namespace MotionTracker
} // namespace cf
