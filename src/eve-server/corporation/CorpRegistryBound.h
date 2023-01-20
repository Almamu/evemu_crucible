


#ifndef __EVEMU_CORP_CORPREGISTRY_BOUND_H_
#define __EVEMU_CORP_CORPREGISTRY_BOUND_H_


#include "../eve-server.h"
#include "corporation/CorporationDB.h"
#include "services/BoundService.h"
#include "chat/LSCService.h"
#include "cache/ObjCacheService.h"
#include "corporation/OfficeSparseBound.h"

class CorpRegistryService;
class OfficeSparseBound;

class CorpRegistryBound : public EVEBoundObject <CorpRegistryBound>, public BoundServiceParent <OfficeSparseBound>
{
public:
    CorpRegistryBound(EVEServiceManager &mgr, CorpRegistryService& parent, CorporationDB& db, uint32 corpID);

    uint32 GetCorporationID() const { return this->m_corpID; }

    void BoundReleased (OfficeSparseBound* bound) override;

protected:
  EVEResult GetEveOwners(EVECallArgs& call);
  EVEResult GetCorporation(EVECallArgs& call);
  EVEResult GetCorporations(EVECallArgs& call, PyInt* corporationID);
  EVEResult GetInfoWindowDataForChar(EVECallArgs& call, std::optional <PyInt*> characterID);
  EVEResult GetLockedItemLocations(EVECallArgs& call);
  EVEResult AddCorporation(EVECallArgs& call,
        PyDataType* name, PyDataType* ticker, PyDataType* description, PyDataType* url, PyFloat* taxRate,
        PyDataType* shape1, PyDataType* shape2, PyDataType* shape3,
        PyDataType* color1, PyDataType* color2, PyDataType* color3,
        PyDataType* typeface, PyInt* applicationEnabled);
  EVEResult GetSuggestedTickerNames(EVECallArgs& call, PyString* name);
  EVEResult GetOffices(EVECallArgs& call);
  EVEResult GetStations(EVECallArgs& call);

  EVEResult CreateRecruitmentAd(EVECallArgs& call, PyInt* days, PyInt* typeMask, std::optional <PyInt*> allianceID, PyString* description, PyInt* channelID, PyList* recruiterIDs, PyString* title);
  EVEResult UpdateRecruitmentAd(EVECallArgs& call, PyInt* adID, PyInt* typeMask, PyString* description, PyInt* channelID, PyList* recruiterIDs, PyString* title, PyInt* addedDays);
  EVEResult DeleteRecruitmentAd(EVECallArgs& call, PyInt* adID);
  EVEResult GetRecruiters(EVECallArgs& call, PyInt* corpID, PyInt* adID);
  EVEResult GetRecruitmentAdsForCorporation(EVECallArgs& call);
  EVEResult GetMyApplications(EVECallArgs& call);
  EVEResult InsertApplication(EVECallArgs& call, PyInt* corporationID, PyDataType* message);
  EVEResult GetApplications(EVECallArgs& call);
  EVEResult UpdateApplicationOffer(EVECallArgs&call, PyInt* characterID, PyDataType* applicationText, PyInt* status, PyNone* applicationDateTime);
  EVEResult DeleteApplication(EVECallArgs& call, PyInt* corporationID, PyInt* characterID);

  EVEResult UpdateDivisionNames(EVECallArgs& call,
        PyDataType* div1, PyDataType* div2, PyDataType* div3, PyDataType* div4, PyDataType* div5, PyDataType* div6, PyDataType* div8,
        PyDataType* wal1, PyDataType* wal2, PyDataType* wal3, PyDataType* wal4, PyDataType* wal5, PyDataType* wal6, PyDataType* wal7);
  EVEResult UpdateCorporation(EVECallArgs& call, PyDataType* description, PyDataType* url, PyFloat* tax);
  EVEResult UpdateLogo(EVECallArgs& call,
        PyDataType* shape1, PyDataType* shape2, PyDataType* shape3,
        PyDataType* color1, PyDataType* color2, PyDataType* color3,
        PyDataType* typeface);
  EVEResult SetAccountKey(EVECallArgs& call, PyInt* accountKey);
  EVEResult GetMember(EVECallArgs& call, PyInt* characterID);
  EVEResult GetMembers(EVECallArgs& call);

  EVEResult MoveCompanyShares(EVECallArgs& call, PyInt* corporationID, PyInt* toShareholderID, PyInt* numberOfShares);
  EVEResult MovePrivateShares(EVECallArgs& call, PyInt* corporationID, PyInt* toShareholderID, PyInt* numberOfShares);
  EVEResult GetSharesByShareholder(EVECallArgs& call, PyBool* corpShares);
  EVEResult GetShareholders(EVECallArgs& call);
  EVEResult PayoutDividend(EVECallArgs& call, PyBool* payShareholders, PyFloat* payoutAmount);

  EVEResult CanViewVotes(EVECallArgs& call, PyInt* corporationID);
  EVEResult InsertVoteCase(EVECallArgs& call, PyDataType* voteCaseText, PyDataType* description, PyInt* corporationID, PyInt* voteType, std::optional <PyObject*> voteCaseOptions, PyInt* startDateTime, PyInt* endDateTime);
  EVEResult GetVotes(EVECallArgs& call, PyInt* corporationId, PyInt* voteCaseID);
  EVEResult CanVote(EVECallArgs& call, PyInt* corporationID);
  EVEResult InsertVote(EVECallArgs& call, PyInt* corporationID, PyInt* voteCaseID, PyInt* voteValue);
  EVEResult GetVoteCasesByCorporation(EVECallArgs& call, PyInt* corporationID, std::optional<PyInt*> status, std::optional <PyInt*> maxLen);
  EVEResult GetVoteCaseOptions(EVECallArgs& call, PyInt* corporationID, PyInt* voteCaseID);
  EVEResult GetSanctionedActionsByCorporation(EVECallArgs& call, PyInt* corporationID, PyInt* state);

  EVEResult GetRoleGroups(EVECallArgs& call);
  EVEResult GetRoles(EVECallArgs& call);

  EVEResult CreateLabel(EVECallArgs& call, PyDataType* name, PyDataType* color);
  EVEResult GetLabels(EVECallArgs& call);
  EVEResult DeleteLabel(EVECallArgs& call, PyInt* labelID);
  EVEResult EditLabel(EVECallArgs& call, PyInt* labelID, PyDataType* name, PyDataType* color);
  EVEResult AssignLabels(EVECallArgs& call, PyList* contactIDs, PyInt* labelMask);
  EVEResult RemoveLabels(EVECallArgs& call, PyList* contactIDs, PyInt* labelMask);

  EVEResult GetTitles(EVECallArgs& call);
  EVEResult UpdateTitle(EVECallArgs& call,
        PyDataType* titleID, PyDataType* titleName,
        PyDataType* roles, PyDataType* grantableRoles,
        PyDataType* rolesAtHQ, PyDataType* grantableRolesAtHQ,
        PyDataType* rolesAtBase, PyDataType* grantableRolesAtBase,
        PyDataType* rolesAtOther, PyDataType* grantableRolesAtOther);
  EVEResult UpdateTitles(EVECallArgs& call, PyObject* titles);
  EVEResult DeleteTitle(EVECallArgs& call, PyInt* titleID);

  EVEResult AddBulletin(EVECallArgs& call, PyDataType* title, PyDataType* body);
  EVEResult GetBulletins(EVECallArgs& call);
  EVEResult DeleteBulletin(EVECallArgs& call, PyInt* bulletinID);

  EVEResult GetLocationalRoles(EVECallArgs& call);
  EVEResult GetRecentKillsAndLosses(EVECallArgs& call, PyInt* number, PyInt* offset);
  EVEResult UpdateMember(EVECallArgs& call,
        PyInt* characterID, PyDataType* title, PyDataType* divisionID, PyDataType* squadronID,
        PyInt* roles, PyInt* grantableRoles, PyInt* rolesAtHQ, PyInt* grantableRolesAtHQ,
        PyInt* rolesAtBase, PyInt* grantableRolesAtBase, PyInt* rolesAtOther, PyInt* grantableRolesAtOther,
        PyDataType* baseID, PyDataType* titleMask, PyDataType* blockRoles);
  EVEResult ExecuteActions(EVECallArgs& call, PyList* targetIDs, PyList* remoteActions);

  EVEResult GetCorporateContacts(EVECallArgs& call);
  EVEResult AddCorporateContact(EVECallArgs& call, PyInt* contactID, PyInt* relationshipID);
  EVEResult EditContactsRelationshipID(EVECallArgs& call, PyList* contactIDs, PyInt* relationshipID);
  EVEResult RemoveCorporateContacts(EVECallArgs& call, PyList* contactIDs);
  EVEResult EditCorporateContact(EVECallArgs& call, PyInt* contactID, PyInt* relationshipID);

  EVEResult CreateAlliance(EVECallArgs& call, PyDataType* allianceName, PyDataType* shortName, PyDataType* description, PyDataType* url);
  EVEResult ApplyToJoinAlliance(EVECallArgs& call, PyInt* allianceID, PyString* applicationText);
  EVEResult DeleteAllianceApplication(EVECallArgs& call, PyInt* allianceID);
  EVEResult GetAllianceApplications(EVECallArgs& call);
  EVEResult GetSuggestedAllianceShortNames(EVECallArgs& call, PyString* name);

  EVEResult GetMemberTrackingInfo(EVECallArgs& call);
  EVEResult GetMembersPaged(EVECallArgs& call, PyInt* page);
  EVEResult GetMembersByIds(EVECallArgs& call, PyList* memberIDs);
  EVEResult GetMemberIDsWithMoreThanAvgShares(EVECallArgs& call);
  EVEResult GetMemberIDsByQuery(EVECallArgs& call, PyList* queryList, std::optional <PyInt*> includeImplied, PyInt* searchTitles);
  EVEResult GetMemberTrackingInfoSimple(EVECallArgs& call);

  EVEResult GetRentalDetailsPlayer(EVECallArgs& call);
  EVEResult GetRentalDetailsCorp(EVECallArgs& call);

  EVEResult UpdateCorporationAbilities(EVECallArgs& call);
  EVEResult UpdateStationManagementSettings(EVECallArgs& call,
        PyDataType* modifiedServiceAccessRulesByServiceID, PyDataType* modifiedServiceCostModifiers,
        PyDataType* modifiedRentableItems, PyDataType* stationName, PyDataType* description,
        PyDataType* dockingCostPerVolume, PyDataType* officeRentalCost, PyDataType* reprocessingStationsTake,
        PyDataType* reprocessingHangarFlag, PyDataType* exitTime, PyDataType* standingOwnerID);
  EVEResult GetNumberOfPotentialCEOs(EVECallArgs& call);

  EVEResult CanLeaveCurrentCorporation(EVECallArgs& call);
    

protected:
    static void FillOCApplicationChange(OnCorporationApplicationChanged& OCAC, const Corp::ApplicationInfo& Old, const Corp::ApplicationInfo& New);

    bool GetSearchValues(int8 op, PyDataType* rep, std::ostringstream& query);
    uint8 GetQueryType(std::string queryType);

    CorporationDB& m_db;
    ObjCacheService* m_cache;
    LSCService* m_lsc;
    OfficeSparseBound* m_offices;

    uint32 m_corpID;
};

#endif  // __EVEMU_CORP_CORPREGISTRY_BOUND_H_
