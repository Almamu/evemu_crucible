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

CFilterRowset::CFilterRowset (DBRowDescriptor* header, const char* indexColumn, bool checkArenaOwnership) :
    PyObjectEx_Type2 (_CreateArgs (), _CreateKeywords (header, this->arena()->String (indexColumn))),
    mHeader (header) {
    if (checkArenaOwnership) {
        this->_checkArenaOwnership (header);
    }
}

CFilterRowset::~CFilterRowset () {
}

/**
 * @return Adds an empty row that can be filled manually
 */
CRowset* CFilterRowset::insert (PyDataType* key, bool checkArenaOwnership) {
    CRowset* set = new(this->arena()) CRowset (mHeader, checkArenaOwnership);

    this->dict ()->set (key, set, checkArenaOwnership);

    return set;
}

CRowset* CFilterRowset::get (PyDataType* key) const {
    return (CRowset*) this->dict ()->get (key);
}

size_t CFilterRowset::keyCount () const {
    return this->dict ()->size ();
}

void CFilterRowset::setAllowDuplicateCompoundKeys (bool allow) {
    // TODO: IMPLEMENT
}

void CFilterRowset::setIndexName (const char* indexName) {
    // TODO: IMPLEMENT
}

void CFilterRowset::clearIndexName () {
    // TODO: IMPLEMENT
}

PyTuple* CFilterRowset::_CreateArgs()
{
    return this->arena()->Tuple ({
        this->arena()->Token ("dbutil.CFilterRowset")
    });
}

PyDict* CFilterRowset::_CreateKeywords(DBRowDescriptor* header, PyDataType* indexColumn)
{
    if (header == nullptr)
        throw std::runtime_error ("Invalid header for CFilterRowset");

    return this->arena()->Dict ({
        {"header", header},
        {"columnName", indexColumn}
    });
}
