#pragma once

#include <boost/units/conversion.hpp>
#include <boost/units/base_dimension.hpp>
#include <boost/units/base_unit.hpp>
#include <boost/units/unit.hpp>
#include <boost/units/static_constant.hpp>
#include <boost/units/quantity.hpp>
#include <boost/units/physical_dimensions/time.hpp>
#include <boost/units/make_system.hpp>
#include <boost/units/make_scaled_unit.hpp>

#include "cf/time.h"

namespace cf {
namespace score {

namespace bu = boost::units;

// Base dimensions
struct beat_base_dimension : public bu::base_dimension<beat_base_dimension, 1> {};
typedef bu::time_base_dimension time_base_dimension;

// Direct dimensions
typedef beat_base_dimension::dimension_type beat_dimension;
typedef bu::time_dimension time_dimension;

// Derived dimensions (these have no base dimension or unit)
typedef bu::derived_dimension<
	beat_base_dimension, 1,
	time_base_dimension, -1>::type tempo_dimension;

// Base units
struct beat_base_unit : public bu::base_unit<beat_base_unit, beat_dimension, 1> {};
//struct second_base_unit : public bu::base_unit<second_base_unit, time_dimension, 2> {};
typedef bu::si::second_base_unit second_base_unit;

// system
typedef bu::make_system<
    beat_base_unit,
	second_base_unit>::type system;

// Units

typedef bu::unit<bu::dimensionless_type, system> dimensionless;

typedef bu::unit<beat_dimension, system> musical_time;
BOOST_UNITS_STATIC_CONSTANT(beat, musical_time);
BOOST_UNITS_STATIC_CONSTANT(beats, musical_time);
BOOST_UNITS_STATIC_CONSTANT(quarter_note, musical_time);
BOOST_UNITS_STATIC_CONSTANT(quarter_notes, musical_time);

typedef bu::unit<time_dimension, system> physical_time;
BOOST_UNITS_STATIC_CONSTANT(second, physical_time);
BOOST_UNITS_STATIC_CONSTANT(seconds, physical_time);

typedef bu::unit<tempo_dimension, system> tempo;
BOOST_UNITS_STATIC_CONSTANT(beats_per_second, tempo);

// Beat divisions

typedef bu::make_scaled_unit<musical_time, bu::scale<2, bu::static_rational<2> > >::type full_note_duration;
BOOST_UNITS_STATIC_CONSTANT(full_note, full_note_duration);
BOOST_UNITS_STATIC_CONSTANT(full_notes, full_note_duration);

typedef bu::make_scaled_unit<musical_time, bu::scale<2, bu::static_rational<1> > >::type half_note_duration;
BOOST_UNITS_STATIC_CONSTANT(half_note, half_note_duration);
BOOST_UNITS_STATIC_CONSTANT(half_notes, half_note_duration);

typedef bu::make_scaled_unit<musical_time, bu::scale<2, bu::static_rational<-1> > >::type eight_note_duration;
BOOST_UNITS_STATIC_CONSTANT(eight_note, eight_note_duration);
BOOST_UNITS_STATIC_CONSTANT(eight_notes, eight_note_duration);

typedef bu::make_scaled_unit<musical_time, bu::scale<2, bu::static_rational<-2> > >::type sixteenth_note_duration;
BOOST_UNITS_STATIC_CONSTANT(sixteenth_note, sixteenth_note_duration);
BOOST_UNITS_STATIC_CONSTANT(sixteenth_notes, sixteenth_note_duration);


} // namespace score
} // namespace cf
