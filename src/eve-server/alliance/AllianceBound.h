
 /**
  * @name AllianceBound.h
  *     Alliance Bound code
  *
  * @Author:        Allan
  * Updated:        James
  * @date:          16 January 2018
  */



#ifndef EVE_ALLIANCE_ALLYREGISTRY_BOUND_H_
#define EVE_ALLIANCE_ALLYREGISTRY_BOUND_H_


#include "../eve-server.h"
#include "services/BoundService.h"
#include "alliance/AllianceDB.h"
#include "cache/ObjCacheService.h"
#include "chat/LSCService.h"
#include "Client.h"

class AllianceRegistry;

class AllianceBound : public EVEBoundObject <AllianceBound>
{
public:
    AllianceBound(EVEServiceManager& mgr, AllianceRegistry& parent, AllianceDB& db, uint32 allyID);

    static void FillOAApplicationChange(OnAllianceApplicationChanged& OAAC, const Alliance::ApplicationInfo& Old, const Alliance::ApplicationInfo& New);
    static void FillOAMemberChange(OnAllianceMemberChange& OAMC, const Alliance::ApplicationInfo& Old, const Alliance::ApplicationInfo& New);

    uint32 GetAllianceID () const { return this->m_allyID; }
protected:
  EVEResult CreateLabel(EVECallArgs& call, PyString* name, std::optional<PyInt*> color);
  EVEResult GetLabels(EVECallArgs& call);
  EVEResult DeleteLabel(EVECallArgs& call, PyInt* labelID);
  EVEResult EditLabel(EVECallArgs& call, PyInt* labelID, PyString* name, std::optional<PyInt*> color);
  EVEResult AssignLabels(EVECallArgs& call, PyList* contactIDs, PyInt* labelMask);
  EVEResult RemoveLabels(EVECallArgs& call, PyList* contactIDs, PyInt* labelMask);

  EVEResult AddBulletin(EVECallArgs& call, PyString* title, PyString* body);
  EVEResult GetBulletins(EVECallArgs& call);
  EVEResult DeleteBulletin(EVECallArgs& call, PyInt* id);

  EVEResult GetMembers(EVECallArgs& call);
  EVEResult DeclareExecutorSupport(EVECallArgs& call, PyInt* chosenExecutor);
  EVEResult DeleteMember(EVECallArgs& call, PyInt* corporationID);
  EVEResult GetApplications(EVECallArgs& call);
  EVEResult UpdateApplication(EVECallArgs& call, PyInt* corporationID, PyString* applicationText, PyInt* state);
  EVEResult AddToVoiceChat(EVECallArgs& call, PyString* channelName);
  EVEResult PayBill(EVECallArgs& call, PyInt* billID, PyInt* fromAccountKey);
  EVEResult GetBillBalance(EVECallArgs& call, PyInt* billID);
  EVEResult GetBills(EVECallArgs& call);
  EVEResult GetBillsReceivable(EVECallArgs& call);
  EVEResult GetAllianceContacts(EVECallArgs& call);
  EVEResult AddAllianceContact(EVECallArgs& call, PyInt* contactID, PyInt* relationshipID);
  EVEResult EditAllianceContact(EVECallArgs& call, PyInt* contactID, PyInt* relationshipID);
  EVEResult RemoveAllianceContacts(EVECallArgs& call, PyList* contactIDs);
  EVEResult EditContactsRelationshipID(EVECallArgs& call, PyList* contactIDs, PyInt* relationshipID);
  EVEResult GetAlliance(EVECallArgs& call);
  EVEResult UpdateAlliance(EVECallArgs& call, PyString* description, PyString* url);

    AllianceDB& m_db;
    ObjCacheService* m_cache;
    LSCService* m_lsc;

    uint32 m_allyID;
};

#endif  // EVE_ALLIANCE_ALLYREGISTRY_BOUND_H_
