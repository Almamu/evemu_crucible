#pragma once

#include <atomic>

#include "MemoryArena.h"

/**
 * A basic memory arena that reserves a static memory area to store python objects
 * that will be kept in memory until the arena is destroyed.
 *
 * This arena does not grow nor shrink during it's lifetime
 */
template <class T = MemoryArena>
class StaticMemoryArena : public T {
public:
    /**
     * @param reservedSize The byte amount to reserve
     */
    explicit StaticMemoryArena (size_t reservedSize) :
        mReservedSize (reservedSize),
        mMemory (static_cast<char*> (::operator new (reservedSize))),
        mFreeBytes (reservedSize),
        mCurrentOffset (0) {
        // sets the memory to a predictible value
        this->clear();
    }
    /**
     * Frees the allocated memory
     */
    ~StaticMemoryArena () {
        // free all used memory
        delete mMemory;
    }


    [[nodiscard]] void* allocate (size_t size) override {
        size = MemoryArena::sizeAllocation (size);
        mFreeBytes -= size;

        // TODO: TAKE ALIGNMENT INTO ACCOUNT TOO? (std::align)
        if (mFreeBytes < 0) {
            // recover the bytes left if anything else wants to use it so the arena is not in an inconsistent state
            mFreeBytes += size;
            throw std::bad_alloc ();
        }

        char* current = &mMemory [mCurrentOffset];

        mCurrentOffset += size;

        return MemoryArena::initializeAllocation (current, this);
    }

    void deallocate (void* address) override {
        // deallocation does nothing as this is a static memory arena
    }

    [[nodiscard]] const size_t freeBytes() const {
        return mFreeBytes;
    }

protected:
    /**
     * Clears all reserved memory on this arena
     */
    virtual void clear () {
        mFreeBytes.store(mReservedSize);
        mCurrentOffset = 0;

        // overwrite allocated memory with a value that is somewhat obvious
        memset(mMemory, 0xDE, mFreeBytes);
    }

    /** The total amount of bytes reserved for this arena */
    std::atomic<size_t> mReservedSize;
    /** The current amount of bytes left for usage in the arena */
    std::atomic<size_t> mFreeBytes;
    /** The current position inside the arena's memory for the next allocation */
    std::atomic<size_t> mCurrentOffset;
    /** The reserved memory area */
    char* mMemory;
};
