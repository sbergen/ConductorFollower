#pragma once

#include "cf/time.h"

namespace cf {
namespace ScoreFollower {

typedef microseconds_t score_time_t;
typedef timestamp_t real_time_t;

// Tempo is microseconds per quarter note 
typedef microseconds_t tempo_t;

// Speed is relative speed (i.e. 1.0 == normal speed)
typedef double speed_t;

// Beat position is plain beats, including fraction
typedef double beat_pos_t;

} // namespace ScoreFollower
} // namespace cf
