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
    PyDataType (Type) {
}

PyDict::PyDict (std::initializer_list <PyDict::storage_type::value_type> init) :
    PyDataType (Type),
    mItems (init) {
}

PyDict::~PyDict () {
    for (auto it : *this) {
        PySafeDecRef (it.first);
        PySafeDecRef (it.second);
    }
}

PyDict::const_iterator PyDict::begin () const {
    return this->mItems.begin ();
}

PyDict::const_iterator PyDict::end () const {
    return this->mItems.end ();
}

size_t PyDict::size () const {
    return this->mItems.size ();
}

template<class Key, class Value>
UnorderedMapWrapper<Key, Value> PyDict::wrap () const {
    return UnorderedMapWrapper <Key, Value> (this->mItems);
}

bool PyDict::empty () const {
    return this->mItems.empty ();
}

bool PyDict::equals (const PyDataType* right) const {
    if (PyDataType::equals (right) == false)
        return false;

    PyDict* other = right->As <PyDict> ();

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

void PyDict::set (PyDataType* key, PyDataType* value) {
    auto it = this->mItems.find (key);

    if (it == this->mItems.end ()) {
        this->mItems.insert (std::make_pair (key, value));
    } else {
        PySafeDecRef (key);
        // free reference to value
        PySafeDecRef (it->second);
        it->second = value;
    }
}

template <class T>
T* PyDict::get (PyDataType* key) const {
    auto it = this->mItems.find (key);

    if (it == this->mItems.end ())
        throw std::out_of_range ("Cannot find the given key in the dictionary");

    PySafeDecRef (key);

    return it->second->As <T> ();
}

PyDataType::PyType PyDict::Type = PyTypeDict;