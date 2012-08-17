#pragma once

#include "cf/time.h"
#include "cf/score_units.h"

namespace cf {
namespace ScoreFollower {

namespace bu = boost::units;

// Wall time uses Boost.Chrono
typedef timestamp_t real_time_t;

// Score time, beats and tempo in Boost.Units
typedef bu::quantity<score::physical_time> score_time_t;
typedef bu::quantity<score::tempo> tempo_t;
typedef bu::quantity<score::musical_time> beat_pos_t;
typedef bu::quantity<score::bar_count> bar_pos_t;
typedef bu::quantity<score::bar_duration> bar_duration_t;
typedef beat_pos_t beats_t;
typedef bar_pos_t bars_t;

// Speed is relative speed (i.e. 1.0 == normal speed)
// i.e. dimensionless in Boost.Units
typedef double speed_t;

// Sample related stuff
typedef bu::quantity<score::samplerate> samplerate_t;
typedef bu::quantity<score::sample_count> samples_t;

} // namespace ScoreFollower
} // namespace cf
