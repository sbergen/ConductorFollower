#include "cf/TlsfAllocator.h"

namespace cf {

std::size_t TlsfPool::maxSize_ = 0;
tlsf_pool TlsfPool::pool_ = 0;
void * TlsfPool::memory_ = 0;

void
TlsfPool::Init(std::size_t size)
{
	maxSize_ = size;
	memory_ = new char[size];
	pool_ = tlsf_create(memory_, size);
}

void
TlsfPool::Destroy()
{
	tlsf_destroy(pool_);
	delete[] memory_;
}

} // namespace cf