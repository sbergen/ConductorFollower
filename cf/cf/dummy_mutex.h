#pragma once

namespace cf {

// Dummy mutex for single-threaded lockfree things
class dummy_mutex
{
public:
    void lock() {}
    bool try_lock() {return true;}
    void unlock() {}
};

} // namespace cf