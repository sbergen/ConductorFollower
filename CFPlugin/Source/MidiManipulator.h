#pragma once

#include "ScoreFollower/ScoreEventManipulator.h"

// The juce header has evil macros, so be sure to include it last...
#include "JuceHeader.h"

class MidiManipulator : public cf::ScoreFollower::ScoreEventManipulator
{
public:
	double GetVelocity(ScoreEventHandle const & handle)
	{
		return handle.data<MidiMessage>().getFloatVelocity();
	}

	void ApplyVelocity(ScoreEventHandle & handle, double velocity)
	{
		MidiMessage & data = handle.data<MidiMessage>();
		if (!data.isNoteOn()) { return; }
		data.setVelocity(velocity);
	}
};
