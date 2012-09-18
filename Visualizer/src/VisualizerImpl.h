#pragma once

#include "Visualizer/Visualizer.h"

namespace cf {
namespace Visualizer {

class VisualizerImpl : public Visualizer
{
public: // Visualizer implementation
	void UpdateData(VisualizationData & data);

private:
};

} // namespace Visualizer
} // namespace cf
