#pragma once

#include <boost/shared_ptr.hpp>

#include "cf/time.h"

#include "JuceHeader.h"

namespace cf {
namespace Visualizer {

class Data;
class Position;

class Visualizer : public juce::Component
{
public:
	typedef boost::shared_ptr<Visualizer> Ptr;
	static Ptr Create();

public:
	virtual void SetSize(int width, int height) = 0;
	virtual void UpdateData(Data const & data) = 0;
	virtual void NewHandPosition(timestamp_t const & time, Position const & pos) = 0;
	virtual void NewBarPhase(timestamp_t const & time, double phase) = 0;
	virtual void NewBeat(timestamp_t const & time) = 0;
	virtual void NewBeatPhaseInfo(timestamp_t const & time, double phase, double offset) = 0;
};

} // namespace Visualizer
} // namespace cf
