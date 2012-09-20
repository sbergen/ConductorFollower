#include "Visualizer/Data.h"

#include <cassert>

namespace cf {
namespace Visualizer {

Data::Data()
	: width_(0)
	, height_(0)
	, maxDepth_(1)
	, frameId_(0)
	, beatOccurred(false)
{
}

void
Data::Update(int width, int height,
	int maxDepth, frame_id_type frameId,
	depth_type const * data)
{
	width_ = width;
	height_ = height;
	maxDepth_ = maxDepth;
	frameId_ = frameId;
	depthData_.assign(data, data + (width * height));
}

void
Data::Reserve(int width, int height)
{
	depthData_.reserve(width * height);
}

Data::depth_type const &
Data::operator()(int x, int y) const
{
	assert(x < width_ && y < height_);
	return depthData_[y * width_ + x];
}

} // namespace Visualizer
} // namespace cf