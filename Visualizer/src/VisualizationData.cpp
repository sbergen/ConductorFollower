#include "Visualizer/VisualizationData.h"

namespace cf {
namespace Visualizer {

VisualizationData::VisualizationData()
	: width_(0)
	, height_(0)
	, maxDepth_(1)
{
}

void
VisualizationData::Update(int width, int height, int maxDepth, depth_type const * data)
{
	width_ = width;
	height_ = height;
	maxDepth_ = maxDepth;
	depthData_.assign(data, data + (width * height));
}

VisualizationData::depth_type const &
VisualizationData::operator()(int x, int y) const
{
	assert(x < width_ && y < height_);
	return depthData_[y * width_ + x];
}

} // namespace Visualizer
} // namespace cf