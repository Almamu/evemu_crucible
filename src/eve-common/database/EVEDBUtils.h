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
*/

#ifndef __EVEDBUTILS_H_INCL__
#define __EVEDBUTILS_H_INCL__

#include "database/dbcore.h"
#include "network/packet_types.h"

#include "python/Types.h"

void DBResultToIntIntDict(DBQueryResult &result, std::map<int32, int32> &into);
void DBResultToUIntUIntDict(DBQueryResult &result, std::map<uint32, uint32> &into);
// result is assumed to be "ORDER BY result[0]"
void DBResultToIntIntlistDict(DBQueryResult &result, std::map<int32, PyDataType *> &into,
                               PythonArena* arena = HeapPythonArena::instance);

PyDataType *DBColumnToPyDataType(const DBResultRow &row, uint32 column_index,
                                  PythonArena* arena = HeapPythonArena::instance);

// this returns a std PyObject "util.Rowset" with data in 'lines'
PyObject *DBResultToRowset(DBQueryResult &result, PythonArena* arena = HeapPythonArena::instance);
PyObject *DBResultToIndexRowset(DBQueryResult &result, const char *key,
                                 PythonArena* arena = HeapPythonArena::instance);
PyObject *DBResultToIndexRowset(DBQueryResult &result, uint32 key_index,
                                 PythonArena* arena = HeapPythonArena::instance);

PyTuple *DBResultToTupleSet(DBQueryResult &result, PythonArena* arena = HeapPythonArena::instance);
void populateResListWithValues(DBQueryResult &result, PyList *into, PythonArena* arena = HeapPythonArena::instance);
// 2 lists, 1-colNames, 2-PyObject "util.Row" with data in 'lines'
PyTuple *DBResultToRowList(DBQueryResult &result,
                            PythonArena* arena = HeapPythonArena::instance, const char *type = "util.Row");
PyTuple *DBResultToPackedRowListTuple(DBQueryResult &result, PythonArena* arena = HeapPythonArena::instance);

PyDict *DBResultToIntRowDict(DBQueryResult &result, uint32 key_index,
                              PythonArena* arena = HeapPythonArena::instance, const char *type = "util.Row");
PyDict *DBResultToIntIntDict(DBQueryResult &result, PythonArena* arena = HeapPythonArena::instance);
PyDict *DBResultToPackedRowDict(DBQueryResult &result, const char *key,
                                 PythonArena* arena = HeapPythonArena::instance);
PyDict *DBResultToPackedRowDict(DBQueryResult &result, uint32 key_index,
                                 PythonArena* arena = HeapPythonArena::instance);

PyList *DBResultToPackedRowList(DBQueryResult &result, PythonArena* arena = HeapPythonArena::instance);

// this fills PyObjectEx2(util.KeyVal)'s 'list" with PacketRow objects
PyObjectEx *DBResultToCRowset(DBQueryResult &result, PythonArena* arena = HeapPythonArena::instance);
// this fills PyObjectEx2(util.KeyVal)'s 'list" with Indexed PacketRow objects
PyObjectEx *DBResultToCIndexedRowset(DBQueryResult &result, const char *key,
                                      PythonArena* arena = HeapPythonArena::instance);
PyObjectEx *DBResultToCIndexedRowset(DBQueryResult &result, uint32 key_index,
                                      PythonArena* arena = HeapPythonArena::instance);

//single rows:
PyObject *DBRowToKeyVal(DBResultRow &row, PythonArena* arena = HeapPythonArena::instance);
PyObject *DBRowToRow(DBResultRow &row, const char *type = "util.Row", PythonArena* arena = HeapPythonArena::instance);
PyPackedRow *DBRowToPackedRow(DBResultRow &row, PythonArena* arena = HeapPythonArena::instance);

PyPackedRow* CreatePackedRow (const DBResultRow& row, DBRowDescriptor* header,
                              PythonArena* arena = HeapPythonArena::instance);

#endif




