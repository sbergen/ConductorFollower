#pragma once

namespace cf {
namespace FeatureExtractor {

class Event
{
public:
	// Types
	enum Type
	{
		TrackingStarted,
		TrackingEnded,
		Beat
	};

	Event(Type type) : type_(type) {}

	Type type() { return type_; }

private:
	Type type_;

};

} // namespace cf
} // namespace FeatureExtractor
