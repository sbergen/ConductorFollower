#pragma once

#include <array>
#include <cassert>

namespace cf {

#if defined(_MSC_VER) && (_MSC_VER <= 1600)
#define noexcept
#endif

// Bounded vecotr class
// TODO does not run destructors on resizes!
template <typename T, size_t Size>
class bounded_vector {
private:
	typedef std::array<T, Size> array_type;

public: // types
	typedef typename array_type::value_type value_type;

	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef typename array_type::iterator iterator;
	typedef typename array_type::const_iterator const_iterator;
	typedef typename array_type::size_type size_type;
	typedef typename array_type::difference_type difference_type;

	typedef typename array_type::pointer pointer;
	typedef typename array_type::const_pointer const_pointer;

	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

public: // construct/copy/destroy
	bounded_vector()
		: size_(0)
	{
		array_.fill(T());
	}

	explicit bounded_vector(size_type n)
		: size_(n)
	{
		assert(n <= Size);
		array_.fill(T());
	}
	
	bounded_vector(size_type n, const T& value)
		: size_(n)
	{
		assert(n <= Size);
		array_.fill(value);
	}
	
	template <class InputIterator>
	bounded_vector(InputIterator first, InputIterator last)
	{
		array_.fill(T());
		assign(first, last);
	}

	bounded_vector(const bounded_vector& other)
		: size_(other.size_)
		, array_(other.array_)
	{}

	bounded_vector(bounded_vector&& other)
		: size_(other.size_)
		, array_(other.array_)
	{}

	//bounded_vector(initializer_list<T>);

	~bounded_vector() {}

	bounded_vector& operator=(const bounded_vector& other)
	{
		if (*this == other) { return; }
		size_ = other.size_;
		array_ = ther.array_;
	}

	bounded_vector& operator=(bounded_vector&& other)
	{
		swap(other);
	}

	//bounded_vector& operator=(initializer_list<T>);

	template <class InputIterator>
	void assign(InputIterator first, InputIterator last)
	{
		assert(std::distance(first, last) <= Size);
		std::copy(first, last, std::begin(array_));
	}

	void assign(size_type n, const T& u)
	{
		size_ = n;
		array_.fill(u);
	}

	//void assign(initializer_list<T>);

public: // iterators
	iterator begin() noexcept { return array_.begin(); }
	const_iterator begin() const noexcept { return array_.begin(); }
	iterator end() noexcept { return begin() + size_; }
	const_iterator end() const noexcept { return begin() + size_; }

	reverse_iterator rbegin() noexcept { return array_.rbegin(); }
	const_reverse_iterator rbegin() const noexcept { return array_.rbegin(); }
	reverse_iterator rend() noexcept { return array_.rbegin() + size_; }
	const_reverse_iterator rend() const noexcept { return array_.rbegin() + size_; }

	const_iterator cbegin() const noexcept { return array_.cbegin(); }
	const_iterator cend() const noexcept { return array_.cbegin() + size_; }

	const_reverse_iterator crbegin() const noexcept { return array_.crbegin(); }
	const_reverse_iterator crend() const noexcept { return array_.crbegin() + size_; }

public:	// capacity
	size_type size() const noexcept { return size_; }
	size_type max_size() const noexcept  { return Size; }

	void resize(size_type sz)
	{
		assert(sz <= Size);
		size_ = sz;
	}

	void resize(size_type sz, const T& c) { assing(sz, c); }
	size_type capacity() const noexcept { return size_; }
	bool empty() const noexcept { return size_ == 0; }
	void reserve(size_type n) { assert(n <= Size); }
	void shrink_to_fit() {}

public: // element access
	reference operator[](size_type n) { return array_[n]; }
	const_reference operator[](size_type n) const { return array_[n]; }

	const_reference at(size_type n) const { return array_.at(n); }
	reference at(size_type n) { return array_.at(n); }

	reference front() { return array_.front(); }
	const_reference front() const { return array_.front(); }

	reference back() { return array_.back(); }
	const_reference back() const { return array_.back(); }
	
public: // data access
	T* data() noexcept { return array_.data(); }
	const T* data() const noexcept { return array_.data(); }
	
public: // modifiers:
	//template <class... Args> void emplace_back(Args&&... args);
	void push_back(const T& x) { array_[size_++] = x; }
	void push_back(T&& x) { array_[size_++] = x; }
	void pop_back() { --size_; }

	//template <class... Args> iterator emplace(const_iterator position, Args&&... args);

	iterator insert(const_iterator position, const T& x)
	{
		insert(T(x));
	}

	iterator insert(const_iterator position, T&& x)
	{
		assert(size_ < Size);
		auto distance = std::distance(cbegin(), position);
		auto insertIt = begin() + distance;
		std::move(position, end(), insertIt + 1);
		*insertIt = x;
		++size_;
	}

	iterator insert(const_iterator position, size_type n, const T& x)
	{
		assert(size_ + n <= Size);
		auto distance = std::distance(cbegin(), position);
		auto insertIt = begin() + distance;
		std::move(position, end(), insertIt + n);
		std::fill(insertIt, insertIt + n, x);
		size_ += n;
	}

	template <class InputIterator>
	iterator insert(const_iterator position, InputIterator first, InputIterator last)
	{
		auto n = std::distance(first, last);
		assert(size_ + n <= Size);
		auto distance = std::distance(cbegin(), position);
		auto insertIt = begin() + distance;
		std::move(position, end(), insertIt + n);
		std::copy(fist, last, insertIt);
		size_ += n;
	}

	//iterator insert(const_iterator position, initializer_list<T> il);
	
	iterator erase(const_iterator position)
	{
		auto distance = std::distance(cbegin(), position);
		auto insertIt = begin() + distance;
		std::move(begin() + distance + 1, end(), insertIt);
		--size_;
	}

	iterator erase(const_iterator first, const_iterator last)
	{
		auto fDistance = std::distance(cbegin(), first);
		auto lDistance = std::distance(cbegin(), last);
		auto insertIt = begin() + fDistance;
		std::move(begin() + lDistance + 1, end(), insertIt);
		size_ -= lDistance - fDistance;
	}

	void swap(bounded_vector& other)
	{
		std::swap(array_, other.array_);
		std::swap(size_, other.size_);
	}

	void clear() noexcept
	{
		size_ = 0;
	}

private:
	array_type array_;
	size_type size_;

};

// comparison
template <class T, size_t Size>
bool operator==(const bounded_vector<T, Size>& x, const bounded_vector<T, Size>& y)
{
	if (&x == &y) { return true; }
	if (x.size_ != y.size_) { return false; }
	return std::equal(x.begin(), x.end(), y.begin());
}

template <class T, size_t Size>
bool operator< (const bounded_vector<T, Size>& x, const bounded_vector<T, Size>& y)
{
	return std::lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}

template <class T, size_t Size>
bool operator!=(const bounded_vector<T, Size>& x, const bounded_vector<T, Size>& y)
{
	return !(x == y);
}

template <class T, size_t Size>
bool operator> (const bounded_vector<T, Size>& x, const bounded_vector<T, Size>& y)
{
	return y < x;
}

template <class T, size_t Size>
bool operator>=(const bounded_vector<T, Size>& x, const bounded_vector<T, Size>& y)
{
	return !(x < y);
}

template <class T, size_t Size>
bool operator<=(const bounded_vector<T, Size>& x, const bounded_vector<T, Size>& y)
{
	return !(x > y);
}

// specialized algorithms:
template <class T, size_t Size>
void swap(bounded_vector<T, Size>& x, bounded_vector<T, Size>& y)
{
	x.swap(y);
}

#ifdef noexcept
#undef noexcept
#endif

} // namespace cf