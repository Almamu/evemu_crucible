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
#include "PyFileDumpVisitor.h"
#include "PyLogDumpVisitor.h"

const char* const s_TypeString[] =
{
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
    "PackedRow",
    "SubStruct",
    "SubStream"
};

PyDataType::PyDataType (PyType type) :
    RefObject (1),
    mType (type) {
}

PyDataType::~PyDataType () {
    // these objects might be deleted as they can be allocated on the heap
    if (this->GetCount () > 1)
        throw std::runtime_error ("Destroying a PyDataType without being properly dereferenced");
}

const char* PyDataType::TypeString () const {
    if (mType >= PyTypeMin && mType <= PyTypeMax)
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

std::string PyDataType::string () const {
    if (this->Is <PyString> ())
        return this->As <PyString> ()->content ();

    return "";
}

int64_t PyDataType::i64 () const {
    if (this->Is <PyInt> ())
        return this->As <PyInt> ()->value ();
    if (this->Is <PyFloat> ())
        return this->As <PyFloat> ()->value ();
    if (this->Is <PyBool> ())
        return this->As <PyBool> ()->value ();

    return 0;
}

uint32_t PyDataType::u32 () const {
    if (this->Is <PyInt> ())
        return this->As <PyInt> ()->value ();
    if (this->Is <PyFloat> ())
        return this->As <PyFloat> ()->value ();
    if (this->Is <PyBool> ())
        return this->As <PyBool> ()->value ();

    return 0;
}

double PyDataType::decimal () const {
    if (this->Is <PyInt> ())
        return this->As <PyInt> ()->value ();
    if (this->Is <PyFloat> ())
        return this->As <PyFloat> ()->value ();
    if (this->Is <PyBool> ())
        return this->As <PyBool> ()->value ();

    return 0;
}