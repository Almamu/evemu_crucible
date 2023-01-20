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

#include "PyString.h"

#include <utility>

PyString::PyString (std::string str, bool isUTF) :
    PyDataType (Type),
    mHashCache (-1),
    mValue (std::move(str), this->allocator<std::string>()),
    mIsUTF (isUTF) {
}

PyString::PyString (const char* str, size_t len, bool isUTF) :
    PyDataType (Type),
    mHashCache (-1),
    mValue (str, len, this->allocator<std::string>()),
    mIsUTF (isUTF) {
}

const std::string& PyString::content () const {
    return this->mValue;
}

bool PyString::utf8 () const {
    return this->mIsUTF;
}

size_t PyString::size() const {
    return this->utf8 () ? utf8::distance (this->content ().begin (), this->content ().end ()) : this->content ().size ();
}

bool PyString::equals (const PyDataType* right) const {
    return PyDataType::equals (right) && this->content () == right->as<PyString>()->content ();
}

int32 PyString::hash() const {
    if (mHashCache != -1)
        return mHashCache;

    if (mValue.length () > 0) {
        mHashCache = std::hash <std::string> {} (mValue);
    } else {
        mHashCache = 0;
    }

    return mHashCache;
}

bool PyString::visit (PyVisitor& v) const {
    return v.VisitString (this);
}

PyString* PyString::clone () const {
    return this->clone (HeapPythonArena::instance);
}

PyString* PyString::clone (PythonArena* arena) const {
    return arena->String (this->content ());
}

PyString::operator std::string() const {
    return this->content();
}

PyDataType::PyType PyString::Type = PyTypeString;