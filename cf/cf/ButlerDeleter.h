#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include "cf/ButlerThread.h"

namespace cf {

class ButlerDeleter : boost::noncopyable
{
public:
	ButlerDeleter(ButlerThread & butler)
		: butler_(butler) {}

	template<typename T>
	void operator()(T * ptr)
	{
		butler_.ScheduleDelete(ptr);
	}


private:
	ButlerThread & butler_;
};

// NOTE, parts of the shared_ptr get freed immediately with delete! Not RT safe
template<typename T>
boost::shared_ptr<T> butler_deletable_copy(T const & value, ButlerThread & butler)
{
	return boost::shared_ptr<T>(new T(value), ButlerDeleter(butler));
}

} // namespace cf
