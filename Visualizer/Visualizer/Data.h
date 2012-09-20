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

	struct Position
	{
		Position() : x(-1), y(-1) {}
		Position(int x, int y) : x(x), y(y) {}

		operator bool() const { return x > -1 && y > -1; }

		int x;
		int y;
	};

public:
	Data();
	void Update(int width, int height,
		int maxDepth, frame_id_type frameId,
		depth_type const * data);
	void Reserve(int width, int height);
	void SetHandPosition(Position pos) { handPosition_ = pos; }

	depth_type const & operator()(int x, int y) const;

	int width() const { return width_; }
	int height() const { return height_; }
	depth_type const & maxDepth() const { return maxDepth_; }
	frame_id_type const & frameId() const { return frameId_; }
	Position const & HandPosition() const { return handPosition_; }

	// hmm, why not just have them public?
	bool beatOccurred;

private:
	int width_;
	int height_;
	depth_type maxDepth_;
	frame_id_type frameId_;
	std::vector<depth_type> depthData_;
	Position handPosition_;
};

typedef boost::shared_ptr<Data> DataPtr;

typedef ChenBuffer<Data, 1, false> DataBuffer;
typedef boost::shared_ptr<DataBuffer> DataBufferPtr;

} // namespace Visualizer
} // namespace cf
