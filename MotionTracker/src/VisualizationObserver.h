#pragma once

#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>

#include "Visualizer/Data.h"
#include "Visualizer/Position.h"

namespace cf {
namespace MotionTracker {

class VisualizationObserver : public boost::noncopyable
{
public:
	virtual ~VisualizationObserver() {}

	virtual void InitVisualizationData(int width, int height) = 0;

	// Get data to write to
	virtual Visualizer::DataPtr GetVisualizationData() = 0;

	// Notifies that the data retuned by GetVisualizationData() contains new data
	virtual void NewVisualizationData() = 0;

	// Provides Visualization position for the hand
	virtual void NewVisualizationHandPosition(Visualizer::Position const & pos) = 0;
};

} // namespace MotionTracker
} // namespace cf
