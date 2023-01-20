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
    Author:        Zhur, Allan
*/


#ifndef __CONFIG_SERVICE_H_INCL__
#define __CONFIG_SERVICE_H_INCL__

#include "config/ConfigDB.h"
#include "services/Service.h"
#include "Client.h"

class ConfigService : public Service <ConfigService>
{
public:
    ConfigService();

protected:
    ConfigDB m_db;

    EVEResult GetUnits(EVECallArgs& call);
    EVEResult GetMapLandmarks(EVECallArgs& call);
    EVEResult GetMultiOwnersEx(EVECallArgs& call, PyList* ownerIDs);
    EVEResult GetMultiAllianceShortNamesEx(EVECallArgs& call, PyList* allianceIDs);
    EVEResult GetMultiLocationsEx(EVECallArgs& call, PyList* locationIDs);
    EVEResult GetMultiStationEx(EVECallArgs& call, PyList* stationIDs);
    EVEResult GetMultiCorpTickerNamesEx(EVECallArgs& call, PyList* corporationIDs);
    EVEResult GetMultiGraphicsEx(EVECallArgs& call, PyList* graphicIDs);
    EVEResult GetMap(EVECallArgs& call, PyInt* solarSystemID);
    EVEResult GetMapOffices(EVECallArgs& call, PyInt* solarSystemID);
    EVEResult GetMapObjects(EVECallArgs& call, PyInt* systemID, PyInt* region, PyInt* constellation, PyInt* system, PyInt* station, PyInt* unknown);
    EVEResult GetMultiInvTypesEx(EVECallArgs& call, PyList* typeIDs);
    EVEResult GetMapConnections(EVECallArgs& call, PyInt* itemID, PyInt* reg, PyInt* con, PyInt* sol, PyInt* cel, PyInt* _c);
    EVEResult GetStationSolarSystemsByOwner(EVECallArgs& call, PyInt* ownerID);
    EVEResult GetCelestialStatistic(EVECallArgs& call, PyInt* celestialID);
    EVEResult GetDynamicCelestials(EVECallArgs& call, PyInt* locationID);
    EVEResult SetMapLandmarks(EVECallArgs& call, PyList* landmarkData);
};

#endif