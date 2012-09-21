#pragma once

#include <boost/variant.hpp>

#include "cf/time.h"

namespace cf {
namespace ScoreFollower {

struct BeatEvent
{
	BeatEvent(double phase, double offset)
		: phase(phase), offset(offset) {}

	double phase;
	double offset;
};

struct BarPhaseEvent
{
	BarPhaseEvent(double phase)
		: phase(phase) {}

	double phase;
};

class StatusEvent
{
public:
	// Types
	enum Type
	{
		Invalid = -1,
		BarPhase,
		Beat
	};

	// Data
	typedef boost::variant<
		boost::blank,
		BeatEvent,
		BarPhaseEvent
		> Data;

	// Ctors
	StatusEvent() : type_(Invalid) {}

	template<typename TData>
	StatusEvent(timestamp_t const & timestamp, Type type, TData data)
		: timestamp_(timestamp)
		, type_(type)
		, data_(data)
	{
	}

	// Data access
	timestamp_t timestamp() const { return timestamp_; }

	Type type() const { return type_; }

	Data data() const { return data_; }
	template <typename T> T const & data() const { return boost::get<T const &>(data_); }

private:
	timestamp_t timestamp_;
	Type type_;
	Data data_;
};

class StatusEventProvider
{
public:
	virtual ~StatusEventProvider() {};

	virtual bool DequeueEvent(StatusEvent & result) = 0;
};

} // namespace ScoreFollower
} // namespace cf
