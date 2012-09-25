#include "TempoFollower.h"

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

	void operator() (Data::Fermata const & fermata) const
	{
		Fermata f(fermata, parent_.tempoMap_);

		if (!parent_.fermatas_.AllEvents().Empty()) {
			assert(parent_.fermatas_.AllEvents().Back().data.end < f.tempoReference);
		}

		parent_.fermatas_.RegisterEvent(f.tempoReference.time(), f);
	}

private:
	TempoFollower & parent_;
};

} // namespace ScoreFollower
} // namespace cf
