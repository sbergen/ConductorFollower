#pragma once

#include <boost/pointer_cast.hpp>

#include "JuceHeader.h"

#include "ScoreFollower/ScoreEvent.h"

namespace sf = cf::ScoreFollower;

class MidiEventBase : public sf::ScoreEvent
{
public:
	MidiEventBase(juce::MidiMessage const & message,
		sf::score_time_t const & noteLength = 0.0 * cf::score::seconds,
		sf::score_time_t const & timeToNext = std::numeric_limits<double>::max() * cf::score::seconds);

public: // ScoreEvent implementation
	double GetVelocity() { return msg_.getFloatVelocity(); }
	sf::score_time_t GetNoteLength() { return noteLength_; }
	sf::score_time_t GetTimeToNextNote() { return timeToNext_; }
	bool IsNoteOn() { return msg_.isNoteOn(); }

	void ApplyVelocity(double velocity);
	void SetChannel(int channel);

	// MakeKeyswitch needs to be implemented in deriving classes

public: // Additional functionality
	juce::MidiMessage const & Message() const { return msg_; }

protected:
	juce::MidiMessage msg_;

private:
	sf::score_time_t noteLength_;
	sf::score_time_t timeToNext_;
};

// Keyswitch event, may not produce more keyswitches (will throw)
class KeyswitchEvent : public MidiEventBase
{
public:
	KeyswitchEvent(int channel);
	void SetNote(int note);

public: // ScoreEvent implementation
	sf::ScoreEventPtr MakeKeyswitch(int note);
};

// "regular" midi event, which holds an allocated keyswitch event for note ons
class MidiEvent : public MidiEventBase
{
public:
	MidiEvent(juce::MidiMessage const & message, sf::score_time_t const & noteLength,
		sf::score_time_t const & timeToNext = std::numeric_limits<double>::max() * cf::score::seconds);

public: // ScoreEvent implementation
	void SetChannel(int channel);
	sf::ScoreEventPtr MakeKeyswitch(int note);

private:
	boost::shared_ptr<KeyswitchEvent> keyswitchEvent_;
};

typedef boost::shared_ptr<MidiEventBase> MidiEventPtr;

inline MidiEventPtr midi_event_cast(sf::ScoreEventPtr ev)
{
	// Yes, we are using static_cast for downcasting on purpose here
	return boost::static_pointer_cast<MidiEventBase>(ev);
}

