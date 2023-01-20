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


#ifndef __CORPSTATIONMGR_SERVICE_H_INCL__
#define __CORPSTATIONMGR_SERVICE_H_INCL__

#include <map>

#include "corporation/CorporationDB.h"
#include "services/BoundService.h"

class PyRep;
class CorpStationMgrIMBound;

class CorpStationMgr : public BindableService <CorpStationMgr, CorpStationMgrIMBound>
{
public:
    CorpStationMgr(EVEServiceManager& mgr);

protected:
    CorporationDB m_db;

    EVEResult GetStationServiceStates(EVECallArgs& call);
    EVEResult GetImprovementStaticData(EVECallArgs& call);

    //overloaded in order to support bound objects:
    BoundDispatcher* BindObject(Client *client, PyDataType* bindParameters) override;
    void BoundReleased (CorpStationMgrIMBound* bound) override;

private:
    std::map <uint32_t, CorpStationMgrIMBound*> m_instances;
};

class CorpStationMgrIMBound : public EVEBoundObject <CorpStationMgrIMBound>
{
    friend class CorpStationMgr;
public:
    CorpStationMgrIMBound(EVEServiceManager& mgr, CorpStationMgr& parent, CorporationDB& db, uint32 station_id);

protected:
  EVEResult GetCorporateStationInfo(EVECallArgs& call);
  EVEResult DoStandingCheckForStationService(EVECallArgs& call, PyInt* stationServiceID);
  EVEResult GetPotentialHomeStations(EVECallArgs& call);
  EVEResult SetHomeStation(EVECallArgs& call, PyInt* newHomeStationID);
  EVEResult SetCloneTypeID(EVECallArgs& call, PyInt* cloneTypeID);
  EVEResult GetQuoteForRentingAnOffice(EVECallArgs& call);
  EVEResult RentOffice(EVECallArgs& call, PyInt* amount);
  EVEResult CancelRentOfOffice(EVECallArgs& call);
  EVEResult GetStationOffices(EVECallArgs& call);
  EVEResult GetNumberOfUnrentedOffices(EVECallArgs& call);
  EVEResult MoveCorpHQHere(EVECallArgs& call);
    //testing
  EVEResult GetCorporateStationOffice(EVECallArgs& call);
  EVEResult DoesPlayersCorpHaveJunkAtStation(EVECallArgs& call);
  EVEResult GetQuoteForGettingCorpJunkBack(EVECallArgs& call);
  EVEResult PayForReturnOfCorpJunk(EVECallArgs& call, PyFloat* cost);
  EVEResult GetStationServiceIdentifiers(EVECallArgs& call);
  EVEResult GetStationDetails(EVECallArgs& call, PyInt* stationID);
  EVEResult GetStationServiceAccessRule(EVECallArgs& call, PyInt* stationID, PyInt* serviceID);
  EVEResult GetStationManagementServiceCostModifiers(EVECallArgs& call, PyInt* stationID);
  EVEResult GetRentableItems(EVECallArgs& call);
  EVEResult GetOwnerIDsOfClonesAtStation(EVECallArgs& call, PyInt* corporationID);
  EVEResult GetStationImprovements(EVECallArgs& call);

protected:
    const uint32 GetStationID() const { return this->m_stationID; }

    StationItem* pStationItem;    //we do not own this

    CorporationDB& m_db;
    const uint32 m_stationID;
};

#endif

/*
 * RentingOfficeRequestDenied
 * RentingAnOfficeCostsMore
 * NoOfficeAtStation
 * NoOfficesAreAvailableForRenting
 * OfficeEstablishmentItemNotAtStation
 * OfficeRentalCostMustBePositive
 */