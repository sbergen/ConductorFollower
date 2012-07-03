#pragma once

namespace cf {
namespace ScoreFollower {

// Glorified void *
class ScoreEventHandle
{
public:
	// TODO think about this a bit more stil...
	ScoreEventHandle() : data_(nullptr) {}

	template<typename T>
	ScoreEventHandle(T & t) : data_(&t)
	{}

	template<typename T>
	T & data() { return *static_cast<T *>(data_); }

	template<typename T>
	T const & data() const { return *static_cast<T const *>(data_); }

private:
	void * data_;
};

} // namespace ScoreFollower
} // namespace cf
