#pragma once

#include "eve-core.h"
#include <source_location>

#include "arenas/MemoryArena.h"
#include "arenas/HeapMemoryArena.h"

/**
 * Base class for all objects that can be allocated on a memory arena.
 *
 * Provides helper methods + operators for allocation/deallocation
 */
template <class DefaultArena, class T = MemoryArena>
class ArenaAware {
public:
    ArenaAware() {
        mOrigin = MemoryArena::objectToAllocation ((void*) this)->filename;
    }

    // IMPORTANT, DO NOT REMOVE!!! THIS ENSURES THAT "this" POINTS TO THE REAL ADDRESS OF THE OBJECT
    // SO THE ARENA FUNCTION STARTS READING OFF THE RIGHT PLACE
    // THIS MIGHT BE COMPILER-DEPENDANT, SO WE MIGHT NEED TO PROVIDE SPECIFIC VERSIONS OF THIS CLASS
    // FOR DIFFERENT COMPILERS
    // THIS IS NEEDED BECAUSE OF THE WAY THE ARENA DATA IS TRACKED, ALLOCATORS ALLOCATE THE SIZE OF THE CLASS
    // PLUS A HEADER STRUCT. THIS HEADER STRUCT IS WRITTEN BEFORE THE ACTUAL OBJECT'S POINTER (AND IS PART OF THE ALLOCATION)
    // THUS WE NEED A POINTER TO THE BEGINNING OF THE OBJECT DATA IN THE ALLOCATED MEMORY BLOCK
    //
    // THIS IS SOLVED BY THE VIRTUAL DESTRUCTOR, WHICH CHANGES HOW THE MEMORY IS LAID OUT FOR CLASSES THAT INHERIT FROM THIS
    // CLASS, ENSURING THAT THE "this" POINTER ACTUALLY POINTS TO THE FULL OBJECT, NOT ONE OF THE INHERITED CLASSES
    virtual ~ArenaAware() {}

    /**
     * @return The arena used to allocate this
     */
    [[nodiscard]] T* arena () const {
        return static_cast<T*> (MemoryArena::objectToAllocation((void*) this)->arena);
    }

    /**
     * @return An allocator associated with the objet's arena that can be used by the standard library
     */
    template <class Container>
    [[nodiscard]] ArenaAllocatorWrapper<Container*> allocator() const {
        return this->arena()->template allocator<Container*>();
    }

    /**
     * @return The location where the allocation of this object happened
     */
    [[nodiscard]] std::string* origin() const {
        return mOrigin;
    }

    // TODO: DISABLE SOURCE INFORMATION ON RELEASE BUILDS?
    [[nodiscard]] void* operator new (size_t size, std::source_location location = std::source_location::current()) {
        return operator new (size, DefaultArena::instance, location);
    }

    [[nodiscard]] void* operator new (size_t size, T* arena, std::source_location location = std::source_location::current()) {
        void* pointer = arena->allocate (size);

        // get caller and plop it in the header too
        MemoryArena::AllocationHeader* header = MemoryArena::objectToAllocation (pointer);

        // build a new string
        std::stringstream stream;
        stream << location.file_name() << ":" << location.line();
        header->filename = new std::string(stream.str());

        return pointer;
    }

    void operator delete (void* pointer) {
        if (pointer == nullptr) {
            return;
        }

        MemoryArena::AllocationHeader* header = MemoryArena::objectToAllocation (pointer);

        delete header->filename;

        static_cast<ArenaAware<T>*> (pointer)->arena ()->deallocate (pointer);
    }

    void operator delete (void* pointer, T* arena) {
        if (pointer == nullptr) {
            return;
        }

        MemoryArena::AllocationHeader* header = MemoryArena::objectToAllocation (pointer);

        delete header->filename;

        static_cast<ArenaAware<T>*> (pointer)->arena ()->deallocate (pointer);
    }

    void operator delete (void* pointer, std::source_location location) {
        if (pointer == nullptr) {
            return;
        }

        MemoryArena::AllocationHeader* header = MemoryArena::objectToAllocation (pointer);

        delete header->filename;

        static_cast<ArenaAware<T>*> (pointer)->arena ()->deallocate (pointer);
    }

    void operator delete (void* pointer, T* arena, std::source_location location) {
        MemoryArena::AllocationHeader* header = MemoryArena::objectToAllocation (pointer);

        delete header->filename;

        arena->deallocate (pointer);
    }

protected:
    std::string* mOrigin;
};
