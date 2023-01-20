#pragma once

#include "HeapMemoryArena.h"

/**
 * A memory arena that doesn't allow deletion until the arena itself is destroyed
 *
 * Uses the heap as location to store the data
 */
template <class T = MemoryArena>
class PerpetualMemoryArena : public HeapMemoryArena<T> {
public:
    void deallocate (void* address) override {
        throw std::runtime_error("Deallocation request for an object inside a perpetual memory arena");
    }
};
