#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include "ScoreFollower/types.h"
#include "ScoreFollower/ScoreEvent.h"

namespace cf {
namespace ScoreFollower {

template<typename TData> class TrackReader;

typedef boost::shared_ptr<TrackReader<ScoreEventPtr> > TrackReaderPtr;
typedef boost::shared_ptr<TrackReader<tempo_t> > TempoReaderPtr;

// Abstraction of reading a score (e.g. midi)
class ScoreReader : public boost::noncopyable
{
public:
	virtual ~ScoreReader()  {}

	virtual int TrackCount() const = 0;
	virtual TrackReaderPtr Track(int index) = 0;
	virtual TempoReaderPtr TempoTrack() = 0;
	
};

} // namespace ScoreFollower
} // namespace cf
