#include "NoteManipulator.h"

namespace cf {
namespace ScoreFollower {

void
NoteManipulator::LearnInstruments(Data::InstrumentMap const & instruments, Data::TrackList const & tracks)
{
	trackInstruments_.clear();
	trackInstruments_.reserve(tracks.size());

	for(auto it = tracks.begin(); it != tracks.end(); ++it) {
		auto instrumentIt = instruments.find(it->instrument);
		if (instrumentIt == instruments.end()) {
			throw std::runtime_error("Instrument not found!");
		}

		trackInstruments_.push_back(instrumentIt->second);
	}
}

} // namespace ScoreFollower
} // namespace cf