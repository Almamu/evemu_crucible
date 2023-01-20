#pragma once

#include <stdexcept>
#include <type_traits>
#include <memory>

template <class T>
class ArenaAllocatorWrapper;

/**
 * Base interface that defines how memory allocation/freeing actually works
 */
class MemoryArena {
public:
    /**
     * Reserves some memory in the memory arena for usage
     *
     * @param size The amount of bytes to reserve
     * @return Pointer to reserved memory
     */
    [[nodiscard]] virtual void* allocate (size_t size);

    /**
     * Marks reserved memory as not in use anymore
     *
     * @param start
     */
    virtual void deallocate (void* address);

    /**
     * Wraps this memory arena for usage with the standard C++ library
     *
     * @tparam T
     *
     * @return
     */
    template <class T>
    ArenaAllocatorWrapper<T> allocator () {
        return ArenaAllocatorWrapper<T> (this);
    }

    struct AllocationHeader {
        MemoryArena* arena;
        int marker;
        std::string* filename;
    };

    /**
     * Initializes the allocation with the proper data
     *
     * @param pointer The memory address where the allocation starts
     * @param arena The python arena to reference
     *
     * @return The final address of the object to be used
     */
    [[nodiscard]] static inline void* initializeAllocation(void* pointer, MemoryArena* arena) {
        auto data = reinterpret_cast<AllocationHeader*> (pointer);

        data->arena = arena;
        data->marker = arena->mMemoryMarker;

        return allocationToObject(pointer);
    }

    static inline void clearAllocation (void* pointer) {
        auto data = reinterpret_cast <AllocationHeader*> (pointer);

        // set marker to something that we can easily identify in the debugger
        data->marker = 0xDEADBEEF;
    }

    /**
     * Helper method that takes an object pointer and returns it's real memory block address
     *
     * @param pointer
     *
     * @return
     */
    [[nodiscard]] static inline AllocationHeader* objectToAllocation(void* pointer) {
        return reinterpret_cast<AllocationHeader*>(reinterpret_cast<uint8_t*>(pointer) - sizeof(AllocationHeader));
    }

    /**
     * Helper method that takes a memory block address and returns the object's pointer inside it
     * @param pointer
     * @return
     */
    [[nodiscard]] static inline void* allocationToObject(void* pointer) {
        return reinterpret_cast<void*>(reinterpret_cast<uint8_t*>(pointer) + sizeof(AllocationHeader));
    }

    /**
     * Helper method that obtains the required space for allocating a new object based off the object's size
     *
     * @param required
     *
     * @return
     */
    [[nodiscard]] static inline size_t sizeAllocation(size_t required) {
        return required + sizeof(AllocationHeader);
    }

  protected:
    MemoryArena(int memoryMarker = rand());

    /** The memory marker to use for this python arena */
    int mMemoryMarker;
};

template <class T>
class ArenaAllocatorWrapper : public std::allocator<T> {
public:
    using value_type = T;

    ArenaAllocatorWrapper(MemoryArena* arena) : mArena (arena) {}

    T* allocate (size_t count) {
        if (count > std::allocator_traits<ArenaAllocatorWrapper>::max_size(*this)) {
            throw std::bad_alloc();
        }

        return static_cast<T*> (mArena->allocate (count * sizeof (T)));
    }

    void deallocate (T* pointer, size_t) {
        mArena->deallocate (pointer);
    }

    void construct (T* p, const T& value) {
        _construct (p, value);
    }

    void destroy(T* p) {
        _destroy (p);
    }

private:
    MemoryArena* mArena;
};