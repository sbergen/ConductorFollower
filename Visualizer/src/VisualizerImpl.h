#pragma once

#include "Visualizer/Visualizer.h"

namespace cf {
namespace Visualizer {

class VisualizerImpl : public Visualizer
{
public: // Visualizer implementation
	void UpdateData(Data const & data);

private:
};

} // namespace Visualizer
} // namespace cf
