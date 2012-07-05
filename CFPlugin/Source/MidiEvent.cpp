#include "MidiEvent.h"

MidiEvent::MidiEvent(juce::MidiMessageSequence::MidiEventHolder * event)
	: msg_(event->message)
{
}

double
MidiEvent::GetVelocity()
{
	return msg_.getVelocity();
}

void
MidiEvent::ApplyVelocity(double velocity)
{
	if (!msg_.isNoteOn()) { return; }
	msg_.setVelocity(velocity);
}
