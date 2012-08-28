#pragma once

#include "Data/ScoreEvent.h"

#include "ScorePosition.h"
#include "TempoMap.h"

namespace cf {
namespace ScoreFollower {

struct Fermata
{
	Fermata() {}

	Fermata(Data::Fermata const & fermata, TempoMap const & tempoMap)
		: tempoReference(tempoMap.TranslatePosition(fermata.tempoReference))
		, position(tempoMap.TranslatePosition(fermata.position))
		, end(tempoMap.TranslatePosition(fermata.end))
	{}

	ScorePosition tempoReference;
	ScorePosition position;
	ScorePosition end;
};

class FermataState
{
public:
	enum Action {
		NoAction,
		StoreReferenceTempo,
		EnterFermata
	};

public:
	FermataState()
		: state_(NotInitialized)
	{}

	bool IsInFermata() { return state_ == InFermata; }

	void Reset() { state_ = NotInitialized; }

	FermataState& operator=(Fermata const & fermata)
	{
		fermata_ = fermata;
		state_ = NoReference;
		return *this;
	}

	Action GetActionAtTime(score_time_t const & time)
	{
		switch(state_)
		{
			case NotInitialized:
				break;
			case NoReference:
				if (time >= fermata_.tempoReference.time()) {
					state_ = GotReference;
					return StoreReferenceTempo;
				}
				break;
			case GotReference:
				if (time >= fermata_.position.time()) {
					state_ = InFermata;
					return EnterFermata;
				}
				break;
			case InFermata:
				break;
		}

		return NoAction;
	}

	ScorePosition FermataEnd() { return fermata_.end; }

private:
	enum State {
		NotInitialized,
		NoReference,
		GotReference,
		InFermata
	};

private:
	State state_;
	Fermata fermata_;
};

} // namespace ScoreFollower
} // namespace cf
