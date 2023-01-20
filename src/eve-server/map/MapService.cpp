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
    Rewrite:    Allan
*/

#include "eve-server.h"


#include "StaticDataMgr.h"
#include "map/MapData.h"
#include "map/MapService.h"
#include "system/SystemManager.h"
#include "system/sov/SovereigntyDataMgr.h"

MapService::MapService() :
    Service("map", eAccessLevel_Character)
{
    this->Add("GetHistory", &MapService::GetHistory);
    this->Add("GetBeaconCount", &MapService::GetBeaconCount);    //ColorStarsByCynosuralFields
    this->Add("GetStationCount", &MapService::GetStationCount);    //ColorStarsByStationCount
    this->Add("GetMyExtraMapInfo", &MapService::GetMyExtraMapInfo);     //ColorStarsByCorpMates
    this->Add("GetStationExtraInfo", &MapService::GetStationExtraInfo);
    this->Add("GetSolarSystemVisits", &MapService::GetSolarSystemVisits);
    this->Add("GetLinkableJumpArrays", &MapService::GetLinkableJumpArrays);
    this->Add("GetMyExtraMapInfoAgents", &MapService::GetMyExtraMapInfoAgents);  //ColorStarsByMyAgents
    this->Add("GetSolarSystemPseudoSecurities", &MapService::GetSolarSystemPseudoSecurities);

    /**  not handled yet...these are empty calls  */
    this->Add("GetStuckSystems", &MapService::GetStuckSystems);
    this->Add("GetRecentSovActivity", &MapService::GetRecentSovActivity);
    this->Add("GetDeadspaceAgentsMap", &MapService::GetDeadspaceAgentsMap);
    this->Add("GetDeadspaceComplexMap", &MapService::GetDeadspaceComplexMap);
    this->Add("GetIncursionGlobalReport", &MapService::GetIncursionGlobalReport);
    this->Add("GetSystemsInIncursions", &MapService::GetSystemsInIncursions);    //ColorStarsByIncursions
    this->Add("GetSystemsInIncursionsGM", &MapService::GetSystemsInIncursionsGM);  //ColorStarsByIncursions
    this->Add("GetVictoryPoints", &MapService::GetVictoryPoints);
    this->Add("GetAllianceJumpBridges", &MapService::GetAllianceJumpBridges);
    this->Add("GetAllianceBeacons", &MapService::GetAllianceBeacons);
    this->Add("GetCurrentSovData", &MapService::GetCurrentSovData);
    // custom call for displaying all items in system
    this->Add ("GetCurrentEntities", &MapService::GetCurrentEntities);
}

EVEResult MapService::GetCurrentEntities(EVECallArgs&call)
{
    return call.client->SystemMgr()->GetCurrentEntities();
}

EVEResult MapService::GetSolarSystemVisits(EVECallArgs&call)
{
    return MapDB::GetSolSystemVisits(call.client->GetCharacterID());
}

EVEResult MapService::GetMyExtraMapInfoAgents(EVECallArgs&call)
{
    return StandingDB::GetMyStandings(call.client->GetCharacterID());
}

EVEResult MapService::GetMyExtraMapInfo(EVECallArgs&call)
{
    return CharacterDB::GetMyCorpMates(call.client->GetCorporationID());
}

EVEResult MapService::GetBeaconCount(EVECallArgs&call)
{
    return MapDB::GetDynamicData(2, 24);
}

EVEResult MapService::GetStationExtraInfo(EVECallArgs&call)
{
    return sMapData.GetStationExtraInfo();
}

EVEResult MapService::GetSolarSystemPseudoSecurities(EVECallArgs&call)
{
    // cant find a call to this in client (possible old call)
    return sMapData.GetPseudoSecurities();
}

EVEResult MapService::GetStationCount(EVECallArgs&call)
{
    // cached on client side.  if cache is empty, this call is made.
    return sDataMgr.GetStationCount();
}

EVEResult MapService::GetHistory(EVECallArgs&call, PyInt* int1, PyInt* int2) {
    if (is_log_enabled(SERVICE__CALLS))
        sLog.Cyan( "MapService::Handle_GetHistory()", "type: %i, timeframe: %i", int1, int2 );

    return MapDB::GetDynamicData(int1->value(), int2->value());
}

EVEResult MapService::GetLinkableJumpArrays(EVECallArgs&call)
{   // working
    DBQueryResult res;
    PosMgrDB::GetLinkableJumpArrays(call.client->GetCorporationID(), res);
    PyList* list = new PyList();
    DBResultRow row;
    while (res.GetRow(row)) {
        // SELECT systemID, itemID
        list->add(new PyTuple {
            new PyInt (row.GetInt(0)),
            new PyInt (row.GetInt(1))
        });
    }

    return list;
}

/** not handled */

EVEResult MapService::GetAllianceJumpBridges(EVECallArgs&call)
{
    /**     bridgesByLocation = m.GetAllianceJumpBridges()
     *      for toLocID, fromLocID in bridgesByLocation:
     */
    sLog.Warning( "MapService::Handle_GetAllianceJumpBridges()", "size=%lu", call.tuple->size());
    call.dump(SERVICE__CALL_DUMP);

    DBQueryResult res;
    PosMgrDB::GetAllianceJumpArrays(call.client->GetAllianceID(), res);
    PyList* list = new PyList();
    DBResultRow row;
    while (res.GetRow(row)) {
        // SELECT systemID, itemID
        list->add(new PyTuple {
            new PyInt (row.GetInt(0)),
            new PyInt (row.GetInt(1))
        });
    }

    return list;
}

EVEResult MapService::GetAllianceBeacons(EVECallArgs&call)
{
    sLog.Warning( "MapService::Handle_GetAllianceBeacons()", "size=%lu", call.tuple->size());
    call.dump(SERVICE__CALL_DUMP);

    // Get data directly from sovereignty manager, avoiding db hits
    return svDataMgr.GetAllianceBeacons(call.client->GetAllianceID());
}

EVEResult MapService::GetCurrentSovData(
    EVECallArgs&call, PyInt* locationID)
{/**
    data = sm.RemoteSvc('map').GetCurrentSovData(constellationID)
    returns locationID, ?
    return sm.RemoteSvc('map').GetCurrentSovData(locationID)
    */
    sLog.Warning( "MapService::Handle_GetCurrentSovData()", "size=%lu", call.tuple->size());
    call.dump(SERVICE__CALL_DUMP);

    return svDataMgr.GetCurrentSovData(locationID->value());
}

EVEResult MapService::GetRecentSovActivity(EVECallArgs&call)
{
    /** @todo will have to make db table for this one.  */
    /*
     *    data = sm.RemoteSvc('map').GetRecentSovActivity()
     *    changes = []
     *    resultMap = {}
     *    toPrime = set()
     *    for item in data:
     *        if item.stationID is None:
     *            if bool(changeMode & mapcommon.SOV_CHANGES_SOV_GAIN) and item.oldOwnerID is None:
     *                changes.append((item.solarSystemID, 'UI/Map/StarModeHandler/sovereigntySovGained', (None, item.ownerID)))
     *                toPrime.add(item.ownerID)
     *            elif bool(changeMode & mapcommon.SOV_CHANGES_SOV_LOST) and item.ownerID is None:
     *                changes.append((item.solarSystemID, 'UI/Map/StarModeHandler/sovereigntySovLost', (item.oldOwnerID, None)))
     *                toPrime.add(item.oldOwnerID)
     *        elif bool(changeMode & mapcommon.SOV_CHANGES_SOV_GAIN) and item.oldOwnerID is None:
     *            changes.append((item.solarSystemID, 'UI/Map/StarModeHandler/sovereigntyNewOutpost', (None, item.ownerID)))
     *            toPrime.add(item.ownerID)
     *        elif bool(changeMode & mapcommon.SOV_CHANGES_SOV_GAIN) and item.ownerID is not None:
     *            changes.append((item.solarSystemID, 'UI/Map/StarModeHandler/sovereigntyConqueredOutpost', (item.ownerID, item.oldOwnerID)))
     *            toPrime.add(item.ownerID)
     *            toPrime.add(item.oldOwnerID)
     *
     */

    PyDict* result = new PyDict();

    return result;
}

//   DED Agent Site Report
EVEResult MapService::GetDeadspaceAgentsMap(
    EVECallArgs&call, PyInt* languageID)
{/* no packet data
        dungeons = sm.RemoteSvc('map').GetDeadspaceAgentsMap(eve.session.languageID)
        solarSystemID, dungeonID, difficulty, dungeonName = dungeons
*/
    PyDataType *result = new PyDict();

    return result;
}

//  DED Deadspace Report
//22:37:54 L MapService::Handle_GetDeadspaceComplexMap(): size= 1
EVEResult MapService::GetDeadspaceComplexMap(
    EVECallArgs&call, PyInt* languageID)
{/* no packet data
        dungeons = sm.RemoteSvc('map').GetDeadspaceComplexMap(eve.session.languageID)
        solarSystemID, dungeonID, difficulty, dungeonName = dungeons

        get this data from managerDB.GetAnomalyList(DBQueryResult& res)
        res =  sysSignatures (sigID,sigItemID,dungeonType,sigName,systemID,sigTypeID,sigGroupID,scanGroupID,scanAttributeID,x,y,z)
*/
    sLog.Warning( "MapService::Handle_GetDeadspaceComplexMap()", "size=%lu", call.tuple->size());
    call.dump(SERVICE__CALL_DUMP);
    PyDataType *result = new PyDict();

    return result;
}

EVEResult MapService::GetSystemsInIncursions(EVECallArgs&call) {
    /**  EVE_Incursion.h
     *        participatingSystems = ms.GetSystemsInIncursions()
     *        for solarSystemID, sceneType in participatingSystems:
     *            -- (sceneType = staging, vanguard)
     */

    // copy format from GetLinkableJumpArrays()
    DBQueryResult res;
    //PosMgrDB::GetAllianceBeacons(call.client->GetCorporationID(), res);
    PyList* list = new PyList();
    DBResultRow row;
    while (res.GetRow(row)) {
        // SELECT systemID, sceneType
        list->add(new PyTuple {
            new PyInt (row.GetInt (0)), // solarSystemID
            new PyInt (row.GetInt (1)), // sceneType
        });
    }

    return list;
}

EVEResult MapService::GetSystemsInIncursionsGM(EVECallArgs&call) {
    /**
     *        participatingSystems = ms.GetSystemsInIncursionsGM()
     *        for solarSystemID, sceneType in participatingSystems:
     *            -- (sceneType = staging, vanguard, assault, headquarters)
     */

    // copy format from GetLinkableJumpArrays()
    DBQueryResult res;
    //PosMgrDB::GetAllianceBeacons(call.client->GetCorporationID(), res);
    PyList* list = new PyList();
    DBResultRow row;
    while (res.GetRow(row)) {
        // SELECT systemID, sceneType
        list->add(new PyTuple {
            new PyInt (row.GetInt (0)), // solarSystemID,
            new PyInt (row.GetInt (1)), // sceneType
        });
    }

    return list;
}

//05:52:07 L MapService::Handle_GetIncursionGlobalReport(): size= 0
EVEResult MapService::GetIncursionGlobalReport(EVECallArgs&call) {
  /**
            report = sm.RemoteSvc('map').GetIncursionGlobalReport()
            rewardGroupIDs = [ r.rewardGroupID for r in report ]
            delayedRewards = sm.GetService('incursion').GetDelayedRewardsByGroupIDs(rewardGroupIDs)
            scrolllist = []
            factionsToPrime = set()
            for data in report:
                data.jumps = GetJumps(data.stagingSolarSystemID)
                data.influenceData = util.KeyVal(influence=data.influence, lastUpdated=data.lastUpdated, graceTime=data.graceTime, decayRate=data.decayRate)
                ssitem = map.GetItem(data.stagingSolarSystemID)
                data.stagingSolarSystemName = ssitem.itemName
                data.security = map.GetSecurityStatus(data.stagingSolarSystemID)
                data.constellationID = ssitem.locationID
                data.constellationName = map.GetItem(ssitem.locationID).itemName
                data.factionID = ssitem.factionID or starmap.GetAllianceSolarSystems().get(data.stagingSolarSystemID, None)
                factionsToPrime.add(data.factionID)
                rewards = delayedRewards.get(data.rewardGroupID, None)
                data.loyaltyPoints = rewards[0].rewardQuantity if rewards else 0
                scrolllist.append(listentry.Get('GlobalIncursionReportEntry', data))
*/
  /*
      [PySubStream 1128 bytes]
        [PyObjectData Name: objectCaching.CachedMethodCallResult]
          [PyTuple 3 items]
            [PyDict 1 kvp]
              [PyString "versionCheck"]
              [PyTuple 3 items]
                [PyNone]
                [PyNone]
                [PyString "15 minutes"]
            [PySubStream 1027 bytes]
              [PyList 7 items]
                [PyObjectData Name: util.KeyVal]
                  [PyDict 9 kvp]
                    [PyString "graceTime"]
                    [PyFloat 30]
                    [PyString "decayRate"]
                    [PyFloat 0.00999999977648258]
                    [PyString "influence"]
                    [PyFloat 0.0145000005140901]
                    [PyString "lastUpdated"]
                    [PyIntegerVar 129492976800000000]
                    [PyString "state"]
                    [PyInt 1]
                    [PyString "hasBoss"]
                    [PyInt 0]
                    [PyString "stagingSolarSystemID"]
                    [PyInt 30004323]
                    [PyString "rewardGroupID"]
                    [PyInt 192]
                    [PyString "taleID"]
                    [PyInt 192]
            [PyList 2 items]
              [PyIntegerVar 129493861959830226]
              [PyInt -950263469]
              */
    sLog.Warning( "MapService::Handle_GetIncursionGlobalReport()", "size=%lu", call.tuple->size());
    call.dump(SERVICE__CALL_DUMP);

    return PyStatic.NewNone();
}

//   factional warfare shit
//https://wiki.eveonline.com/en/wiki/Victory_Points_and_Command_Bunker
EVEResult MapService::GetVictoryPoints(
    EVECallArgs&call)
{/**           factionID, viewmode, solarsystemid, threshold, current in oldhistory.iteritems():
                 */
    sLog.Warning( "MapService::Handle_GetVictoryPoints()", "size=%lu", call.tuple->size());
    call.dump(SERVICE__CALL_DUMP);

    return PyStatic.NewNone();
}

EVEResult MapService::GetStuckSystems(EVECallArgs&call)
{
    // cant find a call to this in client (possible old call)
    sLog.Warning( "MapService::Handle_GetStuckSystems()", "size=%lu", call.tuple->size());
    call.dump(SERVICE__CALL_DUMP);

    return new PyTuple {
        new PyInt (0)
    };
}

