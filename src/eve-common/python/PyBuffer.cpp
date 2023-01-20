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

#include "eve-common.h"
#include "PyBuffer.h"

PyBuffer::PyBuffer (const uint8& fill, size_t len) :
    PyDataType (Type),
    mValue (new Buffer (len, fill)),
    mHashCache (-1) {
}

template <typename Iter>
PyBuffer::PyBuffer (Iter first, Iter second) :
    PyDataType (Type),
    mValue (new Buffer (first, second)),
    mHashCache (-1) {
}

PyBuffer::PyBuffer (const Buffer& buffer) :
    PyDataType (Type),
    mValue (new Buffer (buffer)),
    mHashCache (-1) {
}

PyBuffer::PyBuffer (Buffer** buffer) :
    PyDataType (Type),
    mValue (*buffer),
    mHashCache (-1) {
    *buffer = nullptr; // take ownership of the buffer
}

PyBuffer::~PyBuffer () {
    delete this->mValue;
}

const Buffer& PyBuffer::content () const {
    return *this->mValue;
}

size_t PyBuffer::size () const {
    return this->mValue->size ();
}

int32 PyBuffer::hash () const  {
    if (mHashCache != -1 )
        return mHashCache;

    int32 len=0, x=0;
    unsigned char* p(nullptr);

    /* XXX potential bugs here, a readonly buffer does not imply that the
     * underlying memory is immutable.  b_readonly is a necessary but not
     * sufficient condition for a buffer to be hashable.  Perhaps it would
     * be better to only allow hashing if the underlying object is known to
     * be immutable (e.g. PyString_Check() is true).  Another idea would
     * be to call tp_hash on the underlying object and see if it raises
     * an error. */
    //if ( !self->b_readonly )
    //{
    //   PyErr_SetString(PyExc_TypeError,
    //      "writable buffers are not hashable");
    // return -1;
    //}

    //if (!get_buf(self, &ptr, &size, ANY_BUFFER))
    //    return -1;
    p = (unsigned char *) &content()[0];
    len = (int32)content().size();
    x = *p << 7;
    while( --len >= 0 )
        x = (1000003*x) ^ *p++;
    x ^= content().size();
    if (x == -1)
        x = -2;
    mHashCache = x;
    return x;
}

bool PyBuffer::visit (PyVisitor& v) const {
    return v.VisitBuffer (this);
}

bool PyBuffer::equals (const PyDataType* right) const {
    return PyDataType::equals (right) && this->hash () == right->hash ();
}

PyDataType::PyType PyBuffer::Type = PyTypeBuffer;