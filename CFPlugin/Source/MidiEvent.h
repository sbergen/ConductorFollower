#pragma once

#include <boost/pointer_cast.hpp>

#include "JuceHeader.h"

#include "ScoreFollower/ScoreEvent.h"

namespace sf = cf::ScoreFollower;

class MidiEvent : public sf::ScoreEvent
{
public:
	MidiEvent(juce::MidiMessage const & message, sf::score_time_t const & noteLength);

public: // ScoreEvent implementation
	double GetVelocity() { return msg_.getFloatVelocity(); }
	sf::score_time_t GetNoteLength() { return noteLength_; }
	bool IsNoteOn() { return msg_.isNoteOn(); }

	void ApplyVelocity(double velocity);

	sf::ScoreEventPtr MakeKeyswitch(int note);

public: // Additional functionality
	juce::MidiMessage const & Message() const { return msg_; }

private:
	juce::MidiMessage msg_;
	sf::score_time_t noteLength_;
};

typedef boost::shared_ptr<MidiEvent> MidiEventPtr;

inline MidiEventPtr midi_event_cast(sf::ScoreEventPtr ev)
{
	// Yes, we are using static_cast for downcasting on purpose here
	return boost::static_pointer_cast<MidiEvent>(ev);
}

