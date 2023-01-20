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

#include "../PyString.h"
#include "../PyToken.h"
#include "../PyInt.h"
#include "DBRowDescriptor.h"
#include "utils/EVEUtils.h"

DBRowDescriptor::DBRowDescriptor () :
    PyObjectEx_Type1 (new PyToken ("blue.DBRowDescriptor"), _CreateArgs()) {
}

DBRowDescriptor::DBRowDescriptor (PyList* keywords) :
    PyObjectEx_Type1 (new PyToken ("blue.DBRowDescriptor"), _CreateArgs(), keywords) {
}

DBRowDescriptor::DBRowDescriptor (const DBQueryResult& res) :
    PyObjectEx_Type1 (new PyToken ("blue.DBRowDescriptor"), _CreateArgs()) {
    for (uint32_t i = 0; i < res.ColumnCount (); i ++)
        this->add (res.ColumnName (i), res.ColumnType (i));
}

DBRowDescriptor::DBRowDescriptor (const DBResultRow& row) :
    PyObjectEx_Type1 (new PyToken ("blue.DBRowDescriptor"), _CreateArgs()) {
    for (uint32_t i = 0; i < row.ColumnCount (); i ++)
        this->add (row.ColumnName (i), row.ColumnType (i));
}

uint32_t DBRowDescriptor::count () const {
    return _GetColumnList ()->size ();
}

const PyString* DBRowDescriptor::name (uint32_t index) const {
    return this->_GetColumn (index)->at <PyString> (0);
}

DBTYPE DBRowDescriptor::type (uint32_t index) const {
    return (DBTYPE) this->_GetColumn (index)->at <PyInt> (1)->value ();
}

uint32_t DBRowDescriptor::find (const char* name) const {
    PyString tmp (name);

    for (uint32_t i = 0; i < this->count (); i ++)
        if (tmp.hash () == this->name (i)->hash ())
            return i;

    return this->count ();
}

bool DBRowDescriptor::verify (uint32_t index, PyDataType* value) {
    return DBTYPE_IsCompatible (this->type (index), value);
}

void DBRowDescriptor::add (const char* name, DBTYPE type) {
    this->_GetColumnList ()->mItems.push_back (
        new PyTuple {
            new PyString (name), new PyInt (type)
        }
    );
}