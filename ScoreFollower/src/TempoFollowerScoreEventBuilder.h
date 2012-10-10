#include "TempoFollower.h"

#include "Data/ScoreEvent.h"

namespace cf {
namespace ScoreFollower {

class TempoFollower::ScoreEventBuilder
{
public:
	typedef void result_type;

	ScoreEventBuilder(TempoFollower & parent)
		: parent_(parent) {}

	void operator() (Data::TempoSensitivityChange const & change) const
	{
		auto scoreTime = parent_.tempoMap_.TimeAt(change.position);
		parent_.tempoSensitivities_.RegisterEvent(scoreTime, change);
	}

	void operator() (Data::Fermata const &) const
	{
		// TODO not implemented
	}

private:
	TempoFollower & parent_;
};

} // namespace ScoreFollower
} // namespace cf
