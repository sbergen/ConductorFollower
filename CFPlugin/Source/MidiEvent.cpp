#include "MidiEvent.h"

#include <boost/make_shared.hpp>

// MidiEventBase

MidiEventBase::MidiEventBase(juce::MidiMessage const & message, sf::score_time_t const & noteLength, sf::score_time_t const & timeToNext)
	: msg_(message)
	, noteLength_(noteLength)
	, timeToNext_(timeToNext)
{	
}

void
MidiEventBase::ApplyVelocity(double velocity)
{
	if (!msg_.isNoteOn()) { return; }
	msg_.setVelocity(static_cast<float>(velocity));
}

void
MidiEventBase::SetChannel(int channel)
{
	msg_.setChannel(channel);
}

// KeyswitchEvent

KeyswitchEvent::KeyswitchEvent(int channel)
	: MidiEventBase(juce::MidiMessage::noteOn(channel, 0, 1.0f))
{}

void
KeyswitchEvent::SetNote(int note)
{
	msg_.setNoteNumber(note);
}

sf::ScoreEventPtr
KeyswitchEvent::MakeKeyswitch(int)
{
	throw std::runtime_error("Trying to create keyswitch for keyswitch!");
}

// MidiEvent

MidiEvent::MidiEvent(juce::MidiMessage const & message, sf::score_time_t const & noteLength,
	sf::score_time_t const & timeToNext)
	: MidiEventBase(message, noteLength, timeToNext)
{
	// Create keyswitch only for note ons
	if (message.isNoteOn()) {
		keyswitchEvent_ = boost::make_shared<KeyswitchEvent>(message.getChannel());
	}
}

void
MidiEvent::SetChannel(int channel)
{
	MidiEventBase::SetChannel(channel);
	if (keyswitchEvent_) {
		keyswitchEvent_->SetChannel(channel);
	}
}

sf::ScoreEventPtr
MidiEvent::MakeKeyswitch(int note)
{
	if (!keyswitchEvent_) {
		throw std::runtime_error("Trying to create keyswitch for non-note-on event!");
	}

	keyswitchEvent_->SetNote(note);
	return keyswitchEvent_;
}
