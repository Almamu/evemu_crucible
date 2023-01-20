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

#include "PyInt.h"

PyInt::PyInt (int64 value) : PyDataType (Type), mValue (value) {}
PyInt::~PyInt() {}

int64 PyInt::value () const {
    return this->mValue;
}

bool PyInt::visit (PyVisitor& v) const {
    return v.VisitInteger (this);
}

int32 PyInt::hash () const {
#define PyLong_SHIFT    15
#define PyLong_BASE     (1 << PyLong_SHIFT)
#define PyLong_MASK     ((int)(PyLong_BASE - 1))

#define LONG_BIT_PyLong_SHIFT    (8*sizeof(long) - PyLong_SHIFT)

    long x=0;
    int i=8, sign=1;

    /* This is designed so that Python ints and longs with the
    same value hash to the same value, otherwise comparisons
    of mapping keys will turn out weird */
    if (i < 0 ) {
        sign = -1;
        i = -(i);
    }
    /* The following loop produces a C long x such that (unsigned long)x
    is congruent to the absolute value of v modulo ULONG_MAX.  The
    resulting x is nonzero if and only if v is. */
    while( --i >= 0 ) {
        /* Force a native long #-bits (32 or 64) circular shift */
        x = ((x << PyLong_SHIFT) & ~PyLong_MASK) | ((x >> LONG_BIT_PyLong_SHIFT) & PyLong_MASK);
        x += ((uint8*)&mValue)[i];// v->ob_digit[i];
        /* If the addition above overflowed (thinking of x as
        unsigned), we compensate by incrementing.  This preserves
        the value modulo ULONG_MAX. */
        if ((unsigned long)x < ((uint8*)&mValue)[i] )//v->ob_digit[i])
            x++;
    }
    x = x * sign;
    if (x == -1 )
        x = -2;
    return x;

#undef PyLong_SHIFT
#undef PyLong_BASE
#undef PyLong_MASK

#undef LONG_BIT_PyLong_SHIFT
}

bool PyInt::equals (const PyDataType* right) const {
    return PyDataType::equals(right) && this->value () == right->As <PyInt> ()->value ();
}

PyInt::operator int64() const {
    return this->mValue;
}
/*
PyInt::operator uint64() const {
    return (uint64) this->mValue;
}
*/
PyInt::operator int32() const {
    return (int32) (this->mValue & 0xFFFFFFFF);
}

PyInt::operator uint32() const {
    return (uint32) (this->mValue & 0xFFFFFFFF);
}

PyInt::operator int16() const {
    return (int16) (this->mValue & 0xFFFF);
}

PyInt::operator uint16() const {
    return (uint16) (this->mValue & 0xFFFF);
}

PyInt::operator int8() const {
    return (int8) (this->mValue & 0xFF);
}

PyInt::operator uint8() const {
    return (uint8) (this->mValue & 0xFF);
}

PyDataType::PyType PyInt::Type = PyTypeInt;