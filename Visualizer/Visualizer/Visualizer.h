#pragma once

#include <boost/shared_ptr.hpp>

#include "JuceHeader.h"

namespace cf {
namespace Visualizer {

class Data;

class Visualizer : public juce::Component
{
public:
	typedef boost::shared_ptr<Visualizer> Ptr;
	static Ptr Create();

public:
	virtual void SetSize(int width, int height) = 0;
	virtual void UpdateData(Data const & data) = 0;
};

} // namespace Visualizer
} // namespace cf
