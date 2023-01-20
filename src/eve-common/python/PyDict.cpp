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

#include "PyDict.h"
#include "PyInt.h"
#include "PyFloat.h"
#include "PyString.h"

PyDict::PyDict () :
    PyDataType (Type),
    mItems (this->allocator<PyDict::value_type>()) {
}

PyDict::PyDict (std::initializer_list <PyDict::storage_type::value_type> init, bool checkArenaOwnership) :
    PyDataType (Type),
    mItems (init, this->allocator<PyDict::value_type>()) {
    if (checkArenaOwnership) {
        for (auto it : init) {
            this->_checkArenaOwnership (it.first);
            this->_checkArenaOwnership (it.second);
        }
    }
}

PyDict::PyDict (std::initializer_list <std::map<std::string,PyDataType*>::value_type> init, bool checkArenaOwnership) :
    PyDataType (Type),
    mItems (this->allocator<PyDict::value_type>()) {
    for (auto it : init)
        this->set(it.first, it.second, checkArenaOwnership);
}

PyDict::PyDict (PyDict::storage_type storage, bool checkArenaOwnership) :
    PyDataType (Type),
    mItems (storage, this->allocator<PyDict::value_type>()) {
    if (checkArenaOwnership) {
        for (auto it : storage) {
            this->_checkArenaOwnership (it.first);
            this->_checkArenaOwnership (it.second);
        }
    }
}

PyDict::~PyDict () {
    for (auto it : *this) {
        PySafeDecRef (it.first);
        PySafeDecRef (it.second);
    }
}

PyDict::const_iterator PyDict::begin () const {
    return mItems.begin ();
}

PyDict::const_iterator PyDict::end () const {
    return mItems.end ();
}

size_t PyDict::size () const {
    return mItems.size ();
}

bool PyDict::empty () const {
    return mItems.empty ();
}

bool PyDict::equals (const PyDataType* right) const {
    if (PyDataType::equals (right) == false)
        return false;

    PyDict* other = right->as<PyDict>();

    if (this->size () != other->size ())
        return false;

    PyDict::const_iterator cur, end, otherCur, otherEnd;

    cur = this->begin ();
    end = this->end ();
    otherCur = other->begin ();
    otherEnd = other->end ();

    for (; cur != end && otherCur != otherEnd; cur ++, otherCur ++)
        if (
            cur->first->equals (otherCur->first) == false ||
            cur->second->equals (otherCur->second) == false)
            return false;

    return true;
}

bool PyDict::visit (PyVisitor& v) const {
    return v.VisitDict (this);
}

PyDict* PyDict::clone () const {
    return this->clone(HeapPythonArena::instance);
}

PyDict* PyDict::clone (PythonArena* arena) const {
    storage_type copy;

    for (auto it : *this)
        copy.insert({it.first->clone (arena), it.second->clone (arena)});

    return arena->Dict(copy);
}

void PyDict::set (PyDataType* key, PyDataType* value, bool checkArenaOwnership) {
    if (checkArenaOwnership) {
        this->_checkArenaOwnership (key);
        this->_checkArenaOwnership (value);
    }

    auto it = mItems.find (key);

    if (it == mItems.end ()) {
        mItems.insert (std::make_pair (key, value));
    } else {
        PySafeDecRef (key);
        // free reference to value
        PySafeDecRef (it->second);
        it->second = value;
    }
}

void PyDict::set (const std::string& key, PyDataType* value, bool checkArenaOwnership) {
    if (checkArenaOwnership) {
        this->_checkArenaOwnership (value);
    }

    auto keyRep = this->arena()->String (key);
    auto it = mItems.find (keyRep);

    if (it == mItems.end ()) {
        mItems.insert (std::make_pair (keyRep, value));
    } else {
        PySafeDecRef (keyRep);
        // free reference to value
        PySafeDecRef (it->second);
        it->second = value;
    }
}

PyDataType::PyType PyDict::Type = PyTypeDict;