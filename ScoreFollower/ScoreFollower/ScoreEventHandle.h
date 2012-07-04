#pragma once

#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>

namespace cf {
namespace ScoreFollower {

// Glorified void *
class ScoreEventHandle
{
public:
	// Has to be default constructible, copy-constructible and assignable
	// in order to work with collections.
	// Default copy-ctor and assignment are fine
	ScoreEventHandle() : data_(nullptr) {}

private: // Explicit creation from and conversion to data only via ScoreEventAdapter
	template<typename T>
	friend class ScoreEventAdapter;
	
	template<typename T>
	static ScoreEventHandle Create(T & t)
	{
		return ScoreEventHandle(&t);
	}

	template<typename T>
	T & data() { return *static_cast<T *>(data_); }

	template<typename T>
	T const & data() const { return *static_cast<T const *>(data_); }

private:
	// Has to be a pointer type, otherwise messes up copy-ctor
	template<typename T>
	ScoreEventHandle(T * t)
		: data_(t)
	{
		BOOST_STATIC_ASSERT((!boost::is_same<T, ScoreEventHandle>::value));
	}

	void * data_;
};


} // namespace ScoreFollower
} // namespace cf
