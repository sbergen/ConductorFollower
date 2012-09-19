#include "VisualizerImpl.h"

#include <boost/make_shared.hpp>

namespace cf {
namespace Visualizer {

boost::shared_ptr<Visualizer>
Visualizer::Create()
{
	return boost::make_shared<VisualizerImpl>();
}

VisualizerImpl::VisualizerImpl()
	: maxDepth_(0)
	, handBuffer_(20)
{
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
	handBuffer_.push_back(data.HandPosition());

	for (int x = 0; x < data.width(); ++x) {
		for (int y= 0; y < data.height(); ++y) {
			auto color = ColorFromDepth(data(x, y));
			depthImage_.setPixelAt(x, y, color);
		}
	}
}

void
VisualizerImpl::paint(Graphics & g)
{
	g.drawImageAt(depthImage_, 0, 0);

	juce::Colour color(255, 0, 0);
	float alpha(1.0);
	g.setColour(color);

	Data::Position prevPosition;
	for (auto it = handBuffer_.rbegin(); it != handBuffer_.rend(); ++it) {
		if (prevPosition) {
			alpha *= 0.8;
			g.setColour(color.withAlpha(alpha));

			g.drawLine(juce::Line<float>(prevPosition.x, prevPosition.y, it->x, it->y), 6.0f);

		} else {
			// last known position
			g.setFillType(juce::FillType(juce::Colour(255, 0, 0)));
			g.fillEllipse(it->x, it->y, 6.0f, 6.0f);
		}

		if (*it) {
			prevPosition = *it;
		}
	}
}

juce::Colour
VisualizerImpl::ColorFromDepth(Data::depth_type depth)
{
	if (depth > maxDepth_) { maxDepth_ = depth; }

	float brightness = static_cast<float>(maxDepth_ - depth) / maxDepth_;
	return juce::Colour(0.0f, 0.0f, brightness, 1.0f); 
}

} // namespace Visualizer
} // namespace cf


