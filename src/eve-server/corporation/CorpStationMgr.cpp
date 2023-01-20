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

/*
 * CORP__ERROR
 * CORP__WARNING
 * CORP__INFO
 * CORP__MESSAGE
 * CORP__TRACE
 * CORP__CALL
 * CORP__CALL_DUMP
 * CORP__RSP_DUMP
 * CORP__DB_ERROR
 * CORP__DB_WARNING
 * CORP__DB_INFO
 * CORP__DB_MESSAGE
 */

#include "eve-server.h"



#include "StaticDataMgr.h"
#include "account/AccountService.h"
#include "chat/LSCService.h"
#include "corporation/CorpStationMgr.h"
#include "station/Station.h"
#include "station/StationDataMgr.h"
#include "station/StationDB.h"
#include "services/ServiceManager.h"

CorpStationMgr::CorpStationMgr(EVEServiceManager& mgr) :
    BindableService("corpStationMgr", mgr)
{
    this->Add("GetStationServiceStates", &CorpStationMgr::GetStationServiceStates);
    this->Add("GetImprovementStaticData", &CorpStationMgr::GetImprovementStaticData);
}

BoundDispatcher* CorpStationMgr::BindObject(Client *client, PyDataType* bindParameters) {
    if (!bindParameters->is<PyInt>()) {
        codelog(SERVICE__ERROR, "%s Service: invalid bind argument type %s", GetName().c_str(), bindParameters->TypeString());
        return nullptr;
    }

    uint32_t stationID = bindParameters->as<PyInt> ()->value ();

    // ensure the player is in the given station
    if (client->GetStationID2() != stationID)
        return nullptr;

    auto it = this->m_instances.find (stationID);

    if (it != this->m_instances.end ())
        return it->second;

    CorpStationMgrIMBound* bound = new CorpStationMgrIMBound(this->GetServiceManager(), *this, m_db, stationID);

    this->m_instances.insert_or_assign (stationID, bound);

    return bound;
}

void CorpStationMgr::BoundReleased (CorpStationMgrIMBound* bound) {
    auto it = this->m_instances.find (bound->GetStationID());

    if (it == this->m_instances.end ())
        return;

    this->m_instances.erase (it);
}

EVEResult CorpStationMgr::GetImprovementStaticData(EVECallArgs& call)
{
    _log(CORP__CALL, "CorpStationMgr::Handle_GetImprovementStaticData()");
    call.dump(CORP__CALL_DUMP);

    DBQueryResult res;
    StationDB::StationDB::GetOutpostImprovementStaticData(res);

    DBResultRow row;

    DBRowDescriptor* header = new DBRowDescriptor();
    header->add("typeID", DBTYPE_I4);
    header->add("raceID", DBTYPE_I4);
    header->add("requiredAssemblyLineTypeID", DBTYPE_I4);
    header->add("requiredImprovementTypeID", DBTYPE_I4);

    CRowset* rowset = new CRowset(header);

    while (res.GetRow(row)) {
        PyPackedRow* newRow = rowset->insert();
        newRow->set("typeID", new PyInt(row.GetInt(0)));
        newRow->set("raceID", new PyInt(row.GetInt(1)));
        uint32 asmLine = row.GetInt(2);
        if (asmLine == 0) {
            newRow->set("requiredAssemblyLineTypeID", new PyNone());
        }
        else {
            newRow->set("requiredAssemblyLineTypeID", new PyInt(asmLine));
        }
        uint32 improvement = row.GetInt(3);
        if (improvement == 0) {
            newRow->set("requiredImprovementTypeID", new PyNone());
        }
        else {
            newRow->set("requiredImprovementTypeID", new PyInt(improvement));
        }
    }

    PyDict* dict = new PyDict();
    dict->set ("improvementTypes", rowset);

    return new PyObject("util.KeyVal", dict);
}

EVEResult CorpStationMgr::GetStationServiceStates(EVECallArgs& call)
{
    _log(CORP__CALL, "CorpStationMgr::Handle_GetStationServiceStates()");
    call.dump(CORP__CALL_DUMP);

    DBQueryResult res;
    StationDB::GetStationServiceStates(call.client->GetLocationID(), res);

    return DBResultToIntRowDict(res, 2);
}

CorpStationMgrIMBound::CorpStationMgrIMBound(EVEServiceManager& mgr, CorpStationMgr& parent, CorporationDB& db, uint32 station_id) :
    EVEBoundObject(mgr, parent),
    m_db(db),
    m_stationID(station_id)
{
    this->Add("GetCorporateStationInfo", &CorpStationMgrIMBound::GetCorporateStationInfo);
    this->Add("DoStandingCheckForStationService", &CorpStationMgrIMBound::DoStandingCheckForStationService);
    this->Add("GetPotentialHomeStations", &CorpStationMgrIMBound::GetPotentialHomeStations);
    this->Add("SetHomeStation", &CorpStationMgrIMBound::SetHomeStation);
    this->Add("SetCloneTypeID", &CorpStationMgrIMBound::SetCloneTypeID);
    this->Add("GetQuoteForRentingAnOffice", &CorpStationMgrIMBound::GetQuoteForRentingAnOffice);
    this->Add("GetNumberOfUnrentedOffices", &CorpStationMgrIMBound::GetNumberOfUnrentedOffices);
    this->Add("MoveCorpHQHere", &CorpStationMgrIMBound::MoveCorpHQHere);
    this->Add("RentOffice", &CorpStationMgrIMBound::RentOffice);
    this->Add("CancelRentOfOffice", &CorpStationMgrIMBound::CancelRentOfOffice);
    //testing
    this->Add("GetStationOffices", &CorpStationMgrIMBound::GetStationOffices);
    this->Add("GetCorporateStationOffice", &CorpStationMgrIMBound::GetCorporateStationOffice);
    this->Add("DoesPlayersCorpHaveJunkAtStation", &CorpStationMgrIMBound::DoesPlayersCorpHaveJunkAtStation);
    this->Add("GetQuoteForGettingCorpJunkBack", &CorpStationMgrIMBound::GetQuoteForGettingCorpJunkBack);
    this->Add("PayForReturnOfCorpJunk", &CorpStationMgrIMBound::PayForReturnOfCorpJunk);
    this->Add("GetStationServiceIdentifiers", &CorpStationMgrIMBound::GetStationServiceIdentifiers);
    this->Add("GetStationDetails", &CorpStationMgrIMBound::GetStationDetails);
    this->Add("GetStationServiceAccessRule", &CorpStationMgrIMBound::GetStationServiceAccessRule);
    this->Add("GetStationManagementServiceCostModifiers", &CorpStationMgrIMBound::GetStationManagementServiceCostModifiers);
    this->Add("GetRentableItems", &CorpStationMgrIMBound::GetRentableItems);
    this->Add("GetOwnerIDsOfClonesAtStation", &CorpStationMgrIMBound::GetOwnerIDsOfClonesAtStation);
    this->Add("GetStationImprovements", &CorpStationMgrIMBound::GetStationImprovements);

    pStationItem = sEntityList.GetStationByID(station_id).get();
}

EVEResult CorpStationMgrIMBound::GetStationOffices(EVECallArgs& call)
{
    return pStationItem->GetOffices();
}

EVEResult CorpStationMgrIMBound::GetNumberOfUnrentedOffices(EVECallArgs&call)
{
    return new PyInt(pStationItem->GetAvalibleOfficeCount());
}

EVEResult CorpStationMgrIMBound::GetQuoteForRentingAnOffice(EVECallArgs&call)
{
    return new PyInt(pStationItem->GetOfficeRentalFee());
}

// cannot find this call in client code
EVEResult CorpStationMgrIMBound::GetCorporateStationInfo(EVECallArgs&call) {
    // is this right?
    /* returns:
     *  list(
     *      eveowners:
     *          rowset: ownerID,ownerName,typeID
     *      corporations:
     *          rowset:corporationID,corporationName,description,shares,graphicID,memberCount,ceoID,stationID,raceID,corporationType,creatorID,hasPlayerPersonnelManager,tickerName,sendCharTerminationMessage,shape1,shape2,shape3,color1,color2,color3,typeface,memberLimit,allowedMemberRaceIDs,url,taxRate,minimumJoinStanding,division1,division2,division3,division4,division5,division6,division7,allianceID
     *      offices (may be None):
     *          rowset: corporationID,itemID,officeFolderID
     *      )
     */

    return new PyList {
        m_db.ListStationOwners (m_stationID),
        m_db.ListStationCorps (m_stationID),
        StationDB::GetOffices (m_stationID)/*m_db.ListStationOffices*/
    };
}

EVEResult CorpStationMgrIMBound::SetCloneTypeID(EVECallArgs&call, PyInt* cloneTypeID) {
    //Get cost of clone
    double cost = m_db.GetCloneTypeCostByID(cloneTypeID->value());

    //take the money, send wallet blink event record the transaction in their journal.
    std::string reason = "DESC: Updating Clone in ";
    // make config option for station name or system name here?
    reason += call.client->GetSystemName();
    reason += " at ";
    reason += stDataMgr.GetStationName(call.client->GetStationID());

    AccountService::TransferFunds(
        call.client->GetCharacterID(),
        call.client->GetStationID(),
        cost,
        reason.c_str(),
        Journal::EntryType::CloneActivation,
        call.client->GetStationID(),
        Account::KeyType::Cash
    );

    //update type of clone
    CharacterDB c_db;
    c_db.ChangeCloneType(call.client->GetCharacterID(), cloneTypeID->value());
    return PyStatic.NewNone();
}

EVEResult CorpStationMgrIMBound::RentOffice(EVECallArgs&call, PyInt* amount) {
    // corp role is checked in client before this button is shown.  no need to check here.
    // 1 param, corp rent price
    Client* pClient = call.client;

    // see if corp has office in station already.
    if (pStationItem->GetOfficeID(pClient->GetCorporationID()))
        throw UserError ("RentingYouHaveAnOfficeHere");

    // this may not be needed, as rental fee is queried immediately prior to this call
    if (amount->value() != pStationItem->GetOfficeRentalFee())
        _log(CORP__WARNING, "RentOffice() - Was quoted %i but station reports %u for office rental at %s", \
            amount->value(), pStationItem->GetOfficeRentalFee(), pStationItem->name());

    // check if the corp has enough money
    // remove the money and record the transaction
    std::string reason = "Office Rental at ";
    reason += pStationItem->itemName();
    reason += " by ";
    reason += pClient->GetCharName();

    AccountService::TransferFunds(pClient->GetCorporationID(), pStationItem->GetOwnerID(), amount->value(), reason.c_str(), Journal::EntryType::OfficeRentalFee);

/** @note  why is this disabled?
    int64 balance = AccountDB::GetCorpBalance(pClient->GetCorporationID(), Account::KeyType::Cash);
    if (balance < arg.arg) {
        std::map<std::string, PyDataType *> args;
        args["amount"] = new PyFloat(arg.arg);
        args["balance"] = new PyFloat(balance);
        throw PyException(MakeUserError("NotEnoughMoney", args));
    }
*/
    OfficeData odata = OfficeData();
        odata.ticker = pClient->GetChar()->corpTicker();
        odata.corporationID = pClient->GetCorporationID();
        odata.expiryTime = Win32TimeNow() + EvE::Time::Month;
        odata.lockDown = false;
        odata.rentalFee = amount->value();
        odata.typeID = 27;  // office typeID
    pStationItem->RentOffice(odata);

    if (!odata.officeID) {
        codelog(CORP__ERROR, "%s: Error at renting a new office", pClient->GetName());
        return nullptr;
    }

    // send data to bill mgr for creating bill and notifications

    /*
     *        [PyString "OnNotificationReceived"]
     *        [PyList 0 items]
     *        [PyString "clientID"]           << Notify::Types::
     */

    /** @todo update this to use corpNotify */
    // This has to be sent to everyone in the station
    //  are corp members notified?
    OfficeAttributeUpdate change;
        change.oldOfficeFolderID = PyStatic.NewNone();
        change.oldOfficeID = PyStatic.NewNone();
        change.oldStationID = PyStatic.NewNone();
        change.oldTypeID = PyStatic.NewNone();
        change.newOfficeFolderID = odata.folderID;
        change.newOfficeID = odata.officeID;
        change.newStationID = odata.stationID;
        change.newTypeID = odata.typeID;
    OnObjectPublicAttributesUpdated update;
        update.notificationParams = new PyDict();
        update.realRowCount = 1; // what is this?
        update.bindID = this->GetIDString ();
        update.changePKIndexValue = odata.stationID; // primary key
        update.changes = change.Encode();
    PyTuple* payload = update.Encode();
    std::vector<Client*> cVec;
    cVec.clear();
    pStationItem->GetGuestList(cVec);
    for (auto cur : cVec) {
        PyIncRef(payload);
        cur->SendNotification("OnObjectPublicAttributesUpdated", "stationid", &payload, false);
    }
    PyDecRef( payload );

    // returns officeID
    return new PyInt(odata.officeID);
}

EVEResult CorpStationMgrIMBound::GetCorporateStationOffice(EVECallArgs&call) {
  /*
   *        if not session.stationid2:
   *            return
   *        if self.itemIDOfficeFolderIDByCorporationID is None:
   *            self.itemIDOfficeFolderIDByCorporationID = util.IndexRowset(['corporationID', 'itemID', 'officeFolderID'], [], 'corporationID')
   *            corpStationMgr = self.GetCorpStationManager()
   *            corpStationMgr.Bind()
   *            offices = corpStationMgr.GetCorporateStationOffice()
   *            for office in offices:
   *                self.itemIDOfficeFolderIDByCorporationID[office.corporationID] = [office.corporationID, office.itemID, office.officeFolderID]
   */
  // this gets all corp offices in current station
    return pStationItem->GetOffices();
}

EVEResult CorpStationMgrIMBound::MoveCorpHQHere(EVECallArgs&call)
{
    if (call.client->GetCorpHQ() == m_stationID)
        throw UserError ("CorpHQIsAtThisStation");

    uint32 corpID = call.client->GetCorporationID();
    CorporationDB::UpdateCorpHQ(corpID, m_stationID);

    // update all online members with new hq change.  no OnCorporationChanged bcast
    /** @todo update this */
    Client* pClient(nullptr);
    std::vector<uint32> ids;
    CorporationDB::GetMemberIDs(corpID, ids, true);
    for (auto cur : ids) {
        pClient = sEntityList.FindClientByCharID(cur);
        if (pClient != nullptr)
            pClient->GetChar()->SetCorpHQ(m_stationID);
    }

    return nullptr;
}

/**     ***********************************************************************
 * @note   these below are partially coded
 */

EVEResult CorpStationMgrIMBound::GetPotentialHomeStations(EVECallArgs&call) {
    // this is for station options for xfering clone
    //returns stationID,typeID,serviceMask

    Client* pClient(call.client);
    std::vector<uint32> stVec;
    stVec.push_back(m_stationID);   // current station.  is this right?
    // first char home station always an option
    stVec.push_back(CharacterDB::GetStartingStationByCareer(pClient->GetChar()->careerID()));
    // get corp stations and add to vector
    m_db.GetCorpStations(pClient->GetCorporationID(), stVec);

    // TODO:  not sure how to determine possible stations yet...
    /*  ideas:
     *      get all corp offices.
     *      determine standings of requesting character to specific stations/locations
     *
     *      pc corp members - get corp standings vs. potential stations/corps/locations
     *      npc corp members - get faction standings for potential locations
     *
     *  other possible stations?  test for min/max potential station count?
     */

    PyList* list = new PyList();
    StationData data = StationData();
    for (auto cur : stVec) {
        stDataMgr.GetStationData(cur, data);
        list->add(
            new PyObject(
                "util.KeyVal",
                new PyDict {
                    {"stationID", new PyInt (cur)},
                    {"typeID", new PyInt (data.typeID)},
                    {"serviceMask", new PyInt (data.serviceMask)}
                }
            )
        );
    }

    return list;
}

EVEResult CorpStationMgrIMBound::GetQuoteForGettingCorpJunkBack(
    EVECallArgs&call)
{   //cost = corpStationMgr.GetQuoteForGettingCorpJunkBack()
    _log(CORP__CALL, "CorpStationMgrIMBound::Handle_GetQuoteForGettingCorpJunkBack()");
    call.dump(CORP__CALL_DUMP);

    // office rental fee (with multiplier?  config option for multiplier?)
    //stDataMgr.GetOfficeRentalFee(m_stationID);
    return new PyInt(pStationItem->GetOfficeRentalFee());
}

EVEResult CorpStationMgrIMBound::DoesPlayersCorpHaveJunkAtStation(
    EVECallArgs&call)
{   //if corpStationMgr.DoesPlayersCorpHaveJunkAtStation():
    _log(CORP__CALL, "CorpStationMgrIMBound::Handle_DoesPlayersCorpHaveJunkAtStation()");
    call.dump(CORP__CALL_DUMP);

    // query station for (officeID:flagimpounded)

    // returns boolean
    return PyStatic.NewFalse();
}

EVEResult CorpStationMgrIMBound::SetHomeStation(EVECallArgs&call, PyInt* newHomeStationID) {
    // sm.GetService('corp').GetCorpStationManager().SetHomeStation(newHomeStationID)
    /** @todo this is once a year on live, unless a new char changes corps.
     *  we will need to make other checks when this is called, as i dont think client checks anything.
     */
    call.client->GetChar()->SetBaseID(newHomeStationID->value());
    CharacterDB::ChangeCloneLocation(call.client->GetCharID(), newHomeStationID->value());
    return nullptr;
}


/**     ***********************************************************************
 * @note   these do absolutely nothing at this time....
 */

EVEResult CorpStationMgrIMBound::DoStandingCheckForStationService(EVECallArgs&call, PyInt* stationServiceID) {
    // not sure what this actually does...
    //   corpStationMgr.DoStandingCheckForStationService(stationServiceID)
    /*
     * 01:09:09 L CorpStationMgrIMBound::Handle_DoStandingCheckForStationService(): size= 1
     * 01:09:09 [SvcCallDump]   Call Arguments:
     * 01:09:09 [SvcCallDump]       Tuple: 1 elements
     * 01:09:09 [SvcCallDump]         [ 0] Integer field: 4096  (repair)
     * 23:09:41 L Server: DoStandingCheckForStationService call made to
     * 23:09:41 L CorpStationMgrIMBound::Handle_DoStandingCheckForStationService(): size= 1
     * 23:09:41 [SvcCall]   Call Arguments:
     * 23:09:41 [SvcCall]       Tuple: 1 elements
     * 23:09:41 [SvcCall]         [ 0] Integer field: 8192
     * 23:09:41 L Server: DoStandingCheckForStationService call made to
     * 23:09:41 L CorpStationMgrIMBound::Handle_DoStandingCheckForStationService(): size= 1
     * 23:09:41 [SvcCall]   Call Arguments:
     * 23:09:41 [SvcCall]       Tuple: 1 elements
     * 23:09:41 [SvcCall]         [ 0] Integer field: 16384
     * 18:49:22 L CorpStationMgrIMBound::Handle_DoStandingCheckForStationService(): size= 1
     * 18:49:22 [SvcCall]   Call Arguments:
     * 18:49:22 [SvcCall]       Tuple: 1 elements
     * 18:49:22 [SvcCall]         [ 0] Integer field: 65536  (fitting)
     * 01:11:32 L CorpStationMgrIMBound::Handle_DoStandingCheckForStationService(): size= 1
     * 01:11:32 [SvcCallDump]   Call Arguments:
     * 01:11:32 [SvcCallDump]       Tuple: 1 elements
     * 01:11:32 [SvcCallDump]         [ 0] Integer field: 1048576  (insurance)
     */

    _log(CORP__CALL, "CorpStationMgrIMBound::Handle_DoStandingCheckForStationService()");
    call.dump(CORP__CALL_DUMP);

    // returns None
    return PyStatic.NewNone();
}

EVEResult CorpStationMgrIMBound::CancelRentOfOffice(EVECallArgs&call)
{   //  corpStationMgr.CancelRentOfOffice()
    _log(CORP__CALL, "CorpStationMgrIMBound::Handle_CancelRentOfOffice()");
    call.dump(CORP__CALL_DUMP);

    /* this will need to search for items in office hangar.
     *   if none, then no worries
     *   if items, move them to 'impounded' (flagImpounded - corp junk)
     *
     *  not sure if the items change locations from corp officeID to stationID
     *   if so, cannot remove officeID from office map, but will need a flag to show it as 'not rented'
     */
    return nullptr;
}

EVEResult CorpStationMgrIMBound::PayForReturnOfCorpJunk(
    EVECallArgs&call, PyFloat* cost)
{   //    corpStationMgr.PayForReturnOfCorpJunk(cost)
    _log(CORP__CALL, "CorpStationMgrIMBound::Handle_PayForReturnOfCorpJunk()");
    call.dump(CORP__CALL_DUMP);

    return nullptr;
}

EVEResult CorpStationMgrIMBound::GetStationServiceIdentifiers(EVECallArgs&call)
{
    _log(CORP__CALL, "CorpStationMgrIMBound::Handle_GetStationServiceIdentifiers()");
    call.dump(CORP__CALL_DUMP);

    DBQueryResult res;
    StationDB::GetStationServiceIdentifiers(res);
    // serviceNameID
    return DBResultToCRowset(res);
}

EVEResult CorpStationMgrIMBound::GetStationDetails(EVECallArgs&call, PyInt* stationID)
{
    _log(CORP__CALL, "CorpStationMgrIMBound::Handle_GetStationDetails()");
    call.dump(CORP__CALL_DUMP);

    DBQueryResult res;
    StationDB::GetStationDetails(stationID->value(), res);

    PyDict* dict = new PyDict();
    DBResultRow row;

    while (res.GetRow(row)) {    
        dict->set ("stationName", new PyString(row.GetText(0), row.ColumnLength(0)));
        dict->set ("stationID", new PyInt(row.GetInt(1)));
        dict->set ("orbitID", new PyInt(row.GetInt(2)));
        dict->set ("description", new PyString(row.GetText(3), row.ColumnLength(3)));
        dict->set ("security", new PyFloat(row.GetFloat(4)));
        dict->set ("dockingCostPerVolume", new PyFloat(row.GetFloat(5)));
        dict->set ("officeRentalCost", new PyInt(row.GetInt(6)));
        dict->set ("reprocessingStationsTake", new PyFloat(row.GetFloat(7)));
        dict->set ("reprocessingHangarFlag", new PyInt(row.GetInt(8)));
        dict->set ("corporationID", new PyInt(row.GetInt(9)));
        dict->set ("maxShipVolumeDockable", new PyInt(row.GetInt(10)));
        dict->set ("exitTime", PyStatic.NewNone());
        dict->set ("standingOwnerID", new PyInt(row.GetInt(9)));
        dict->set ("upgradeLevel", new PyInt(row.GetInt(11)));
    }
    return new PyObject("util.KeyVal", dict);
}

EVEResult CorpStationMgrIMBound::GetStationServiceAccessRule(EVECallArgs&call, PyInt* stationID, PyInt* serviceID)
{
    _log(CORP__CALL, "CorpStationMgrIMBound::Handle_GetStationServiceAccessRule()");
    call.dump(CORP__CALL_DUMP);

    DBQueryResult res;
    StationDB::GetStationServiceAccessRule(stationID->value(), serviceID->value(), res);

    PyDict* dict = new PyDict();
    DBResultRow row;

    // If no rule exists for this service, we should return the default
    if (res.GetRowCount() == 0) {
        dict->set ("serviceID", new PyInt(serviceID->value()));
        dict->set ("minimumStanding", new PyFloat(0));
        dict->set ("minimumCharSecurity", new PyFloat(0));
        dict->set ("maximumCharSecurity", new PyFloat(0));
        dict->set ("minimumCorpSecurity", new PyFloat(0));
        dict->set ("maximumCorpSecurity", new PyFloat(0));
    }
    else {
        while (res.GetRow(row)) {    
            dict->set ("serviceID", new PyInt(row.GetInt(0)));
            dict->set ("minimumStanding", new PyFloat(row.GetFloat(2)));
            dict->set ("minimumCharSecurity", new PyFloat(row.GetFloat(3)));
            dict->set ("maximumCharSecurity", new PyFloat(row.GetFloat(4)));
            dict->set ("minimumCorpSecurity", new PyFloat(row.GetFloat(5)));
            dict->set ("maximumCorpSecurity", new PyFloat(row.GetFloat(6)));
        }
    }

    return new PyObject("util.KeyVal", dict);
}

EVEResult CorpStationMgrIMBound::GetStationManagementServiceCostModifiers(EVECallArgs&call, PyInt* stationID)
{
    _log(CORP__CALL, "CorpStationMgrIMBound::Handle_GetStationManagementServiceCostModifiers()");
    call.dump(CORP__CALL_DUMP);

    DBQueryResult res;
    StationDB::GetStationManagementServiceCostModifiers(stationID->value(), res);

    PyDict* dict = new PyDict();
    DBResultRow row;

    DBRowDescriptor *header = new DBRowDescriptor();
    header->add("serviceID", DBTYPE_I4);
    header->add("discountPerGoodStandingPoint", DBTYPE_R4);
    header->add("surchargePerBadStandingPoint", DBTYPE_R4);

    CRowset *rowset = new CRowset(header);

    // If no configuration exists for this service, we should return the default
    if (res.GetRowCount() == 0) {
        StationDB::GetStationServiceIdentifiers(res);
        while (res.GetRow(row)) {
            // Only show services which exist in this station
            uint32 serviceID = row.GetInt(0);
            uint32 serviceMask = stDataMgr.GetStationServiceMask(stationID->value());
            if((serviceMask & serviceID) == serviceID) 
            {
                rowset->insert({
                    new PyInt (serviceID),
                    new PyFloat (0),
                    new PyFloat (0)
                });
            }
        }
    }
    else {
        while (res.GetRow(row)) {    
            rowset->insert({
                new PyInt (row.GetInt (0)),
                new PyFloat (row.GetFloat (1)),
                new PyFloat (row.GetFloat (2))
            });
        }
    }

    return rowset;
}

EVEResult CorpStationMgrIMBound::GetRentableItems(EVECallArgs&call)
{
    _log(CORP__CALL, "CorpStationMgrIMBound::Handle_GetRentableItems()");
    call.dump(CORP__CALL_DUMP);

    DBQueryResult res;
    StationDB::GetRentableItems(call.client->GetLocationID() ,res);

    return DBResultToCRowset(res);    
}

EVEResult CorpStationMgrIMBound::GetOwnerIDsOfClonesAtStation(EVECallArgs&call, PyInt* corporationID)
{
    _log(CORP__CALL, "CorpStationMgrIMBound::Handle_GetOwnerIDsOfClonesAtStation()");
    call.dump(CORP__CALL_DUMP);

    DBQueryResult res;
    StationDB::GetOwnerIDsOfClonesAtStation(call.client->GetLocationID(), corporationID->value(), res);

    return DBResultToCRowset(res);    
}

EVEResult CorpStationMgrIMBound::GetStationImprovements(EVECallArgs&call)
{
    _log(CORP__CALL, "CorpStationMgrIMBound::Handle_GetStationImprovements()");
    call.dump(CORP__CALL_DUMP);

    DBQueryResult res;
    StationDB::GetOutpostImprovements(call.client->GetLocationID(), res);

    PyDict* dict = new PyDict();
    DBResultRow row;

    PyDataType *imp[6];

    // If no data exists for this station, we should return the default
    if (res.GetRowCount() == 0)
        for (int i = 0; i < 6; i++)
            imp[i] = new PyNone();
    else
        while (res.GetRow(row))
            for (int i = 0; i < 6; i++)
                if (row.GetInt(i) == 0)
                    imp[i] == new PyNone();
                else 
                    imp[i] == new PyInt(row.GetInt(i));

    dict->set ("improvementTier2aTypeID", imp[0]);
    dict->set ("improvementTier3aTypeID", imp[1]);
    dict->set ("improvementTier1bTypeID", imp[2]);
    dict->set ("improvementTier1aTypeID", imp[3]);
    dict->set ("improvementTier2bTypeID", imp[4]);
    dict->set ("improvementTier1cTypeID", imp[5]);

    return new PyObject("util.KeyVal", dict);
}