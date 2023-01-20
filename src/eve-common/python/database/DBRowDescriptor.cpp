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
    PyObjectEx_Type1 (this->arena()->Token ("blue.DBRowDescriptor"), _CreateArgs()) {
}

DBRowDescriptor::DBRowDescriptor (PyList* keywords, bool checkArenaOwnership) :
    PyObjectEx_Type1 (this->arena()->Token ("blue.DBRowDescriptor"), _CreateArgs(), keywords) {
    if (checkArenaOwnership) {
        this->_checkArenaOwnership (keywords);
    }
}

DBRowDescriptor::DBRowDescriptor (const DBQueryResult& res) :
    PyObjectEx_Type1 (this->arena()->Token ("blue.DBRowDescriptor"), _CreateArgs()) {
    for (uint32_t i = 0; i < res.ColumnCount (); i ++)
        this->add (res.ColumnName (i), res.ColumnType (i));
}

DBRowDescriptor::DBRowDescriptor (const DBResultRow& row) :
    PyObjectEx_Type1 (this->arena()->Token ("blue.DBRowDescriptor"), _CreateArgs()) {
    for (uint32_t i = 0; i < row.ColumnCount (); i ++)
        this->add (row.ColumnName (i), row.ColumnType (i));
}

DBRowDescriptor::DBRowDescriptor (PyTuple* args, bool checkArenaOwnership) :
    PyObjectEx_Type1 (this->arena()->Token ("blue.DBRowDescriptor"), args) {
    if (checkArenaOwnership) {
        this->_checkArenaOwnership (args);
    }
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
    PyString* tmp = this->arena()->String (name);
    int32 hash = tmp->hash();

    PyDecRef(tmp);

    for (uint32_t i = 0; i < this->count (); i ++)
        if (hash == this->name (i)->hash ())
            return i;

    return this->count ();
}

bool DBRowDescriptor::verify (uint32_t index, PyDataType* value) {
    return DBTYPE_IsCompatible (this->type (index), value);
}

void DBRowDescriptor::add (const char* name, DBTYPE type) {
    this->_GetColumnList ()->mItems.push_back (
        this->arena()->Tuple({
            this->arena()->String (name), this->arena()->Int (type)
        })
    );
}

DBRowDescriptor* DBRowDescriptor::clone (PythonArena* arena) const {
    return new(arena) DBRowDescriptor (this->args()->clone(arena), true);
}

PyTuple* DBRowDescriptor::_GetColumnList() const
{
    return this->args()->at <PyTuple> (0);
}

PyTuple* DBRowDescriptor::_GetColumn( size_t index ) const
{
    return _GetColumnList()->at <PyTuple> (index);
}

PyTuple* DBRowDescriptor::_CreateArgs()
{
    return this->arena()->Tuple({
        this->arena()->Tuple()
    });
}