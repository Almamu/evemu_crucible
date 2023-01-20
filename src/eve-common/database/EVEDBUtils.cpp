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
*/

#include "eve-common.h"

#include "database/EVEDBUtils.h"
#include "python/database/CRowset.h"
#include "python/database/CIndexedRowset.h"
#include "python/database/CFilterRowset.h"


PyDataType* DBColumnToPyDataType(const DBResultRow& row, uint32 index, PythonArena* arena)
{
    /* check for valid column */
    if (row.IsNull(index))
        return arena->None ();      // this doesnt work right.  client still sees this as 0 instead of None

    switch(row.ColumnType(index)) {
        case DBTYPE_I1:
        case DBTYPE_UI1:
        case DBTYPE_I2:
        case DBTYPE_UI2:
        case DBTYPE_I4:
        case DBTYPE_UI4:
            return arena->Int(row.GetInt(index));

        case DBTYPE_I8:
        case DBTYPE_UI8:
            return arena->Int(row.GetInt64(index));

        case DBTYPE_R8:
        case DBTYPE_R4:
            return arena->Float(row.GetDouble(index));

        case DBTYPE_BOOL:
            return arena->Bool(row.GetBool(index));

        case DBTYPE_STR:
            return arena->String(row.GetText(index), row.ColumnLength(index));

        case DBTYPE_WSTR:
            return arena->String(row.GetText(index), row.ColumnLength(index), true);

        case DBTYPE_BYTES: {
            const uint8* data = (const uint8*)row.GetText(index);
            const uint32 len = row.ColumnLength(index);
            return arena->Buffer(data, data + len);
        }

        default: {
            sLog.Error("DBColumnToPyDataType", "invalid column type: %u", row.ColumnType(index));
            return arena->None ();
        }
    }
}

PyObject *DBResultToRowset(DBQueryResult &result, PythonArena* arena)
{
    uint32 cc = result.ColumnCount ();

    PyDict *args = arena->Dict();

    /* check if we have a empty query result and return a empty RowSet */
    if (cc == 0)
        return arena->Object(arena->String ("util.Rowset"), args);

    // list off the column names
    PyList* header = arena->List (cc);

    for (uint32 r = 0; r < cc; ++r)
        header->set (r, arena->String (result.ColumnName (r)));

    args->set (arena->String ("header"), header);

    //RowClass:
    args->set (arena->String ("RowClass"), arena->Token ("util.Row"));

    // lines
    PyList* rowlist = arena->List ();

    //add a line entry for each result row:
    DBResultRow row;

    while (result.GetRow (row)) {
        PyList* linedata = arena->List (cc);
        for (uint32 r = 0; r < cc; ++r)
            linedata->set (r, DBColumnToPyDataType (row, r, arena));

        rowlist->add (linedata);
    }

    args->set (arena->String ("lines"), rowlist);

    return arena->Object (arena->String ("util.Rowset"), args);
}

PyTuple *DBResultToTupleSet(DBQueryResult &result, PythonArena* arena) {
    uint32 cc = result.ColumnCount ();
    if (cc == 0)
        return arena->Tuple();

    //list off the column names:
    PyList* cols = arena->List (cc);
    for(uint32 r = 0; r < cc; ++r)
        cols->set(r, arena->String (result.ColumnName(r)));

    //add a line entry for each result row:
    DBResultRow row;
    PyList* reslist = arena->List ();
    while(result.GetRow(row)) {
        PyList *linedata = arena->List (cc);
        for (uint32 r = 0; r < cc; ++r)
            linedata->set (r, DBColumnToPyDataType(row, r, arena));
        reslist->add (linedata);
    }

    return arena->Tuple ({
        cols, reslist
    });
}

/**
 * Service function - for now used particularly as substitution of DBResultToTupleSet in case if we have multiple DB queries
 * and we need to get the values from them all.
 * This function extracts the values and pushes them in provided PyList. Just values, not including columns header.
 * @param result - DBQueryResult object
 * @param into - PyList to dump values into
 */
void populateResListWithValues(DBQueryResult &result, PyList *into, PythonArena* arena) {
    uint32 cc = result.ColumnCount();
    if (cc == 0) {
        return;
    }

    DBResultRow row;
    while(result.GetRow(row)) {
        PyList *linedata = arena->List (cc);
        for(auto index = 0; index < cc; index++)
            linedata->set (index, DBColumnToPyDataType (row, index, arena));
        into->add (linedata);
    }
}

PyObject *DBResultToIndexRowset(DBQueryResult &result, const char *key, PythonArena* arena) {
    uint32 cc(result.ColumnCount());
    uint32 key_index(0);

    for (key_index = 0; key_index < cc; ++key_index)
        if(strcmp(key, result.ColumnName(key_index)) == 0)
            break;

    if (key_index == cc) {
        sLog.Error("EVEDBUtils", "DBResultToIndexRowset | Failed to find key column '%s' in result for IndexRowset", key);
        return nullptr;
    }

    return DBResultToIndexRowset(result, key_index, arena);
}

PyObject *DBResultToIndexRowset(DBQueryResult &result, uint32 key_index, PythonArena* arena) {
    uint32 cc(result.ColumnCount());

    //start building the IndexRowset
    PyDict *args = arena->Dict();

    if (cc == 0 || cc < key_index)
        return arena->Object (arena->String ("util.IndexRowset"), args);

    //list off the column names:
    PyList *header = arena->List(cc);
    args->set (arena->String ("header"), header);
    for (uint32 i(0); i < cc; ++i)
        header->set(i, arena->String (result.ColumnName(i)));

    //RowClass:
    args->set (arena->String ("RowClass"), arena->Token("util.Row"));
    //idName:
    args->set(arena->String ("idName"), arena->String(result.ColumnName(key_index)));

    //items:
    PyDict *items = arena->Dict();
    //add a line entry for each result row:
    uint32 i(0);
    DBResultRow row;
    while(result.GetRow(row)) {
        PyDataType *key = DBColumnToPyDataType(row, key_index, arena);
        PyList *line = arena->List(cc);
        for (i = 0; i < cc; ++i)
            line->set (i, DBColumnToPyDataType(row, i, arena));

        items->set (key, line);
    }

    args->set(arena->String ("items"), items);
    return arena->Object(arena->String ("util.IndexRowset"), args);
}

PyObject *DBRowToKeyVal(DBResultRow &row, PythonArena* arena) {
    PyDict *args = arena->Dict();
    uint32 cc(row.ColumnCount());
    for (uint32 r(0); r < cc; ++r)
        args->set (arena->String (row.ColumnName(r)), DBColumnToPyDataType(row, r, arena));

    return arena->Object(arena->String ("util.KeyVal"), args);
}

PyObject *DBRowToRow(DBResultRow &row, const char *type, PythonArena* arena)
{
    PyDict *args = arena->Dict();

    //list off the column names:
    uint32 cc(row.ColumnCount());
    PyList *header = arena->List(cc);
    for (uint32 r(0); r < cc; ++r)
        header->set (r, arena->String (row.ColumnName(r)));

    args->set (arena->String ("header"), header);

    //lines:
    PyList *rowlist = arena->List(cc);
    //add a line entry for the row:
    for (uint32 r(0); r < cc; ++r)
        rowlist->set (r, DBColumnToPyDataType(row, r, arena));

    args->set (arena->String ("line"), rowlist);

    return arena->Object(arena->String (type), args);
}

PyTuple *DBResultToRowList(DBQueryResult &result, PythonArena* arena, const char *type) {
    uint32 cc(result.ColumnCount());
    if (cc == 0)
        return arena->Tuple();

    PyList *cols = arena->List(cc);
    //list off the column names:
    for(uint32 r(0); r < cc; ++r)
        cols->set (r, arena->String (result.ColumnName(r)));

    //add a line entry for each result row:
    DBResultRow row;
    PyList *reslist = arena->List();
    while(result.GetRow(row)) {
        //this could be more efficient by not building the column list each time, but cloning it instead.
        PyObject *o = DBRowToRow(row, type, arena);
        reslist->add(o);
    }

    return arena->Tuple ({
        cols, reslist
    });
}

PyDict* DBResultToIntRowDict(DBQueryResult &result, uint32 key_index, PythonArena* arena, const char *type) {
    PyDict* res = arena->Dict();
    int32 k(0);
    //add a line entry for each result row:
    DBResultRow row;
    while (result.GetRow(row)) {
        //this could be more efficient by not building the column list each time, but cloning it instead.
        PyObject *r = DBRowToRow(row, type, arena);
        k = row.GetInt(key_index);
        if (k == 0)
            continue;   //likely a non-integer key
        res->set(arena->Int(k), r);
    }

    return res;
}

PyDict* DBResultToIntIntDict(DBQueryResult &result, PythonArena* arena) {
    PyDict* res = arena->Dict();
    int32 k(0);
    //add a line entry for each result row:
    DBResultRow row;
    while(result.GetRow(row)) {
        if (row.IsNull(0))
            continue;   //no working with NULL keys...
        k = row.GetInt(0);
        if (k == 0)
            continue;   //likely a non-integer key
        if (row.IsNull(1))
            res->set(arena->Int(k), PyStatic.NewNone());
        else
            res->set(arena->Int(k), arena->Int(row.GetInt(1)));
    }

    return res;
}

void FillPackedRow(const DBResultRow& row, PyPackedRow* into, PythonArena* arena = HeapPythonArena::instance)
{
    uint32 cc(row.ColumnCount());
    for (uint32 i(0); i < cc; ++i)
        into->set (i, DBColumnToPyDataType(row, i, arena));
}

PyPackedRow* CreatePackedRow(const DBResultRow& row, DBRowDescriptor* header, PythonArena* arena)
{
    PyPackedRow* res = arena->PackedRow(header);
    FillPackedRow(row, res, arena);
    return res;
}

PyList* DBResultToPackedRowList(DBQueryResult &result, PythonArena* arena)
{
    DBRowDescriptor *header = new(arena) DBRowDescriptor(result);
    PyList * list = arena->List(result.GetRowCount());

    uint32 i(0);
    DBResultRow row;
    while(result.GetRow(row)) {
        list->set (i++, CreatePackedRow(row, header, arena));
        PyIncRef(header);
    }

    PyDecRef(header);
    return list;
}

PyTuple* DBResultToPackedRowListTuple(DBQueryResult &result, PythonArena* arena)
{
    DBRowDescriptor* header = new(arena) DBRowDescriptor(result);
    PyList* list = arena->List(result.GetRowCount());

    DBResultRow row;
    uint32 i(0);
    while(result.GetRow(row)) {
        list->set (i++, CreatePackedRow(row, header, arena));
        PyIncRef(header);
    }

    return arena->Tuple ({
        header, list
    });
}

PyDict *DBResultToPackedRowDict(DBQueryResult &result, const char *key, PythonArena* arena)
{
    uint32 cc(result.ColumnCount());
    uint32 key_index(0);

    for (key_index = 0; key_index < cc; ++key_index)
        if (strcmp(key, result.ColumnName(key_index)) == 0)
            break;

    if(key_index == cc)
    {
        sLog.Error("EVEDBUtils", "DBResultToPackedRowDict | Failed to find key column '%s' in result for CIndexRowset", key);
        return nullptr;
    }

    return DBResultToPackedRowDict(result, key_index, arena);
}

PyDict *DBResultToPackedRowDict(DBQueryResult &result, uint32 key_index, PythonArena* arena)
{
    DBRowDescriptor *header = new(arena) DBRowDescriptor(result);

    PyDict *res = arena->Dict();

    DBResultRow row;
    while(result.GetRow(row)) {
        res->set(DBColumnToPyDataType(row, key_index, arena), CreatePackedRow(row, header, arena));
        PyIncRef(header);
    }

    PyDecRef(header);
    return res;
}

/* Class structure
 * PyClass
 *   PyTuple:2
 *     itr[0]:PyTuple:1
 *       itr[0]:PyClass
 *         PyString:"dbutil.CRowset"
 *     itr[1]:PyDict:1
 *       dict["header"]=PyClass
 *         PyTuple:2
 *           itr[0]:PyClass
 *             PyString:"blue.DBRowDescriptor"
 *           itr[1]:PyTuple:1
 *             itr[0]:PyTuple:N<Amount of cols>
 *               itr[0]:PyTuple:2
 *                 itr[0]:PyString<FieldName>
 *                 itr[1]:PyInt<FieldType> DBTYPE
 *               itr[N]:PyTuple:2
 *                 itr[0]:PyString<FieldName>
 *                 itr[1]:PyInt<FieldType> DBTYPE
 *            PyDict:0
 *            PyList:0
 * PyDict:0
 * PyList:N<Amount of rows>
 *   itr[N]:PyPackedRow
 */

/* this is a very monstrous implementation of a Python Class/Function call
 */
PyObjectEx *DBResultToCRowset(DBQueryResult &result, PythonArena* arena)
{
    /** @todo Mem leak.  `header` never freed */
    DBRowDescriptor *header = new(arena) DBRowDescriptor(result);
    CRowset *rowset = new(arena) CRowset(header);

    DBResultRow row;
    while(result.GetRow(row)) {
        FillPackedRow(row, rowset->insert (), arena);
    }

    //PyDecRef(header);
    return rowset;
}

PyObjectEx *DBResultToCIndexedRowset(DBQueryResult &result, const char *key, PythonArena* arena)
{
    uint32 cc(result.ColumnCount());
    uint32 key_index(0);

    for (key_index = 0; key_index < cc; ++key_index)
        if (strcmp(key, result.ColumnName(key_index)) == 0)
            break;

    if (key_index == cc) {
        sLog.Error("EVEDBUtils", "DBResultToCIndexedRowset | Failed to find key column '%s' in result for CIndexRowset", key);
        return nullptr;
    }

    return DBResultToCIndexedRowset(result, key_index, arena);
}

PyObjectEx *DBResultToCIndexedRowset(DBQueryResult &result, uint32 key_index, PythonArena* arena) {
    /** @todo Mem leak.  `header` never freed */
    DBRowDescriptor *header = new(arena) DBRowDescriptor(result);
    CIndexedRowset *rowset = new(arena) CIndexedRowset(header, result.ColumnName (key_index));

    DBResultRow row;
    while (result.GetRow(row)) {
        PyPackedRow* into = rowset->insert(DBColumnToPyDataType(row, key_index, arena));
        FillPackedRow(row, into, arena);
    }

    //PyDecRef(header);
    return rowset;
}

PyPackedRow *DBRowToPackedRow(DBResultRow &row, PythonArena* arena)
{
    DBRowDescriptor *header = new(arena) DBRowDescriptor(row);
    return CreatePackedRow(row, header, arena);
}

void DBResultToIntIntDict(DBQueryResult &result, std::map<int32, int32> &into) {
    //add a line entry for each result row:
    int32 k(0);
    int32 v(0);
    DBResultRow row;
    while(result.GetRow(row)) {
        if (row.IsNull(0))
            continue;   //no working with NULL keys...
        k = row.GetInt(0);

        if (row.IsNull(1)) {
            v = 0;      //we can deal with assuming NULL == 0
        } else {
            v = row.GetInt(1);
        }

        into[k] = v;
    }
}

void DBResultToUIntUIntDict(DBQueryResult &result, std::map<uint32, uint32> &into) {
    //add a line entry for each result row:
    uint32 k(0);
    uint32 v(0);
    DBResultRow row;
    while(result.GetRow(row)) {
        if (row.IsNull(0))
            continue;   //no working with NULL keys...

        k = row.GetUInt(0);

        if (row.IsNull(1)) {
            v = 0;      //we can deal with assuming NULL == 0
        } else {
            v = row.GetUInt(1);
        }
        into[k] = v;
    }
}

void DBResultToIntIntlistDict(DBQueryResult &result, std::map<int32, PyDataType *> &into, PythonArena* arena) {
    /* this builds a map from the int in result[0], to a list of each result[1]
     * which is has the same result[0]. This function assumes the result is
     * ORDER BY result[0]
     */
    uint32 last_key = 0xFFFFFFFF;

    PyList* l = nullptr;

    DBResultRow row;
    while(result.GetRow(row))
    {
        uint32 k = row.GetUInt(0);
        if (k != last_key)
        {
            //watch for overwrite, no guarantee we are dealing with a key.
            std::map<int32, PyDataType *>::iterator res = into.find(k);
            if (res != into.end())
                PyDecRef(res->second);
            //log an error or warning?

            into[k] = (l = arena->List());
            last_key = k;
        }

        l->add (arena->Int (row.GetInt (1)));
    }
}
