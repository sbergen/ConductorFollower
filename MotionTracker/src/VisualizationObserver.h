#pragma once

#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>

#include "Visualizer/Data.h"

namespace cf {
namespace MotionTracker {

class VisualizationObserver : public boost::noncopyable
{
public:
	virtual ~VisualizationObserver() {}

	virtual void NewVisualizationData(Visualizer::DataPtr data) = 0;
};

} // namespace MotionTracker
} // namespace cf
