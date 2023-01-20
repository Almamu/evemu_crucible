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

#include "PyTuple.h"

template <class... T>
PyTuple::PyTuple (T&&... args) :
    PyDataType (Type),
    mItems ({&args...}) {
}

PyTuple::PyTuple (PyTuple::storage_type data) :
    PyDataType (Type),
    mItems (data) {
}

PyTuple::PyTuple (size_t size) :
    PyDataType (Type),
    mItems (size, nullptr) {
}

PyTuple::~PyTuple () {
    for (auto cur : *this)
        PySafeDecRef (cur);
}

PyTuple::const_iterator PyTuple::begin () const {
    return this->mItems.begin ();
}

PyTuple::const_iterator PyTuple::end () const {
    return this->mItems.end ();
}

size_t PyTuple::size () const {
    return this->mItems.size ();
}

bool PyTuple::empty () const {
    return this->mItems.empty ();
}

template<class T>
VectorWrapper<T> PyTuple::wrap () const {
    return VectorWrapper <T> (this->mItems);
}

template<class T>
T* PyTuple::at(int i) const {
    return this->mItems.at (i)->As <T> ();
}

bool PyTuple::equals (const PyDataType* right) const {
    return PyDataType::equals (right) && this->hash () == right->hash ();
}

bool PyTuple::visit (PyVisitor& v) const {
    return v.VisitTuple (this);
}

int32 PyTuple::hash () const {
    long x=0, y=0;
    auto len = (int32)this->mItems.size();
    long index = 0;
    long mult = 1000003L;
    x = 0x345678L;
    while (--len >= 0) {
        y = this->mItems[index++]->hash();
        if (y == -1)
            return -1;
        x = (x ^ y) * mult;
        /* the cast might truncate len; that doesn't change hash stability */
        mult += (long)(82520L + len + len);
    }
    x += 97531L;
    if (x == -1)
        x = -2;
    return (int32)x;
}

PyDataType::PyType PyTuple::Type = PyTypeTuple;