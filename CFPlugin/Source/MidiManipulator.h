#pragma once

#include "ScoreFollower/ScoreEventManipulator.h"

#include "common.h"

// The juce header has evil macros, so be sure to include it last...
#include "JuceHeader.h"

class MidiManipulator : public cf::ScoreFollower::ScoreEventManipulator
{
public:
	double GetVelocity(ScoreEventHandle const & handle)
	{
		return EventAdapter::Adapt(handle).getFloatVelocity();
	}

	void ApplyVelocity(ScoreEventHandle & handle, double velocity)
	{
		MidiMessage & data = EventAdapter::Adapt(handle);
		if (!data.isNoteOn()) { return; }
		data.setVelocity(velocity);
	}
};
