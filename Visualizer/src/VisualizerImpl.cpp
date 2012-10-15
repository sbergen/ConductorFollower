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
	, barPhase_(-1.0)
	, drawNegativePhase_(true)
	, beatBuffer_(10)
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
VisualizerImpl::NewBarPhase(timestamp_t const & time, double phase)
{
	if (phase < barPhase_) {
		drawNegativePhase_ = !drawNegativePhase_;
	}

	barPhase_ = phase;
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
VisualizerImpl::NewBeatPhaseInfo(timestamp_t const & time, double phase, double offset)
{
	//LOG("Beat, phase: %1%, offset: %2%", phase, offset);
	beatBuffer_.push_back(BeatPhaseData(phase, offset));
}

void
VisualizerImpl::paint(Graphics & g)
{
#ifdef NDEBUG
	g.drawImageAt(depthImage_, 0, 0);
#else
	g.fillAll(juce::Colour(0, 0, 0));
#endif

	juce::Colour color(255, 0, 0);
	float alpha(1.0);
	g.setColour(color);

	PositionData prevPosition;
	for (auto it = handBuffer_.rbegin(); it != handBuffer_.rend(); ++it) {
		if (prevPosition) {
			alpha *= 0.9f;
			g.setColour(color.withAlpha(alpha));

			g.drawLine(juce::Line<float>(
				prevPosition.pos.x, prevPosition.pos.y,
				it->pos.x, it->pos.y), 6.0f);

		} else {
			// last known position
			PaintPoint(g, juce::Colour((juce::uint8)255, 0, 0), 
				it->pos.x, it->pos.y, 6.0f);
		}

		if (it->beat) {
			PaintPoint(g, juce::Colour((juce::uint8)0, 255, 0, alpha), 
				it->pos.x, it->pos.y);
		}

		if (*it) {
			prevPosition = *it;
		}
	}

	PaintBarPhase(g);
	PaintBeatPhases(g);
}

juce::Colour
VisualizerImpl::ColorFromDepth(Data::depth_type depth)
{
	if (depth > maxDepth_) { maxDepth_ = depth; }

	float brightness = static_cast<float>(maxDepth_ - depth) / maxDepth_;
	return juce::Colour(0.0f, 0.0f, brightness, 1.0f); 
}

void
VisualizerImpl::PaintPoint(Graphics & g, juce::Colour color, int x, int y, float width)
{
	g.setFillType(juce::FillType(color));
	g.fillEllipse(x - (width / 2), y - (width / 2), width, width);
}

void
VisualizerImpl::PaintBarPhase(Graphics & g)
{
	if (barPhase_ < 0) { return; }

	float size = 0.9 * height_;
	float xOffset = (width_ - size) / 2;
	float yOffset = (height_ - size) / 2;
	float width = 3;

	float twoPi = 2 * 3.141f;
	float rads = barPhase_ * twoPi;

	juce::Path path;
	if (drawNegativePhase_) {
		path.addArc(xOffset, yOffset, size, size, rads, twoPi, true);
	} else {
		path.addArc(xOffset, yOffset, size, size, 0.0f, rads, true);
	}

	g.setColour(juce::Colour((juce::uint8)0, 190, 190, 0.6f));
	g.strokePath(path, juce::PathStrokeType(width));
}

void
VisualizerImpl::PaintBeatPhases(Graphics & g)
{
	float size = 0.9 * height_ + 6;
	float xOffset = (width_ - size) / 2;
	float yOffset = (height_ - size) / 2;
	float width = 3;

	float twoPi = 2 * 3.141f;
	float alpha = 1.0;
	
	for (auto it = beatBuffer_.rbegin(); it != beatBuffer_.rend(); ++it) {
	
		float start = (it->phase - it->offset) * twoPi;
		float end = it->phase * twoPi;

		juce::Colour aheadColor((juce::uint8)0, 255, 0, alpha);
		juce::Colour behindColor((juce::uint8)255, 0, 0, alpha);
		
		juce::Path path;
		path.addArc(xOffset, yOffset, size, size, start, end, true);

		g.setColour(it->offset > 0.0 ? aheadColor : behindColor);
		g.strokePath(path, juce::PathStrokeType(width));

		alpha *= 0.6;
	}
}

} // namespace Visualizer
} // namespace cf


