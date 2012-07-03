#pragma once

#include <vector>

#include <boost/signals2.hpp>
#include <boost/signals2/mutex.hpp>
#include <boost/utility.hpp>

#include "cf/cf.h"
#include "cf/EventBuffer.h"

namespace cf {
namespace FeatureExtractor {

namespace bs2 = boost::signals2;

typedef EventBuffer<double, timestamp_t> GestureBuffer;

class Extractor : public boost::noncopyable
{
public:
	// Hide implementation
	static Extractor * Create();

public: // types
	typedef bs2::signal_type<void (timestamp_t const &), bs2::keywords::mutex_type<bs2::dummy_mutex> >::type EventSignal;

	typedef bs2::signal_type<void (timestamp_t const & /* down beat time */,
	                               timestamp_t const & /* estimated start time */),
	                         bs2::keywords::mutex_type<bs2::dummy_mutex> >::type StartGestureSignal;

public:
	virtual ~Extractor() {}

	virtual void RegisterPosition(timestamp_t const & time, Point3D const & pos) = 0;
	virtual Point3D MagnitudeOfMovementSince(timestamp_t const & time) = 0;

	EventSignal BeatDetected;
	EventSignal ApexDetected;
	StartGestureSignal StartGestureDetected;
};

} // namespace FeatureExtractor
} // namespace cf
