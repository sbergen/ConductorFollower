#pragma once

#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>

#include "Visualizer/VisualizationData.h"

namespace cf {
namespace MotionTracker {

class VisualizationObserver : public boost::noncopyable
{
public:
	typedef boost::shared_ptr<Visualizer::VisualizationDataBuffer> VisualizationDataPtr;

	virtual ~VisualizationObserver() {}

	virtual void NewVisualizationData(VisualizationDataPtr data) = 0;
};

} // namespace MotionTracker
} // namespace cf
