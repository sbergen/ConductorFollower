#pragma once

#include <boost/circular_buffer.hpp>

#include "Visualizer/Visualizer.h"
#include "Visualizer/Data.h"

namespace cf {
namespace Visualizer {

class VisualizerImpl : public Visualizer
{
public:
	VisualizerImpl();

public: // Visualizer implementation
	void SetSize(int width, int height);
	void UpdateData(Data const & data);

public: // Component overrides
	void paint(Graphics & g);

private:
	juce::Colour ColorFromDepth(Data::depth_type depth);

private:
	int width_;
	int height_;

	Data::depth_type maxDepth_;

	juce::Image depthImage_;

	typedef boost::circular_buffer<Data::Position> HandBuffer;
	HandBuffer handBuffer_;
};

} // namespace Visualizer
} // namespace cf
