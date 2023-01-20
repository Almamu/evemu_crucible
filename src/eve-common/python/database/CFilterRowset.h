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

#pragma once

#include "python/PyList.h"
#include "DBRowDescriptor.h"
#include "CRowset.h"

/**
 * @brief Python object "dbutil.CRowset
 *
 * This object represents a database result,
 * contains a DBRowDescriptor as header and a list
 * of PyPackedRows.
 */
class CFilterRowset : public PyObjectEx_Type2 {
public:
    /**
     * @param[in] rowDesc DBRowDescriptor header to be used.
     */
    CFilterRowset (DBRowDescriptor* header, const char* indexColumn, bool checkArenaOwnership = true);

    /**
     * @return Adds an empty rowset that can be filled manually
     */
    [[nodiscard]] CRowset* insert (PyDataType* key, bool checkArenaOwnership = true);

    /**
     * @param index The row to get
     * @return The requested row
     */
    [[nodiscard]] CRowset* get (PyDataType* index) const;

    /**
     * @return Row count
     */
    [[nodiscard]] size_t keyCount () const;

    /**
     * Indicates if the filter rowset allows duplicated compound keys
     *
     * @param allow
     */
    void setAllowDuplicateCompoundKeys (bool allow);

    /**
     * Sets an index for the filtered rowset
     *
     * @param indexName
     */
    void setIndexName (const char* indexName);

    /**
     * Clears the index name of this filtered rowset
     */
    void clearIndexName ();

private:
    virtual ~CFilterRowset ();

    DBRowDescriptor* mHeader;

    PyTuple* _CreateArgs();
    PyDict* _CreateKeywords(DBRowDescriptor* header, PyDataType* indexColumn);
};

