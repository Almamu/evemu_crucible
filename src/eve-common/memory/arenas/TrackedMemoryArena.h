#pragma once

#include "HeapMemoryArena.h"
#include <vector>

/**
 * Memory arena that allocates elements into the heap, keeping track of what
 * this arena owns to be destroyed once the arena is freed
 */
template <class T = MemoryArena>
class TrackedMemoryArena : public HeapMemoryArena<T> {
public:
    TrackedMemoryArena () : HeapMemoryArena<T> (), mObjects () {}

    ~TrackedMemoryArena () {
        this->clear();
    }

    [[nodiscard]] void* allocate (size_t size) override {
        void* address = HeapMemoryArena<T>::allocate(size);

        mUsedBytes += size;

        mObjects.push_back({.size = size, .pointer = address});

        return address;
    }


    void deallocate (void* address) override {
        auto it = std::find_if(mObjects.begin(), mObjects.end(), [address](TrackedEntity entry) { return address == entry.pointer; });

        // TODO: THROW EXCEPTION?!
        if (it == mObjects.end()) {
            return;
        }

        mUsedBytes -= it->size;
        mObjects.erase(it);

        HeapMemoryArena<T>::deallocate (address);
    }

    void clear() {
        for (auto entry : mObjects) {
            HeapMemoryArena<T>::deallocate (entry.pointer);
        }

        mObjects.clear();
        mUsedBytes = 0;
    }

    [[nodiscard]] const int usedBytes() const {
        return mUsedBytes;
    }

protected:
    struct TrackedEntity {
        size_t size;
        void* pointer;
    };

    std::vector<TrackedEntity> mObjects;
    int mUsedBytes;
};
