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


#ifndef __CORPORATIONDB_H_INCL__
#define __CORPORATIONDB_H_INCL__

#include "ServiceDB.h"
#include "packets/CorporationPkts.h"

#include "corporation/CorpData.h"


struct OfficeData;

class Client;
class PyRep;
class PyObject;

class CorporationDB
: public ServiceDB
{
public:
    PyDataType *GetCorpRoles();
    PyDataType *GetCorpRoleGroups();
    PyDataType *GetTitles(uint32 corpID);
    bool UpdateTitle(uint32 corpID, Call_UpdateTitleData& args, PyDict* updates);
    void DeleteTitle(uint32 corpID, uint16 titleID);
    void CreateTitleData(uint32 corpID);
    PyDataType *GetMember(uint32 charID);
    uint16 GetMemberCount(uint32 corpID);
    void GetMembers(uint32 corpID, DBQueryResult& res);
    void GetMembersForQuery(std::ostringstream& query, std::vector<uint32>& result);
    void GetMembersPaged(uint32 corpID, uint8 page, DBQueryResult& res);
    PyDataType* GetMemberTrackingInfo(uint32 corpID);
    PyDataType* GetMemberTrackingInfoSimple(uint32 corpID);
    PyDataType *GetCorporations(uint32 corpID);
    PyObject *GetCorporation(uint32 corpID);
    PyObject *GetStations(uint32 corpID);
    PyObject *GetEveOwners(uint32 corpID, PythonArena* arena);
    PyDataType* GetBulletins(uint32 corpID);
    void DeleteBulletin(uint32 bulletinID);
    void AddBulletin(uint32 corpID, uint32 ownerID, uint32 cCharID, std::string& title, std::string& body);
    void EditBulletin(uint32 bulletinID, uint32 eCharID, int64 eDataTime, std::string& title, std::string& body);

    PyDataType *GetMyApplications(uint32 charID);
    PyDataType *GetApplications(uint32 corpID);

    void MoveShares(uint32 ownerID, uint32 corpID, int32 corporationID, int32 toShareholderID, int32 numberOfShares);
    PyDataType *GetShares(uint32 corpID);
    PyDataType *GetMyShares(uint32 ownerID);
    bool HasShares(uint32 charID, uint32 corpID);
    PyDataType* PyHasShares(uint32 charID, uint32 corpID);
    PyDataType* GetSharesForCorp(uint32 corpID);

    PyObject *GetEmploymentRecord(uint32 charID);

    void GetCorpStations(uint32 corpID, std::vector<uint32>& stVec);

    void AddItemEvent(uint32 corpID, uint32 charID, uint16 eTypeID);
    void AddRoleHistory(uint32 corpID, uint32 charID, uint32 issuerID, int64 oldRoles, int64 newRoles, bool grantable);
    PyDataType *GetItemEvents(uint32 corpID, uint32 charID, int64 fromDate, int64 toDate, uint8 rowsPerPage);
    PyDataType *GetRoleHistroy(uint32 corpID, uint32 charID, int64 fromDate, int64 toDate, uint8 rowsPerPage);

    void CastVote(uint32 corpID, uint32 charID, uint32 voteCaseID, uint8 optionID);
    void AddVoteCase(uint32 corpID, uint32 charID, Call_InsertVoteCase& args);
    PyDataType* GetVotes(uint32 voteCaseID);
    PyDataType* GetVoteItems(uint32 corpID, uint8 status=0, uint8 maxLen=20);
    PyDataType* GetVoteOptions(uint32 voteCaseID);
    PyDataType* GetSanctionedItems(uint32 corpID, uint8 status=0);

    void GetAutoPay(uint32 corpID, DBQueryResult& res);
    void SetAutoPay();

    //PyObject *ListStationOffices(uint32 station_id);
    PyObject *ListStationCorps(uint32 station_id);
    PyObject *ListStationOwners(uint32 station_id);

    PyDataType *GetCorpInfo(uint32 corpID);

    PyDataType* GetContacts(uint32 corpID);
    void AddContact(uint32 ownerID, int32 contactID, int32 relationshipID);
    void UpdateContact(int32 relationshipID, uint32 contactID, uint32 ownerID);
    void RemoveContact(uint32 contactID, uint32 ownerID);

    PyDataType* GetLabels(uint32 corpID);
    void SetLabel(uint32 corpID, uint32 color, std::string name);
    void EditLabel(uint32 corpID, uint32 labelID, uint32 color, std::string name);
    void DeleteLabel(uint32 corpID, uint32 labelID);

    bool IsTickerTaken(std::string ticker);
    bool AddCorporation(Call_AddCorporation & corpInfo, Client* pClient, uint32 & corpID);
    bool CreateCorporationChangePacket(OnCorporationChanged & cc, uint32 oldCorpID, uint32 newCorpID);
    bool CreateCorporationCreatePacket(OnCorporationChanged & cc, uint32 oldCorpID, uint32 newCorpID);

    bool FetchOfficesKeys (uint32 corporationID, DBQueryResult& res);
    bool FetchOffices(uint32 corpID, uint32 from, uint32 count, DBQueryResult& res);

    PyObject* GetCorporationBills(uint32 corpID, bool payable);

    int32 GetCorpIDforChar(int32 charID);
    uint32 GetStationOwner(uint32 stationID);
    uint32 GetStationCorporationCEO(uint32 stationID);
    static uint32 GetCorporationCEO(uint32 corpID);
    uint16 GetCorpMemberCount(uint32 corpID);
    uint16 GetCorpMemberLimit(uint32 corpID);

    double GetCloneTypeCostByID(uint32 cloneTypeID);

    PyDataType* GetMedalsReceived(int32 charID);
    PyDataType* GetAllCorpMedals(uint32 corpID);
    PyDataType* GetCorpMedalData(uint32 corpID);
    PyDataType* GetMedalsReceivedDetails(int32 charID);
    PyDataType* GetRecipientsOfMedal(int32 medalID);
    PyDataType* GetMedalStatuses();
    PyObjectEx* GetMedalDetails(int32 medalID);
    void GiveMedalToCharacters(uint32 issuerID, uint32 corpID, int32 medalID, std::vector< uint32 >& charVec, const std::string& reason);
    uint16 CreateMedal(uint32 ownerID, uint32 creatorID, const std::string& title, const std::string& description);
    void SaveMedalData(int64 medalID, std::vector< Corp::MedalData >& dataList);
    void SetMedalStatus(uint32 charID, uint16 medalID, uint8 status);

    PyDataType* GetAdTypeData();
    PyDataType* GetAdGroupData();
    PyDataType* GetAdRegistryData(int64 typeMask=0, bool inAlliance=false, int16 minMembers=0, uint16 maxMembers=12602);
    PyDataType* GetAdvert(uint16 adID);
    void DeleteAdvert(uint16 adID);

    uint32 CreateAdvert(Client* pClient, uint32 corpID, int64 typeMask, int8 days, uint16 members, std::string description,
                      uint32 channelID, std::string title);

    void UpdateAdvert(uint16 adID, uint32 corpID, int64 typeMask, int8 days, uint16 members, std::string description,
                        uint32 channelID, std::string title);

    int64 GetAdvertTime(uint16 adID, uint32 corpID);
    void AddRecruiters(uint16 adID, int32 corpID, std::vector< int32 >& charVec);
    PyDataType* GetRecruiters(int32 corpID, uint16 adID);

    bool InsertApplication(Corp::ApplicationInfo& aInfo);
    bool UpdateApplication(const Corp::ApplicationInfo& aInfo);
    bool DeleteApplication(const Corp::ApplicationInfo& aInfo);
    bool GetCurrentApplicationInfo(Corp::ApplicationInfo& aInfo);
    bool CreateMemberAttributeUpdate(uint32 newCorpID, uint32 charID, MemberAttributeUpdate& attrib);

    static std::string GetCorpName(uint32 corpID);
    static std::string GetDivisionName(uint32 corpID, uint16 acctKey);
    bool UpdateDivisionNames(uint32 corpID, const Call_UpdateDivisionNames & divs, PyDict * notif);
    bool UpdateCorporation(uint32 corpID, const std::string& description, const std::string& url, double taxRate, PyDict* notif);
    bool UpdateLogo(uint32 corpID, const Call_UpdateLogo & upd, PyDict * notif);

    PyDataType* GetAssetInventory(uint32 corpID, EVEItemFlags locFlag, const char* flags);
    PyDataType* GetAssetInventoryForLocation(uint32 corpID, uint32 locationID, const char* flags);
    PyDataType* GetKillsAndLosses(uint32 corpID, uint32 number, uint32 offset);

    PyDataType* GetMktInfo(uint32 corpID);

    // gets taxrate, allyID, warID of corpID
    static void GetCorpData(CorpData& data);

    static void UpdateCorpHQ(uint32 corpID, uint32 stationID);
    static void GetMemberIDs(uint32 corpID, std::vector<uint32>& ids, bool online=true);

    static bool GetCorporationBySchool(uint32 schoolID, uint32 &corporationID);
    static bool GetLocationCorporationByCareer(CharacterData &cdata, uint32 &corporationID);
    static bool DoesCorporationExist(uint32 corpID);
};

#endif
