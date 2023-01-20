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
    Updates:    Allan
*/


#ifndef __MAP_SERVICE_H_INCL__
#define __MAP_SERVICE_H_INCL__

#include "services/Service.h"
#include "map/MapDB.h"
#include "Client.h"

class MapService : public Service <MapService>
{
public:
    MapService();

protected:
    MapDB m_db;

    EVEResult GetCurrentEntities(EVECallArgs& call);
    EVEResult GetSolarSystemVisits(EVECallArgs& call);
    EVEResult GetMyExtraMapInfoAgents(EVECallArgs& call);
    EVEResult GetMyExtraMapInfo(EVECallArgs& call);
    EVEResult GetBeaconCount(EVECallArgs& call);
    EVEResult GetStationExtraInfo(EVECallArgs& call);
    EVEResult GetSolarSystemPseudoSecurities(EVECallArgs& call);
    EVEResult GetStationCount(EVECallArgs& call);
    EVEResult GetHistory(EVECallArgs& call, PyInt* int1, PyInt* int2);
    EVEResult GetLinkableJumpArrays(EVECallArgs& call);
    EVEResult GetAllianceJumpBridges(EVECallArgs& call);
    EVEResult GetAllianceBeacons(EVECallArgs& call);
    EVEResult GetCurrentSovData(EVECallArgs& call, PyInt* locationID);
    EVEResult GetRecentSovActivity(EVECallArgs& call);
    EVEResult GetDeadspaceAgentsMap(EVECallArgs& call, PyInt* languageID);
    EVEResult GetDeadspaceComplexMap(EVECallArgs& call, PyInt* languageID);
    EVEResult GetSystemsInIncursions(EVECallArgs& call);
    EVEResult GetSystemsInIncursionsGM(EVECallArgs& call);
    EVEResult GetIncursionGlobalReport(EVECallArgs& call);
    EVEResult GetVictoryPoints(EVECallArgs& call);
    EVEResult GetStuckSystems(EVECallArgs& call);

};

#endif
