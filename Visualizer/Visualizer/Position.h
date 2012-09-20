#pragma once

namespace cf {
namespace Visualizer {

struct Position
{
	Position() : x(-1), y(-1) {}
	Position(int x, int y) : x(x), y(y) {}

	operator bool() const { return x > -1 && y > -1; }

	int x;
	int y;
};

} // namespace Visualizer
} // namespace cf
