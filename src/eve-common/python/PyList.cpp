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

#include "PyList.h"

PyList::PyList () :
    PyDataType (Type), mItems (this->allocator<value_type>()) {
}

PyList::PyList (std::initializer_list<value_type> init, bool checkArenaOwnership) :
    PyDataType (Type),
    mItems (init, this->allocator<value_type>()) {
    if (checkArenaOwnership) {
        for (auto it : init)
            this->_checkArenaOwnership (it);
    }
}

PyList::PyList (PyList::storage_type data, bool checkArenaOwnership) :
    PyDataType (Type),
    mItems (data, this->allocator<value_type>()) {
    if (checkArenaOwnership) {
        for (auto it : data)
            this->_checkArenaOwnership (it);
    }
}

PyList::PyList (size_t size) :
    PyDataType (Type),
    mItems (size, nullptr, this->allocator<value_type>()) {
}

PyList::~PyList () {
    for (auto cur : *this)
        PySafeDecRef (cur);
}

void PyList::add (PyDataType* value, bool checkArenaOwnership) {
    if (checkArenaOwnership) {
        this->_checkArenaOwnership (value);
    }

    mItems.push_back (value);
}

void PyList::set(int i, PyDataType* value, bool checkArenaOwnership) {
    if (checkArenaOwnership) {
        this->_checkArenaOwnership (value);
    }

    mItems [i] = value;
}

void PyList::clear() {
    mItems.clear();
}

PyList::const_iterator PyList::begin () const {
    return mItems.begin ();
}

PyList::const_iterator PyList::end () const {
    return mItems.end ();
}

size_t PyList::size () const {
    return mItems.size ();
}

bool PyList::empty () const {
    return mItems.empty ();
}

PyDataType* PyList::front() const {
    return mItems.front();
}

PyDataType* PyList::back() const {
    return mItems.back();
}

bool PyList::equals (const PyDataType* right) const {
    if (PyDataType::equals (right) == false)
        return false;

    PyList* other = right->as<PyList>();

    if (this->size () != other->size ())
        return false;

    PyList::const_iterator cur, end, otherCur, otherEnd;

    cur = this->begin ();
    end = this->end ();
    otherCur = other->begin ();
    otherEnd = other->end ();

    for (; cur != end && otherCur != otherEnd; cur ++, otherCur ++)
        if ((*cur)->equals (*otherCur) == false)
            return false;

    return true;
}

bool PyList::visit (PyVisitor& v) const {
    return v.VisitList (this);
}

PyList* PyList::clone () const {
    return this->clone (HeapPythonArena::instance);
}

PyList* PyList::clone (PythonArena* arena) const {
    storage_type copy;

    for (auto it : *this)
      copy.push_back (it->clone (arena));

    return arena->List (copy);
}

PyDataType::PyType PyList::Type = PyTypeList;