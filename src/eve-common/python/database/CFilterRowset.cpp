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

#include "CFilterRowset.h"
#include "python/PyToken.h"
#include "python/PyString.h"
#include "python/PyPackedRow.h"

CFilterRowset::CFilterRowset (DBRowDescriptor* header, const char* indexColumn) :
    PyObjectEx_Type2 (_CreateArgs (), _CreateKeywords (header, indexColumn)),
    mHeader (header) {
}

CFilterRowset::~CFilterRowset () {
}

/**
 * @return Adds an empty row that can be filled manually
 */
CRowset* CFilterRowset::insert (PyDataType* key) {
    CRowset* set = new CRowset (this->mHeader);

    this->dict ().set (key, set);

    return set;
}

CRowset* CFilterRowset::get (PyDataType* key) const {
    return (CRowset*) this->dict ().get (key);
}

size_t CFilterRowset::keyCount () const {
    return this->dict ().size ();
}

PyTuple* CFilterRowset::_CreateArgs()
{
    return new PyTuple {
        new PyToken ("dbutil.CFilterRowset")
    };
}

PyDict* CFilterRowset::_CreateKeywords(DBRowDescriptor* header, const char* indexColumn)
{
    if (header == nullptr)
        throw std::runtime_error ("Invalid header for CFilterRowset");

    return new PyDict {
        {new PyString ("header"), header},
        {new PyString ("columnName"), new PyString (indexColumn)}
    };
}
