#pragma once

#include "ScoreFollower/MidiManipulator.h"

// The juce header has evil macros, so be sure to include it last...
#include "JuceHeader.h"

class MidiManipulator : public cf::ScoreFollower::MidiManipulator<MidiMessage>
{
public:
	void ApplyVelocity(MidiMessage & data, double velocity)
	{
		data.setVelocity(data.getFloatVelocity() + (velocity - 0.5));
		//data.multiplyVelocity(velocity);
	}
};
