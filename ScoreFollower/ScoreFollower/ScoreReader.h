#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include "ScoreFollower/types.h"
#include "ScoreFollower/TimeSignature.h"
#include "ScoreFollower/ScoreEvent.h"

namespace cf {
namespace ScoreFollower {

template<typename TData> class TrackReader;

typedef boost::shared_ptr<TrackReader<ScoreEventPtr> > TrackReaderPtr;
typedef boost::shared_ptr<TrackReader<tempo_t> > TempoReaderPtr;
typedef boost::shared_ptr<TrackReader<TimeSignature> > MeterReaderPtr;

// Abstraction of reading a score (e.g. midi)
class ScoreReader : public boost::noncopyable
{
public:
	virtual ~ScoreReader()  {}

	virtual void OpenFile(std::string const & filename) = 0;
	virtual int TrackCount() const = 0;
	virtual TrackReaderPtr Track(int index) = 0;
	virtual TempoReaderPtr TempoTrack() = 0;
	virtual MeterReaderPtr MeterTrack() = 0;
};

} // namespace ScoreFollower
} // namespace cf
