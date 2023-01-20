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

#ifndef __FACTION_WAR_MGR__H__INCL__
#define __FACTION_WAR_MGR__H__INCL__

#include "faction/FactionWarMgrDB.h"
#include "services/Service.h"

class FactionWarMgrService : public Service <FactionWarMgrService>
{
public:
    FactionWarMgrService(EVEServiceManager& mgr);

protected:
  EVEResult GetWarFactions(EVECallArgs& call);
  EVEResult GetFWSystems(EVECallArgs& call);
  EVEResult GetMyCharacterRankOverview(EVECallArgs& call);
  EVEResult GetMyCharacterRankInfo(EVECallArgs& call);
  EVEResult GetFactionMilitiaCorporation(EVECallArgs& call, PyInt* factionID);
  EVEResult GetSystemStatus(EVECallArgs& call, PyInt* solarsystemID, PyInt* warFactionID);
  EVEResult IsEnemyFaction(EVECallArgs& call, PyInt* enemyID, PyInt* factionID);
  EVEResult IsEnemyCorporation(EVECallArgs& call, PyInt* enemyID, PyInt* factionID);
  EVEResult GetCharacterRankInfo(EVECallArgs& call, PyInt* characterID);
  EVEResult GetFactionalWarStatus(EVECallArgs& call);
  EVEResult JoinFactionAsCharacter(EVECallArgs& call, PyInt* factionID);
  EVEResult GetCorporationWarFactionID(EVECallArgs& call, PyInt* corporationID);
  EVEResult GetSystemsConqueredThisRun(EVECallArgs& call);
  EVEResult GetFactionCorporations(EVECallArgs& call, PyInt* factionID);
  EVEResult JoinFactionAsCharacterRecommendationLetter(EVECallArgs& call, PyInt* factionID, PyInt* itemID);
  EVEResult JoinFactionAsAlliance(EVECallArgs& call, PyInt* factionID);
  EVEResult JoinFactionAsCorporation(EVECallArgs& call, PyInt* factionID);
  EVEResult GetStats_FactionInfo(EVECallArgs& call);
  EVEResult GetStats_TopAndAllKillsAndVPs(EVECallArgs& call);
  EVEResult GetStats_Character(EVECallArgs& call);
  EVEResult GetStats_Corp(EVECallArgs& call);
  EVEResult GetStats_Alliance(EVECallArgs& call);
  EVEResult GetStats_Militia(EVECallArgs& call);
  EVEResult GetStats_CorpPilots(EVECallArgs& call);
  EVEResult LeaveFactionAsAlliance(EVECallArgs& call, PyInt* factionID);
  EVEResult LeaveFactionAsCorporation(EVECallArgs& call, PyInt* factionID);
  EVEResult WithdrawJoinFactionAsAlliance(EVECallArgs& call, PyInt* factionID);
  EVEResult WithdrawJoinFactionAsCorporation(EVECallArgs& call, PyInt* factionID);
  EVEResult WithdrawLeaveFactionAsAlliance(EVECallArgs& call, PyInt* factionID);
  EVEResult WithdrawLeaveFactionAsCorporation(EVECallArgs& call, PyInt* factionID);
  EVEResult RefreshCorps(EVECallArgs& call);

private:
    ObjCacheService* m_cache;
    EVEServiceManager& m_manager;
    FactionWarMgrDB m_db;
};

#endif /* __FACTION_WAR_MGR_SERVICE__H__INCL__ */


