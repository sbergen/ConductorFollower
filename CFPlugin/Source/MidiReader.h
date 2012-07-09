#pragma once

#include <string>

#include "JuceHeader.h"

#include "ScoreFollower/ScoreReader.h"
#include "ScoreFollower/TrackReader.h"

#include "MidiEvent.h"

namespace sf = cf::ScoreFollower;

class MidiReader : public sf::ScoreReader
{
public:
	MidiReader();
	~MidiReader();

public: // ScoreReader implementation
	void OpenFile(std::string const & filename);
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
	class TrackReaderImpl : public cf::ScoreFollower::TrackReader<sf::ScoreEventPtr>
	{
	public:
		TrackReaderImpl(MidiMessageSequence const & sequence);
		bool NextEvent(sf::score_time_t & timestamp, sf::ScoreEventPtr & data);
	
	protected:
		MidiMessageSequence const & sequence_;
		int count_;
		int current_;
	};

private:

	MidiFile file_;
};