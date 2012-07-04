#pragma once

#include "ScoreFollower/ScoreEventAdapter.h"

// forward declare the juce stuff, because of the retaeded macros in the headers :(
namespace juce {
	class MidiMessage;
} // namespace juce

typedef cf::ScoreFollower::ScoreEventAdapter<juce::MidiMessage> EventAdapter;
