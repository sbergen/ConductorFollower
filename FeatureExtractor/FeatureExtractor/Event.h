#pragma once

#include "cf/cf.h"

namespace cf {
namespace FeatureExtractor {

class Event
{
public:
	// Types
	enum Type
	{
		Invalid = -1,
		TrackingStarted,
		TrackingEnded,
		Beat
	};

	Event() : type_(Invalid) {}

	Event(timestamp_t const & timestamp, Type type)
		: timestamp_(timestamp)
		, type_(type)
	{}

	timestamp_t timestamp() const { return timestamp_; }
	Type type() const { return type_; }

private:
	timestamp_t timestamp_;
	Type type_;
};

} // namespace FeatureExtractor
} // namespace cf
