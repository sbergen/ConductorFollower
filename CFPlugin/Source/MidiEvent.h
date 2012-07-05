#pragma once

#include <boost/pointer_cast.hpp>

#include "JuceHeader.h"

#include "ScoreFollower/ScoreEvent.h"

namespace sf = cf::ScoreFollower;

class MidiEvent : public sf::ScoreEvent
{
public:
	MidiEvent(juce::MidiMessageSequence::MidiEventHolder * event);

public: // ScoreEvent implementation
	double GetVelocity();
	void ApplyVelocity(double velocity);

public: // Additional functionality
	juce::MidiMessage const & Message() const { return msg_; }

private:
	juce::MidiMessage msg_;
};

typedef boost::shared_ptr<MidiEvent> MidiEventPtr;

inline MidiEventPtr midi_event_cast(sf::ScoreEventPtr ev)
{
	// Yes, we are using static_cast for downcasting on purpose here
	return boost::static_pointer_cast<MidiEvent>(ev);
}

