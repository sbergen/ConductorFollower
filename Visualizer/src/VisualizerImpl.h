#pragma once

#include <boost/circular_buffer.hpp>

#include "Visualizer/Visualizer.h"
#include "Visualizer/Data.h"
#include "Visualizer/Position.h"

namespace cf {
namespace Visualizer {

class VisualizerImpl : public Visualizer
{
private:
	struct PositionData
	{
		PositionData() : beat(false) {}

		PositionData(timestamp_t const & time, Position const & pos)
			: pos(pos), beat(false), timestamp(time)
		{}

		operator bool() const { return pos; }

		Position pos;
		bool beat;
		timestamp_t timestamp;
	};

public:
	VisualizerImpl();

public: // Visualizer implementation
	void SetSize(int width, int height);
	void UpdateData(Data const & data);
	void NewHandPosition(timestamp_t const & time, Position const & pos);
	void NewBeat(timestamp_t const & time);

public: // Component overrides
	void paint(Graphics & g);

private:
	juce::Colour ColorFromDepth(Data::depth_type depth);

private:
	int width_;
	int height_;

	Data::depth_type maxDepth_;

	juce::Image depthImage_;

	typedef boost::circular_buffer<PositionData> HandBuffer;
	HandBuffer handBuffer_;
};

} // namespace Visualizer
} // namespace cf
