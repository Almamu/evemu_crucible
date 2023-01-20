#include "PythonArena.h"

PyInt* PythonArena::Int (int64 value, std::source_location location) {
    return new(this, location) PyInt (value);
}

PyFloat* PythonArena::Float (double value, std::source_location location) {
    return new(this, location) PyFloat (value);
}

PyBool* PythonArena::Bool (bool value, std::source_location location) {
    return new(this, location) PyBool (value);
}

PyNone* PythonArena::None (std::source_location location) {
    return new(this, location) PyNone ();
}

PyString* PythonArena::String (std::string content, bool isUTF8, std::source_location location) {
    return new(this, location) PyString (content, isUTF8);
}

PyString* PythonArena::String (const char* str, size_t len, bool isUTF, std::source_location location) {
    return new(this, location) PyString (str, len, isUTF);
}

PyToken* PythonArena::Token (std::string content, std::source_location location) {
    return new(this, location) PyToken (content);
}

PyBuffer* PythonArena::Buffer (const uint8& fill, size_t length, std::source_location location) {
    return new(this, location) PyBuffer (fill, length);
}

PyBuffer* PythonArena::Buffer (const class Buffer& buffer, std::source_location location) {
    return new(this, location) PyBuffer (buffer);
}

PyTuple* PythonArena::Tuple (std::source_location location) {
    return new(this, location) PyTuple ();
}
PyTuple* PythonArena::Tuple (std::initializer_list<PyTuple::storage_type::value_type> init, bool checkArenaOwnership, std::source_location location) {
    return new(this, location) PyTuple (init, checkArenaOwnership);
}

PyTuple* PythonArena::Tuple (PyTuple::storage_type data, bool checkArenaOwnership, std::source_location location) {
    return new(this, location) PyTuple (data, checkArenaOwnership);
}
PyList* PythonArena::List (std::initializer_list<PyList::storage_type::value_type> init, bool checkArenaOwnership, std::source_location location) {
    return new(this, location) PyList (init, checkArenaOwnership);
}

PyList* PythonArena::List (std::source_location location) {
    return new(this, location) PyList ();
}
PyList* PythonArena::List (PyList::storage_type data, bool checkArenaOwnership, std::source_location location) {
    return new(this, location) PyList (data, checkArenaOwnership);
}

PyList* PythonArena::List (size_t size, std::source_location location) {
    return new(this, location) PyList (size);
}

PyDict* PythonArena::Dict (std::source_location location) {
    return new(this, location) PyDict ();
}

PyDict* PythonArena::Dict (std::initializer_list<PyDict::storage_type::value_type> init, bool checkArenaOwnership, std::source_location location) {
    return new(this, location) PyDict (init, checkArenaOwnership);
}

PyDict* PythonArena::Dict (std::initializer_list <std::map<std::string,PyDataType*>::value_type> init, bool checkArenaOwnership, std::source_location location) {
    return new(this, location) PyDict (init, checkArenaOwnership);
}

PyDict* PythonArena::Dict (PyDict::storage_type data, bool checkArenaOwnership, std::source_location location) {
    return new(this, location) PyDict (data, checkArenaOwnership);
}

PySubStruct* PythonArena::SubStruct(PyDataType *content, bool checkArenaOwnership, std::source_location location) {
    return new(this, location) PySubStruct (content, checkArenaOwnership);
}

PySubStream* PythonArena::SubStream (PyDataType* content, bool checksumed, bool checkArenaOwnership, std::source_location location) {
    return new(this, location) PySubStream (content, checksumed, checkArenaOwnership);
}

PySubStream* PythonArena::SubStream (const class Buffer& content, bool checksumed, std::source_location location) {
    return new(this, location) PySubStream (content, checksumed);
}
PySubStream* PythonArena::SubStream (std::optional<class Buffer> encoded, std::optional<PyDataType*> decoded, bool checksumed, bool checkArenaOwnership, std::source_location location) {
    return new(this, location) PySubStream (encoded, decoded, checksumed, checkArenaOwnership);
}
PyObject* PythonArena::Object (const std::string& type, PyDataType* args, bool checkArenaOwnership, std::source_location location) {
    return new(this, location) PyObject (type, args, checkArenaOwnership);
}

PyObject* PythonArena::Object (PyString* type, PyDataType* args, bool checkArenaOwnership, std::source_location location) {
    return new(this, location) PyObject (type, args, checkArenaOwnership);
}

PyObjectEx* PythonArena::ObjectEx (bool isType2, PyDataType* header, bool checkArenaOwnership, std::source_location location) {
    return new(this, location) PyObjectEx (isType2, header, checkArenaOwnership);
}

PyObjectEx* PythonArena::ObjectEx (bool isType2, PyDataType* header, PyList* list, PyDict* dict, bool checkArenaOwnership, std::source_location location) {
    return new(this, location) PyObjectEx (isType2, header, list, dict, checkArenaOwnership);
}

PyObjectEx_Type1* PythonArena::ObjectEx_Type1 (PyToken* type, PyTuple* args, bool enclosed, bool checkArenaOwnership, std::source_location location) {
    return new(this, location) PyObjectEx_Type1 (type, args, enclosed, checkArenaOwnership);
}

PyObjectEx_Type1* PythonArena::ObjectEx_Type1 (PyObjectEx_Type1* args1, PyTuple* args2, bool enclosed, bool checkArenaOwnership, std::source_location location) {
    return new(this, location) PyObjectEx_Type1 (args1, args2, enclosed, checkArenaOwnership);
}

PyObjectEx_Type1* PythonArena::ObjectEx_Type1 (PyToken* type, PyTuple* args, PyDict* keywords, bool enclosed, bool checkArenaOwnership, std::source_location location) {
    return new(this, location) PyObjectEx_Type1 (type, args, keywords, enclosed, checkArenaOwnership);
}

PyObjectEx_Type1* PythonArena::ObjectEx_Type1 (PyToken* type, PyTuple* args, PyList* keywords, bool enclosed, bool checkArenaOwnership, std::source_location location) {
    return new(this, location) PyObjectEx_Type1 (type, args, keywords, enclosed, checkArenaOwnership);
}

PyObjectEx_Type1* PythonArena::ObjectEx_Type1 (PyDataType* header, PyList* list, PyDict* dict, bool checkArenaOwnership, std::source_location location) {
    return new(this, location) PyObjectEx_Type1 (header, list, dict, checkArenaOwnership);
}

PyObjectEx_Type2* PythonArena::ObjectEx_Type2 (PyTuple* args, PyDict* keywords, bool enclosed, bool checkArenaOwnership, std::source_location location) {
    return new(this, location) PyObjectEx_Type2 (args, keywords, enclosed, checkArenaOwnership);
}

PyObjectEx_Type2* PythonArena::ObjectEx_Type2 (PyToken* args, PyDict* keywords, bool enclosed, bool checkArenaOwnership, std::source_location location) {
    return new(this, location) PyObjectEx_Type2 (args, keywords, enclosed, checkArenaOwnership);
}

PyObjectEx_Type2* PythonArena::ObjectEx_Type2 (PyDataType* header, PyList* list, PyDict* dict, bool checkArenaOwnership, std::source_location location) {
    return new(this, location) PyObjectEx_Type2 (header, list, dict, checkArenaOwnership);
}

PyPackedRow* PythonArena::PackedRow(DBRowDescriptor* header, bool checkArenaOwnership, std::source_location location) {
    return new(this, location) PyPackedRow (header, checkArenaOwnership);
}

PyPackedRow* PythonArena::PackedRow(DBRowDescriptor* header, std::initializer_list <PyDataType*> values, bool checkArenaOwnership, std::source_location location) {
    return new(this, location) PyPackedRow (header, values, checkArenaOwnership);
}

PyPackedRow* PythonArena::PackedRow(DBRowDescriptor* header, PyList* fields, bool checkArenaOwnership, std::source_location location) {
    return new(this, location) PyPackedRow (header, fields, checkArenaOwnership);
}

HeapPythonArena* HeapPythonArena::instance = new HeapPythonArena();