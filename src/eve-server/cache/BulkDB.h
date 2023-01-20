/**
 * @name BulkDB.h
 *   This file is for loading bulkdata from the db
 *   Copyright 2017  Alasiya-EVEmu Team
 *
 * @Author:    Allan
 * @date:      24 January 2017
 *
 */


#ifndef _EVE_CACHE_BULKDB_H_
#define _EVE_CACHE_BULKDB_H_


#include "../eve-server.h"


class BulkDB
: public Singleton<BulkDB>
{
public:
    BulkDB();
    ~BulkDB();

    void Initialize();
    void Close();

    uint8 GetNumChunks(uint8 setID=0);
    int32 GetFileIDfromChunk(uint8 setID, uint8 chunkID);

    /* updated dogma files to send to client in bulkData */
    PyDataType* GetOperands();
    PyDataType* GetDogmaAttribs();
    PyDataType* GetDogmaEffects();

    /* these are split into ~10k-row chunks for easier handling */
    PyDataType* GetExpressions(uint8 chunkID);
    PyDataType* GetDogmaTypeEffects(uint8 chunkID);
    PyDataType* GetDogmaTypeAttribs(uint8 chunkID);

    /* these are used to get chunks */
    PyDataType* GetBulkData(uint8 chunkID);
    PyDataType* GetBulkDataChunks(uint8 setID, uint8 chunkID);

private:
    PerpetualPythonArena m_arena;

    bool m_loaded;
    uint8 m_chunks;

    std::map<uint8, PyDataType*> m_bulkData;          // chunkID/data (preliminary data)
    std::map<uint8, PyDataType*> m_bulkDataChunks;    // chunkID/data
};

#define sBulkDB \
( BulkDB::get() )

#endif  // _EVE_CACHE_BULKDB_H_

/*  notes to keep track of chunkID and the data it refers to
 *
 *  use this to keep mem map of chunk data, loaded on server start
    m_stationPyData.insert(std::pair<uint32, PyDataType*>(row.GetInt(0), m_sdb.DoGetStation(row.GetInt(0))));
 *
 */