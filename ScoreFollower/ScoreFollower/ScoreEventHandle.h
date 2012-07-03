#pragma once

namespace cf {
namespace ScoreFollower {

// Glorified void *
class ScoreEventHandle
{
public:
	ScoreEventHandle() : data_(nullptr) {}

	explicit ScoreEventHandle(ScoreEventHandle const & other)
		: data_(other.data_) {}

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
