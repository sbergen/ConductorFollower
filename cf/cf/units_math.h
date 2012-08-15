#pragma once

#include <boost/units/cmath.hpp>

namespace boost { namespace units {

// Alias max -> fmax
template<class Unit,class Y>
inline quantity<Unit,Y> 
max (const quantity<Unit,Y>& q1, const quantity<Unit,Y>& q2)
{
    return fmax(q1, q2);
}

// Alias min -> fmin
template<class Unit,class Y>
inline quantity<Unit,Y> 
min (const quantity<Unit,Y>& q1, const quantity<Unit,Y>& q2)
{
    return fmin(q1, q2);
}

} } // units, boost