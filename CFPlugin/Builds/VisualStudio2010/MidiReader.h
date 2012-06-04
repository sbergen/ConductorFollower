#pragma once

#include <string>

#include "ScoreFollower/ScoreReader.h"
#include "ScoreFollower/TrackReader.h"

// The juce header has evil macros, so be sure to include it last...
#include "JuceHeader.h"

class MidiReader : public cf::ScoreFollower::ScoreReader<MidiMessage>
{
public:
	MidiReader(String const & filename);

public: // ScoreReader<MidiMessage> implementation
	int TrackCount() const { return file_.getNumTracks(); }
	cf::ScoreFollower::TrackReader<MidiMessage> * Track(int index);
	cf::ScoreFollower::TrackReader<cf::ScoreFollower::tempo_t> * TempoTrack();

private:

	// Tempo reader implementation
	class TempoReaderImpl : public cf::ScoreFollower::TrackReader<cf::ScoreFollower::tempo_t>
	{
	public:
		TempoReaderImpl(MidiFile const & file);
		bool NextEvent(cf::ScoreFollower::score_time_t & timestamp, cf::ScoreFollower::tempo_t & data);

	protected:
		MidiMessageSequence sequence_;
		short timeFormat_;
		int count_;
		int current_;
	};

	// Track reader implementation
	class TrackReaderImpl : public cf::ScoreFollower::TrackReader<MidiMessage>
	{
	public:
		TrackReaderImpl(MidiMessageSequence const & sequence);
		bool NextEvent(cf::ScoreFollower::score_time_t & timestamp, MidiMessage & data);
	
	protected:
		MidiMessageSequence const & sequence_;
		int count_;
		int current_;
	};

private:
	MidiFile file_;
};