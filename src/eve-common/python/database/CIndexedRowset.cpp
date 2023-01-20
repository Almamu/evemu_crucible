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

#include "CIndexedRowset.h"
#include "python/PyToken.h"
#include "python/PyString.h"
#include "python/PyPackedRow.h"

CIndexedRowset::CIndexedRowset (DBRowDescriptor* header, const char* indexColumn) :
    PyObjectEx_Type2 (_CreateArgs (), _CreateKeywords (header, indexColumn)),
    mHeader (header) {
}

CIndexedRowset::~CIndexedRowset () {
}

/**
 * @return Adds an empty row that can be filled manually
 */
PyPackedRow* CIndexedRowset::insert (PyDataType* key) {
    PyPackedRow* row = new PyPackedRow (this->mHeader);

    this->dict ().set (key, row);

    return row;
}

/**
 * @return Adds a new row with the given values
 */
PyPackedRow* CIndexedRowset::insert (PyDataType* key, std::initializer_list <PyDataType*> values) {
    PyPackedRow* row = new PyPackedRow (this->mHeader, values);

    this->dict ().set (key, row);

    return row;
}

PyPackedRow* CIndexedRowset::get (PyDataType* key) const {
    return this->dict ().get <PyPackedRow> (key);
}

size_t CIndexedRowset::count () const {
    return this->dict ().size ();
}

PyTuple* CIndexedRowset::_CreateArgs()
{
    return new PyTuple {
        new PyToken ("dbutil.CIndexedRowset")
    };
}

PyDict* CIndexedRowset::_CreateKeywords(DBRowDescriptor* header, const char* indexColumn)
{
    if (header == nullptr)
        throw std::runtime_error ("Invalid header for CIndexedRowset");

    return new PyDict {
        {new PyString ("header"), header},
        {new PyString ("columnName"), new PyString (indexColumn)}
    };
}
