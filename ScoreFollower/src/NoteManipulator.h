#pragma once

#include <vector>

#include <boost/utility.hpp>

#include "Data/InstrumentParser.h"
#include "Data/Score.h"

namespace cf {
namespace ScoreFollower {

// TODO perhaps rename this later? not quite sure what it's going to be yet...
class NoteManipulator : public boost::noncopyable
{
public:
	void LearnInstruments(Data::InstrumentMap const & instruments, Data::TrackList const & tracks);

private:
	std::vector<Data::Instrument> trackInstruments_;
};

} // namespace ScoreFollower
} // namespace cf