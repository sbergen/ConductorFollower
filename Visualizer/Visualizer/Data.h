#pragma once

#include <vector>
#include <cstdint>

#include <boost/shared_ptr.hpp>

#include "cf/ChenBuffer.h"

namespace cf {
namespace Visualizer {

class Data
{
public:
	typedef unsigned short depth_type;
	typedef uint32_t frame_id_type;

public:
	Data();
	void Update(int width, int height,
		int maxDepth, frame_id_type frameId,
		depth_type const * data);
	void Reserve(int width, int height);

	depth_type const & operator()(int x, int y) const;

	int width() const { return width_; }
	int height() const { return height_; }
	depth_type const & maxDepth() const { return maxDepth_; }
	frame_id_type const & frameId() const { return frameId_; }

private:
	int width_;
	int height_;
	depth_type maxDepth_;
	frame_id_type frameId_;
	std::vector<depth_type> depthData_;
};

typedef boost::shared_ptr<Data> DataPtr;

typedef ChenBuffer<Data, 1, false> DataBuffer;
typedef boost::shared_ptr<DataBuffer> DataBufferPtr;

} // namespace Visualizer
} // namespace cf
