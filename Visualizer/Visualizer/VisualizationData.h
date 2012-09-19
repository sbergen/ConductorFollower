#pragma once

#include <vector>

#include "cf/ChenBuffer.h"

namespace cf {
namespace Visualizer {

class VisualizationData
{
public:
	typedef unsigned short depth_type;

public:
	VisualizationData();
	void Update(int width, int height, int maxDepth, depth_type const * data);
	void Reserve(int width, int height);
	
	depth_type const & operator()(int x, int y) const;

	int width() const { return width_; }
	int height() const { return height_; }
	depth_type const & maxDepth() const { return maxDepth_; }

private:
	int width_;
	int height_;
	depth_type maxDepth_;
	std::vector<depth_type> depthData_;
};

typedef ChenBuffer<VisualizationData, 1, false> VisualizationDataBuffer;

} // namespace Visualizer
} // namespace cf
