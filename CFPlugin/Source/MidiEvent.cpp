#include "MidiEvent.h"

#include <boost/make_shared.hpp>

MidiEvent::MidiEvent(juce::MidiMessage const & message, sf::score_time_t const & noteLength)
	: msg_(message)
	, noteLength_(noteLength)
{	
}

void
MidiEvent::ApplyVelocity(double velocity)
{
	if (!msg_.isNoteOn()) { return; }
	msg_.setVelocity(static_cast<float>(velocity));
}

sf::ScoreEventPtr
MidiEvent::MakeKeyswitch(int note)
{
	auto msg = MidiMessage::noteOn(msg_.getChannel(), note, 1.0f);
	return boost::make_shared<MidiEvent>(msg, 0.0 * cf::score::seconds);
}
