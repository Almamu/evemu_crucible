#pragma once

#include "MemoryArena.h"
#include <vector>

/**
 * A basic python arena that allocates the data into the heap using the default new and delete operators
 */
template <class T = MemoryArena>
class HeapMemoryArena : public T {
public:
    ~HeapMemoryArena () = default;

    [[nodiscard]] void* allocate (size_t size) override {
        return MemoryArena::initializeAllocation(
            ::operator new (MemoryArena::sizeAllocation(size)),
            this
        );
    }

    void deallocate (void* address) override {
        ::operator delete (MemoryArena::objectToAllocation(address));
    }

protected:
    HeapMemoryArena () = default;
};
