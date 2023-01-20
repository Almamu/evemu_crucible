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
#include "memory/PythonArena.h"

PyObject::PyObject (const std::string& type, PyDataType* args, bool checkArenaOwnership) :
    PyDataType (Type),
    mTypename (this->arena()->String(type)),
    mArguments (args) {
    if (checkArenaOwnership) {
        this->_checkArenaOwnership (args);
    }
}

PyObject::PyObject (PyString* type, PyDataType* args, bool checkArenaOwnership) :
    PyDataType (Type),
    mTypename (type),
    mArguments (args) {
    if (checkArenaOwnership) {
        this->_checkArenaOwnership (type);
        this->_checkArenaOwnership (args);
    }
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

    PyObject* other = right->as<PyObject>();

    return
        this->type()->equals (other->type()) &&
        this->arguments()->equals (other->arguments());
}

bool PyObject::visit (PyVisitor& v) const {
    return v.VisitObject (this);
}

PyObject* PyObject::clone () const {
    return this->clone (HeapPythonArena::instance);
}

PyObject* PyObject::clone (PythonArena* arena) const {
    return arena->Object(this->type()->clone(arena), this->arguments()->clone(arena));
}

PyDataType::PyType PyObject::Type = PyTypeObject;