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

#include "PyFloat.h"
#include "PyInt.h"

PyFloat::PyFloat (double value) : PyDataType (Type), mValue (value) {}
PyFloat::~PyFloat () {}

double PyFloat::value () const {
    return this->mValue;
}

bool PyFloat::visit (PyVisitor& v) const {
    return v.VisitReal (this);
}

int32 PyFloat::hash () const {
#define Py_IS_INFINITY( X ) \
    ( !finite( X ) && !isnan( X ) )

    double v = mValue;
    double intpart=0.0, fractpart=0.0;
    int expo=0;
    long hipart=0, x=0;        /* x is the final hash value */
    /* This is designed so that Python numbers of different types
    * that compare equal hash to the same value; otherwise comparisons
    * of mapping keys will turn out weird.
    */

    fractpart = modf(v, &intpart);
    if (fractpart == 0.0) {
        /* This must return the same hash as an equal int or long. */
        if ((intpart > INT32_MAX) or (-intpart > INT32_MAX)) {
            /* Convert to long and use its hash. */
            if (Py_IS_INFINITY(intpart))
                /* can't convert to long int -- arbitrary */
                v = v < 0 ? -271828.0 : 314159.0;
            //plong = PyLong_FromDouble(v);

            PyDataType *plong = new PyInt( (int64)v ); // this is a hack
            if (plong == nullptr)
                return -1;
            x = plong->hash();
            PyDecRef( plong );
            return x;
        }
        /* Fits in a C long == a Python int, so is its own hash. */
        x = (long)intpart;
        if (x == -1)
            x = -2;
        return x;
    }
    /* The fractional part is non-zero, so we don't have to worry about
    * making this match the hash of some other type.
    * Use frexp to get at the bits in the double.
    * Since the VAX D double format has 56 mantissa bits, which is the
    * most of any double format in use, each of these parts may have as
    * many as (but no more than) 56 significant bits.
    * So, assuming sizeof(long) >= 4, each part can be broken into two
    * longs; frexp and multiplication are used to do that.
    * Also, since the Cray double format has 15 exponent bits, which is
    * the most of any double format in use, shifting the exponent field
    * left by 15 won't overflow a long (again assuming sizeof(long) >= 4).
    */
    v = frexp(v, &expo);
    v *= 2147483648.0;    /* 2**31 */
    hipart = (long)v;    /* take the top 32 bits */
    v = (v - (double)hipart) * 2147483648.0; /* get the next 32 bits */
    x = hipart + (long)v + (expo << 15);
    if (x == -1)
        x = -2;
    return x;

#undef Py_IS_INFINITY
}

bool PyFloat::equals (const PyDataType* right) const {
    return PyDataType::equals (right) && this->mValue == right->As <PyFloat> ()->mValue;
}

PyFloat::operator double () const {
    return this->mValue;
}

PyFloat::operator float () const {
    return (float) this->mValue;
}
PyDataType::PyType PyFloat::Type = PyTypeFloat;