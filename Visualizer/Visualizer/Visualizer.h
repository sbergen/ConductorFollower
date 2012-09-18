#pragma once

#include <boost/shared_ptr.hpp>

#include "JuceHeader.h"

namespace cf {
namespace Visualizer {

class VisualizationData;

class Visualizer : public juce::Component
{
public:
	boost::shared_ptr<Visualizer> Create();

public:
	virtual void UpdateData(VisualizationData & data) = 0;
};

} // namespace Visualizer
} // namespace cf
