#include "VisualizerImpl.h"

#include <boost/make_shared.hpp>

namespace cf {
namespace Visualizer {

boost::shared_ptr<Visualizer>
Visualizer::Create()
{
	return boost::make_shared<VisualizerImpl>();
}

void
VisualizerImpl::SetSize(int width, int height)
{
	width_ = width;
	height_ = height;
	depthImage_ = Image(juce::Image::RGB, width, height, true);
}

void
VisualizerImpl::UpdateData(Data const & data)
{
	auto maxDepth = data.maxDepth();
	for (int x = 0; x < data.width(); ++x) {
		for (int y= 0; y < data.height(); ++y) {
			auto color = ColorFromDepth(data(x, y), maxDepth);
			depthImage_.setPixelAt(x, y, color);
		}
	}
}

void
VisualizerImpl::paint(Graphics & g)
{
	g.drawImageAt(depthImage_, 0, 0);
}

juce::Colour
VisualizerImpl::ColorFromDepth(Data::depth_type depth, Data::depth_type maxDepth)
{
	float brightness = static_cast<float>(depth) / maxDepth;
	// Ugly hack to increase the dynamic range...
	brightness = 1.0f - 2 * brightness;

	return juce::Colour(0.0f, 0.0f, brightness, 1.0f); 
}

} // namespace Visualizer
} // namespace cf


