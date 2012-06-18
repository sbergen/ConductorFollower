#pragma once

#include "ScoreFollower/MidiManipulator.h"

// The juce header has evil macros, so be sure to include it last...
#include "JuceHeader.h"

class MidiManipulator : public cf::ScoreFollower::MidiManipulator<MidiMessage>
{
public:
	double GetVelocity(MidiMessage & data) { return data.getFloatVelocity(); }
	void ApplyVelocity(MidiMessage & data, double velocity) { data.setVelocity(velocity); }
};
