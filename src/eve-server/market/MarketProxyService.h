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

#ifndef __MARKETPROXY_SERVICE_H_INCL__
#define __MARKETPROXY_SERVICE_H_INCL__

#include "market/MarketDB.h"
#include "services/Service.h"
#include "services/ServiceManager.h"

#include "cache/ObjCacheService.h"

class MarketProxyService : public Service <MarketProxyService>
{
public:
    MarketProxyService(EVEServiceManager& mgr);

protected:
    MarketDB m_db;

    EVEResult GetMarketGroups(EVECallArgs& call);
    EVEResult StartupCheck(EVECallArgs& call);
    EVEResult GetCharOrders(EVECallArgs& call);
    EVEResult GetCorporationOrders(EVECallArgs& call);
    EVEResult GetStationAsks(EVECallArgs& call);
    EVEResult GetSystemAsks(EVECallArgs& call);
    EVEResult GetRegionBest(EVECallArgs& call);
    EVEResult GetOldPriceHistory(EVECallArgs& call, PyInt* typeID);
    EVEResult GetNewPriceHistory(EVECallArgs& call, PyInt* typeID);
    EVEResult CorpGetNewTransactions(EVECallArgs& call, PyDataType* sellBuy, PyDataType* typeID, PyDataType* clientID, PyDataType* quantity, PyDataType* fromDate, PyDataType* maxPrice, PyDataType* minPrice, PyDataType* accountKey, PyDataType* memberID);
    EVEResult CharGetNewTransactions(EVECallArgs& call, PyDataType* sellBuy, PyDataType* typeID, PyDataType* clientID, PyDataType* quantity, PyDataType* fromDate, PyDataType* maxPrice, PyDataType* minPrice);
    EVEResult GetOrders(EVECallArgs& call, PyInt* typeID);
    EVEResult PlaceCharOrder(EVECallArgs& call, PyInt* stationID, PyInt* typeID, PyFloat* price, PyInt* quantity, PyInt* bid, PyInt* orderRange, std::optional <PyInt*> itemID, PyInt* minVolume, PyInt* duration, PyBool* useCorp, std::optional<PyDataType*> located);
    EVEResult PlaceCharOrder(EVECallArgs&call, PyInt* stationID, PyInt* typeID, PyFloat* price, PyInt* quantity, PyInt* bid, PyInt* orderRange, std::optional <PyInt*> itemID, PyInt* minVolume, PyInt* duration, PyInt* useCorp, std::optional<PyDataType*> located);
    EVEResult ModifyCharOrder(EVECallArgs& call, PyInt* orderID, PyFloat* newPrice, PyInt* bid, PyInt* stationID, PyInt* solarSystemID, PyFloat* price, PyInt* range, PyInt* volRemaining, PyInt* issueDate);
    EVEResult CancelCharOrder(EVECallArgs& call, PyInt* orderID, PyInt* regionID);

private:
    EVEServiceManager& m_manager;
    ObjCacheService* m_cache;
};

#endif
