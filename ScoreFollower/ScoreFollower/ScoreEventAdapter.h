#pragma once

#include "ScoreFollower/ScoreEventHandle.h"

namespace cf {
namespace ScoreFollower {

// Adapter for ScoreEventHandle, to be typedeffed on client side.
// By typedeffing the template parameter, we get uniform conversions,
// and using ScoreEventHandle is safer and simpler
template<typename Adapted>
class ScoreEventAdapter
{
public:
	// Creation,
	static ScoreEventHandle Create(Adapted & data)
		{ return ScoreEventHandle::Create(data); }

	// const and ...
	static Adapted const & Adapt(ScoreEventHandle const & handle)
		{ return handle.data<Adapted>(); }

	// non-const "factory" functions
	static Adapted & Adapt(ScoreEventHandle & handle)
		{ return handle.data<Adapted>(); }
};


} // namespace ScoreFollower
} // namespace cf
