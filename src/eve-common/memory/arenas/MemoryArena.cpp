#include "MemoryArena.h"

MemoryArena::MemoryArena(int memoryMarker) :
    mMemoryMarker (memoryMarker) {
}

void* MemoryArena::allocate (size_t size) {
    throw std::bad_alloc ();
}

void MemoryArena::deallocate (void* address) {
    throw std::bad_alloc ();
}
