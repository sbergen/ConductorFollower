#pragma once

#include "cf/cf.h"

namespace cf {
namespace ScoreFollower {

typedef boost::chrono::microseconds score_time_t;
typedef cf::timestamp_t real_time_t;

// Tempo is microseconds per quarter note 
typedef boost::chrono::microseconds tempo_t;

} // namespace ScoreFollower
} // namespace cf
