/*
    ------------------------------------------------------------------------------------
    LICENSE:
    ------------------------------------------------------------------------------------
    This file is part of EVEmu: EVE Online Server Emulator
    Copyright 2006 - 2021 The EVEmu Team
    For the latest information visit https://evemu.dev
    ------------------------------------------------------------------------------------
    This program is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the Free Software
    Foundation; either version 2 of the License, or (at your option) any later
    version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
    this program; if not, write to the Free Software Foundation, Inc., 59 Temple
    Place - Suite 330, Boston, MA 02111-1307, USA, or go to
    http://www.gnu.org/copyleft/lesser.txt.
    ------------------------------------------------------------------------------------
    Author:     Zhur
    Updates:    Allan
    Rewrite:    Almamu
*/

#include "PyDataType.h"
#include "PyString.h"
#include "PyInt.h"
#include "PyFloat.h"
#include "PyBool.h"
#include "visitors/PyFileDumpVisitor.h"
#include "visitors/PyLogDumpVisitor.h"

#include "memory/Memory.h"

const char* const s_TypeString[] =
{
    "Min",
    "None",
    "Int",
    "Float",
    "String",
    "Token",
    "Buffer",
    "Bool",
    "Tuple",
    "List",
    "Dict",
    "Object",
    "ObjectEx",
    "PackedRow",
    "SubStruct",
    "SubStream",
    "Invalid Type"
};

PyDataType::PyDataType (PyType type) :
    RefObject (1),
    mType (type) { }

PyDataType::~PyDataType () {}

const char* PyDataType::TypeString () const {
    if (mType > PyTypeMin && mType < PyTypeMax)
        return s_TypeString [mType];

    return "Error";
}

PyDataType::PyType PyDataType::GetType () const {
    return this->mType;
}

void PyDataType::dump (FILE* into, const char* prefix) const {
    PyFileDumpVisitor dumper (into, prefix);

    this->visit (dumper);
}

void PyDataType::dump (LogType type, const char *prefix) const {
    PyLogDumpVisitor dumper (type, type, prefix);

    this->visit (dumper);
}

int32 PyDataType::hash () const {
    throw std::runtime_error ("cannot hash this python type");
}

bool PyDataType::visit (PyVisitor& v) const {
    return false;
}

bool PyDataType::equals (const PyDataType* right) const {
    return this == right || (this != nullptr && right != nullptr && this->GetType () == right->GetType ());
}

PyDataType* PyDataType::clone() const {
    return this->clone (HeapPythonArena::instance);
}

PyDataType* PyDataType::clone(PythonArena* arena) const {
    throw std::runtime_error ("cannot clone this python type");
}

std::string PyDataType::string () const {
    if (this->is<PyString>())
        return this->as<PyString>()->content ();

    return "";
}

int64_t PyDataType::i64 () const {
    if (this->is<PyInt>())
        return this->as<PyInt>()->value ();
    if (this->is<PyFloat>())
        return this->as<PyFloat>()->value ();
    if (this->is<PyBool>())
        return this->as<PyBool>()->value ();

    return 0;
}

uint32_t PyDataType::u32 () const {
    if (this->is<PyInt>())
        return this->as<PyInt>()->value ();
    if (this->is<PyFloat>())
        return this->as<PyFloat>()->value ();
    if (this->is<PyBool>())
        return this->as<PyBool>()->value ();

    return 0;
}

int32_t PyDataType::i32 () const {
    if (this->is<PyInt>())
        return this->as<PyInt>()->value ();
    if (this->is<PyFloat>())
        return this->as<PyFloat>()->value ();
    if (this->is<PyBool>())
        return this->as<PyBool>()->value ();

    return 0;
}

double PyDataType::decimal () const {
    if (this->is<PyInt>())
        return this->as<PyInt>()->value ();
    if (this->is<PyFloat>())
        return this->as<PyFloat>()->value ();
    if (this->is<PyBool>())
        return this->as<PyBool>()->value ();

    return 0;
}


void PyDataType::_checkArenaOwnership (PyDataType* obj) {
    if (this->arena() == obj->arena()) {
        return;
    }

    throw std::runtime_error (
        "Trying to combine two PyDataTypes of different arenas is forbidden. "
        "You might be missing a clone() call. If you're 100% certain this is wanted "
        "(for example returning some static data to the client) "
        "set the parameter checkArenaOwnership to false"
    );
}