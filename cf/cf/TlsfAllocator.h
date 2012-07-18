#pragma once

#include <memory>

#include "tlsf/tlsf.h"

namespace cf {

// Non-templated pool for explicit init/deinit
class TlsfPool
{
public:
	static void Init(std::size_t size);
	static void Destroy();
	static tlsf_pool pool() { return pool_; }

private:
	static tlsf_pool pool_;
	static void * memory_;
};

// Templated allocator
template<typename T>
class TlsfAllocator
{
public: // required typedefs

	typedef T value_type;
	typedef value_type * pointer;
	typedef const value_type * const_pointer;
	typedef value_type & reference;
	typedef const value_type & const_reference;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;

    template <typename U>
    struct rebind
	{
		typedef TlsfAllocator<U> other;
    };

public: // Actual functionality

    pointer address(reference r) { return &r; }
    const_pointer address(const_reference s) { return &s; }

    void construct(const pointer ptr, const value_type & t) { new (ptr) T(t); }
    void destroy(const pointer ptr) { ptr->~T(); }

    pointer allocate(const size_type n) { return static_cast<pointer>(tlsf_malloc(TlsfPool::pool(), sizeof(T) * n)); }
    pointer allocate(const size_type n, const void * const) { return allocate(n); }
    static void deallocate(const pointer ptr, const size_type n) { tlsf_free(TlsfPool::pool(), ptr); }

	size_type max_size() { return (std::numeric_limits<size_type>::max)(); }

	bool operator==(const TlsfAllocator &) const { return true; }
    bool operator!=(const TlsfAllocator &) const { return false; }
};


// Specialization for void, required by the standard 
template<>
class TlsfAllocator<void>
{
public:
    typedef void*       pointer;
    typedef const void* const_pointer;
    typedef void        value_type;
    
    template <class U> 
    struct rebind
    {
       typedef TlsfAllocator<U> other;
    };
};

} // namespace cf