#pragma once

#include <boost/ptr_container/ptr_vector.hpp>

#include "ScoreFollower/ScoreReader.h"
#include "ScoreFollower/TrackReader.h"
#include "ScoreFollower/ScoreEventHandle.h"

// The juce header has evil macros, so be sure to include it last...
#include "JuceHeader.h"

namespace sf = cf::ScoreFollower;

class MidiReader : public sf::ScoreReader
{
	typedef boost::ptr_vector<MidiMessage> EventContainer;

public:
	MidiReader(String const & filename);
	~MidiReader();

public: // ScoreReader implementation
	int TrackCount() const { return file_.getNumTracks(); }
	sf::TrackReaderPtr Track(int index);
	sf::TempoReaderPtr TempoTrack();

private:

	// Tempo reader implementation
	class TempoReaderImpl : public sf::TrackReader<sf::tempo_t>
	{
	public:
		TempoReaderImpl(MidiFile const & file);
		bool NextEvent(sf::score_time_t & timestamp, sf::tempo_t & data);

	protected:
		MidiMessageSequence sequence_;
		short timeFormat_;
		int count_;
		int current_;
	};

	// Track reader implementation
	class TrackReaderImpl : public cf::ScoreFollower::TrackReader<sf::ScoreEventHandle>
	{
	public:
		TrackReaderImpl(MidiMessageSequence const & sequence, EventContainer & events);
		bool NextEvent(sf::score_time_t & timestamp, sf::ScoreEventHandle & data);
	
	protected:
		EventContainer & events_;
		MidiMessageSequence const & sequence_;
		int count_;
		int current_;
	};

private:

	MidiFile file_;

	// Events need to be stored here, because they are only referenced from handles
	EventContainer events_;
};