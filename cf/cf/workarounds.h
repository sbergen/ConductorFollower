#pragma once

#include <boost/utility/declval.hpp>

// MSVC 2010 or older
#if defined(_MSC_VER) && (_MSC_VER <= 1600)

#define noexcept

#endif // MSVC 2010 or older