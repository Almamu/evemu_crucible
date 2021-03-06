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
    Author:        Zhur
*/

#ifndef __REPROCESSING_SVC_H__
#define __REPROCESSING_SVC_H__

#include "PyService.h"
#include "PyBoundObject.h"
#include "station/ReprocessingDB.h"

class ReprocessingService
: public PyService
{
public:
    ReprocessingService(PyServiceMgr *mgr);
    virtual ~ReprocessingService();

protected:
    class Dispatcher;
    Dispatcher *const m_dispatch;

    ReprocessingDB m_db;

    virtual PyBoundObject* CreateBoundObject(Client *pClient, const PyRep *bind_args);
};

class ReprocessingServiceBound
: public PyBoundObject
{
public:
    ReprocessingServiceBound(PyServiceMgr *mgr, ReprocessingDB& db, uint32 stationID);
    virtual ~ReprocessingServiceBound();

    PyCallable_DECL_CALL(GetOptionsForItemTypes);
    PyCallable_DECL_CALL(GetReprocessingInfo);
    PyCallable_DECL_CALL(GetQuote);
    PyCallable_DECL_CALL(GetQuotes);
    PyCallable_DECL_CALL(Reprocess);

    virtual void Release();

protected:
    class Dispatcher;
    Dispatcher *const m_dispatch;

    ReprocessingDB& m_db;

    StationItemRef m_stationRef;
    uint32 m_stationCorpID; //corp that owns station. Used for standing
    float m_staEfficiency;
    float m_tax;

    float CalcReprocessingEfficiency(const Client *pClient, InventoryItemRef item = InventoryItemRef(nullptr)) const;
    float CalcTax(float standing) const;
    PyRep* GetQuote(uint32 itemID, Client* pClient);

    float GetStanding(const Client* pClient) const; // gets the higher of char/corp standings with station owner
};


#endif

