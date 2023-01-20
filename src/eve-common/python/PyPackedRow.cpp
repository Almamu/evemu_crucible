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
    Author:     Bloody.Rabbit
    Updates:    Allan
    Rewrite:    Almamu
*/

#include "PyPackedRow.h"

PyPackedRow::PyPackedRow(DBRowDescriptor* header) :
    PyDataType (Type),
    mHeader (header),
    mFields (header->count ()) {
}

PyPackedRow::PyPackedRow (DBRowDescriptor* header, std::initializer_list <PyDataType*> values) :
    PyDataType (Type),
    mHeader (header),
    mFields (values) {
    if (mFields.size () != mHeader->count ())
        throw std::runtime_error ("Column count doesn't match for PyPackedRow");
}

DBRowDescriptor* PyPackedRow::header () const {
    return this->mHeader;
}

const PyList& PyPackedRow::fields () const {
    return this->mFields;
}

void PyPackedRow::set (uint32_t index, PyDataType* value) {
    if (!this->mHeader->verify (index, value)) {
        PyDecRef (value);
        return;
    }

    this->mFields.set (index, value);
}

void PyPackedRow::set (const char* column, PyDataType* value) {
    return this->set (this->mHeader->find (column), value);
}

bool PyPackedRow::equals(const PyDataType* right) const {
    if (PyDataType::equals (right) == false)
        return false;

    PyPackedRow* other = right->As <PyPackedRow> ();

    return other->header ()->equals (this->header ()) && other->fields ().equals (&this->fields ());
}

bool PyPackedRow::visit (PyVisitor& v) const {
    return v.VisitPackedRow (this);
}

template <class T>
T* PyPackedRow::get (uint32_t index) const {
    return this->fields ().at <T> (index);
}

template <class T>
T* PyPackedRow::get (const char* column) const {
    return this->get <T> (this->header ()->find (column));
}

PyDataType::PyType PyPackedRow::Type = PyTypePackedRow;