#pragma once

#include <stdexcept>

#include "memory/Memory.h"
#include "python/Types.h"

/**
 * Special version of a memory arena geared towards python types. Provides convenient methods to create new objects
 * in the arena and serves as base type for passing things around without being specific
 */
class PythonArena : public MemoryArena {
    friend PyObjectEx;
    friend PyPackedRow;
    friend PySubStream;
    friend PyObjectEx_Type1;
    friend PyObjectEx_Type2;
public:
    // TODO: HATE MY LIFE, WE SHOULD FIND A BETTER NAME FOR THESE METHODS
    // TODO: BECAUSE I WAS MOVING AWAY FROM THE PASCAL CASE METHOD NAMES T.T
    [[nodiscard]] PyInt* Int(int64 value, std::source_location location = std::source_location::current());
    [[nodiscard]] PyFloat* Float(double value, std::source_location location = std::source_location::current());
    [[nodiscard]] PyBool* Bool (bool value, std::source_location location = std::source_location::current());
    [[nodiscard]] PyNone* None (std::source_location location = std::source_location::current());
    [[nodiscard]] PyString* String (std::string content, bool isUTF8 = false, std::source_location location = std::source_location::current());
    [[nodiscard]] PyString* String (const char* str, size_t len, bool isUTF = false, std::source_location location = std::source_location::current());
    [[nodiscard]] PyToken* Token (std::string content, std::source_location location = std::source_location::current());
    template <typename Iter>
    [[nodiscard]] PyBuffer* Buffer (Iter first, Iter second, std::source_location location = std::source_location::current()) {
        return new(this, location) PyBuffer (first, second);
    }
    [[nodiscard]] PyBuffer* Buffer (const uint8& fill, size_t length, std::source_location location = std::source_location::current());
    [[nodiscard]] PyBuffer* Buffer (const class Buffer& buffer, std::source_location location = std::source_location::current());
    [[nodiscard]] PyTuple* Tuple (std::source_location location = std::source_location::current());
    [[nodiscard]] PyTuple* Tuple (std::initializer_list<PyTuple::storage_type::value_type> init, bool checkArenaOwnership = true, std::source_location location = std::source_location::current());
    [[nodiscard]] PyTuple* Tuple (PyTuple::storage_type data, bool checkArenaOwnership = true, std::source_location location = std::source_location::current());
    [[nodiscard]] PyList* List (std::source_location location = std::source_location::current());
    [[nodiscard]] PyList* List (std::initializer_list<PyList::storage_type::value_type> init, bool checkArenaOwnership = true, std::source_location location = std::source_location::current());
    [[nodiscard]] PyList* List (PyList::storage_type data, bool checkArenaOwnership = true, std::source_location location = std::source_location::current());
    [[nodiscard]] PyList* List (size_t size, std::source_location location = std::source_location::current());
    [[nodiscard]] PyDict* Dict (std::source_location location = std::source_location::current());
    [[nodiscard]] PyDict* Dict (std::initializer_list<PyDict::storage_type::value_type> init, bool checkArenaOwnership = true, std::source_location location = std::source_location::current());
    [[nodiscard]] PyDict* Dict (std::initializer_list <std::map<std::string,PyDataType*>::value_type> init, bool checkArenaOwnership = true, std::source_location location = std::source_location::current());
    [[nodiscard]] PyDict* Dict (PyDict::storage_type data, bool checkArenaOwnership = true, std::source_location location = std::source_location::current());
    [[nodiscard]] PySubStruct* SubStruct (PyDataType* content, bool checkArenaOwnership = true, std::source_location location = std::source_location::current());
    [[nodiscard]] PySubStream* SubStream (PyDataType* content, bool checksumed = false, bool checkArenaOwnership = true, std::source_location location = std::source_location::current());
    [[nodiscard]] PySubStream* SubStream (const class Buffer& content, bool checksumed = false, std::source_location location = std::source_location::current());
    [[nodiscard]] PyObject* Object (const std::string& type, PyDataType* args, bool checkArenaOwnership = true, std::source_location location = std::source_location::current());
    [[nodiscard]] PyObject* Object (PyString* type, PyDataType* args, bool checkArenaOwnership = true, std::source_location location = std::source_location::current());
    [[nodiscard]] PyObjectEx* ObjectEx (bool isType2, PyDataType* header, bool checkArenaOwnership = true, std::source_location location = std::source_location::current());
    [[nodiscard]] PyObjectEx_Type1* ObjectEx_Type1 (PyToken* type, PyTuple* args, bool enclosed = false, bool checkArenaOwnership = true, std::source_location location = std::source_location::current());
    [[nodiscard]] PyObjectEx_Type1* ObjectEx_Type1 (PyObjectEx_Type1* args1, PyTuple* args2, bool enclosed = false, bool checkArenaOwnership = true, std::source_location location = std::source_location::current());
    [[nodiscard]] PyObjectEx_Type1* ObjectEx_Type1 (PyToken* type, PyTuple* args, PyDict* keywords, bool enclosed = false, bool checkArenaOwnership = true, std::source_location location = std::source_location::current());
    [[nodiscard]] PyObjectEx_Type1* ObjectEx_Type1 (PyToken* type, PyTuple* args, PyList* keywords, bool enclosed = false, bool checkArenaOwnership = true, std::source_location location = std::source_location::current());
    [[nodiscard]] PyObjectEx_Type2* ObjectEx_Type2 (PyTuple* args, PyDict* keywords, bool enclosed = false, bool checkArenaOwnership = true, std::source_location location = std::source_location::current());
    [[nodiscard]] PyObjectEx_Type2* ObjectEx_Type2 (PyToken* args, PyDict* keywords, bool enclosed = false, bool checkArenaOwnership = true, std::source_location location = std::source_location::current());
    [[nodiscard]] PyPackedRow* PackedRow(DBRowDescriptor* header, bool checkArenaOwnership = true, std::source_location location = std::source_location::current());
    [[nodiscard]] PyPackedRow* PackedRow(DBRowDescriptor* header, std::initializer_list <PyDataType*> values, bool checkArenaOwnership = true, std::source_location location = std::source_location::current());

protected:
    [[nodiscard]] PySubStream* SubStream (std::optional<class Buffer> encoded, std::optional<PyDataType*> decoded, bool checksumed, bool checkArenaOwnership = true, std::source_location location = std::source_location::current());
    [[nodiscard]] PyObjectEx* ObjectEx (bool isType2, PyDataType* header, PyList* list, PyDict* dict, bool checkArenaOwnership = true, std::source_location location = std::source_location::current());
    [[nodiscard]] PyObjectEx_Type1* ObjectEx_Type1 (PyDataType* header, PyList* list, PyDict* dict, bool checkArenaOwnership = true, std::source_location location = std::source_location::current());
    [[nodiscard]] PyObjectEx_Type2* ObjectEx_Type2 (PyDataType* header, PyList* list, PyDict* dict, bool checkArenaOwnership = true, std::source_location location = std::source_location::current());
    [[nodiscard]] PyPackedRow* PackedRow(DBRowDescriptor* header, PyList* fields, bool checkArenaOwnership = true, std::source_location location = std::source_location::current());
};

// specialized versions of the memory arenas to provide the functions to construct python objects
class BookmarkedPythonArena : public BookmarkedMemoryArena <PythonArena> {
public:
    explicit BookmarkedPythonArena (size_t reservedSize) : BookmarkedMemoryArena <PythonArena> (reservedSize) {}
};
class HeapPythonArena : public HeapMemoryArena <PythonArena> {
public:
    // TODO: USE A SINGLETON HERE? IT'S NOT REALLY A SINGLETON CLASS PER SE, LOOK INTO THIS
    static HeapPythonArena* instance;
};
class PerpetualPythonArena : public PerpetualMemoryArena <PythonArena> {};
class StaticPythonArena : public StaticMemoryArena <PythonArena> {
public:
    explicit StaticPythonArena (size_t reservedSize) : StaticMemoryArena <PythonArena> (reservedSize) {}
};
class TrackedPythonArena : public TrackedMemoryArena <PythonArena> {
public:
    explicit TrackedPythonArena () : TrackedMemoryArena <PythonArena> () {}
};