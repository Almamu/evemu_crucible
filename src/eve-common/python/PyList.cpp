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
    PyDataType (Type) {
}

template <class... T>
PyList::PyList (T&&... args) :
    PyDataType (Type),
    mItems ({&args...}) {
}

PyList::PyList (PyList::storage_type data) :
    PyDataType (Type),
    mItems (data) {
}

PyList::PyList (size_t size) :
    PyDataType (Type),
    mItems (size, nullptr) {
}

PyList::~PyList () {
    for (auto cur : *this)
        PySafeDecRef (cur);
}

void PyList::add (PyDataType* value) {
    this->mItems.push_back (value);
}

void PyList::set(int i, PyDataType* value) {
    this->mItems [i] = value;
}

PyList::const_iterator PyList::begin () const {
    return this->mItems.begin ();
}

PyList::const_iterator PyList::end () const {
    return this->mItems.end ();
}

size_t PyList::size () const {
    return this->mItems.size ();
}

bool PyList::empty () const {
    return this->mItems.empty ();
}

template<class T>
VectorWrapper<T> PyList::wrap () const {
    return VectorWrapper <T> (this->mItems);
}

template<class T>
T* PyList::at (int i) const {
    return this->mItems.at (i)->As <T> ();
}

bool PyList::equals (const PyDataType* right) const {
    if (PyDataType::equals (right) == false)
        return false;

    PyList* other = right->As <PyList> ();

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

PyDataType::PyType PyList::Type = PyTypeList;