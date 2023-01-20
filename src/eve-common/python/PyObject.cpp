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

#include "PyObject.h"

PyObject::PyObject (PyString* type, PyDataType* args) :
    PyDataType (Type),
    mTypename (type),
    mArguments (args) {
}

PyString* PyObject::type() const {
    return this->mTypename;
}

PyDataType* PyObject::arguments () const {
    return this->mArguments;
}

bool PyObject::equals (const PyDataType* right) const {
    if (PyDataType::equals (right) == false)
        return false;

    PyObject* other = right->As <PyObject>();

    return
        this->type()->equals (other->type()) &&
        this->arguments()->equals (other->arguments());
}

bool PyObject::visit (PyVisitor& v) const {
    return v.VisitObject (this);
}

PyDataType::PyType PyObject::Type = PyTypeObject;