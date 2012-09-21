#include "VisualizerImpl.h"

#include <boost/make_shared.hpp>

#include "cf/globals.h"
#include "cf/time.h"

namespace cf {
namespace Visualizer {

boost::shared_ptr<Visualizer>
Visualizer::Create()
{
	return boost::make_shared<VisualizerImpl>();
}

VisualizerImpl::VisualizerImpl()
	: maxDepth_(0)
	, handBuffer_(50)
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
	for (int x = 0; x < data.width(); ++x) {
		for (int y= 0; y < data.height(); ++y) {
			auto color = ColorFromDepth(data(x, y));
			depthImage_.setPixelAt(x, y, color);
		}
	}
}

void
VisualizerImpl::NewHandPosition(timestamp_t const & time, Position const & pos)
{
	handBuffer_.push_back(PositionData(time, pos));
}

void
VisualizerImpl::NewBeat(timestamp_t const & time)
{
	// The time for visualization events differs from the motion detection timestamps,
	// so the last beat should always be ok, as the events arrive in time order.
	// Comparison here would be just extra work
	handBuffer_.back().beat = true;
}

void
VisualizerImpl::paint(Graphics & g)
{
	g.drawImageAt(depthImage_, 0, 0);

	juce::Colour color(255, 0, 0);
	float alpha(1.0);
	g.setColour(color);

	PositionData prevPosition;
	int i = 1;
	for (auto it = handBuffer_.rbegin(); it != handBuffer_.rend(); ++it) {
		if (prevPosition) {
			alpha *= 0.9f;
			g.setColour(color.withAlpha(alpha));

			g.drawLine(juce::Line<float>(
				prevPosition.pos.x, prevPosition.pos.y,
				it->pos.x, it->pos.y), 6.0f);

		} else {
			// last known position
			g.setFillType(juce::FillType(juce::Colour(255, 0, 0)));
			g.fillEllipse(it->pos.x, it->pos.y, 6.0f, 6.0f);
		}

		if (it->beat) {
			g.setFillType(juce::FillType(juce::Colour((juce::uint8)0, 255, 0, alpha)));
			g.fillEllipse(it->pos.x, it->pos.y, 10.0f, 10.0f);
			i = -3;
		}

		if (i == 0) {
			// Filter lag for beat
			g.setFillType(juce::FillType(juce::Colour((juce::uint8)0, 0, 255, alpha)));
			g.fillEllipse(it->pos.x, it->pos.y, 10.0f, 10.0f);
		}

		if (*it) {
			prevPosition = *it;
		}

		++i;
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


