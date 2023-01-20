
/**
  * @name AllianceBound.cpp
  *     Alliance Bound code
  *
  * @Author:        Allan
  * Updated:        James
  * @date:          16 January 2018
  */

#include <string>

#include "EVE_Corp.h"
#include "EVE_Mail.h"
#include "StaticDataMgr.h"
#include "account/AccountService.h"
#include "alliance/AllianceBound.h"
#include "alliance/AllianceRegistry.h"
#include "alliance/AllianceDB.h"
#include "alliance/AllianceData.h"
#include "cache/ObjCacheService.h"
#include "chat/LSCService.h"
#include "packets/CorporationPkts.h"
#include "corporation/CorporationDB.h"
#include "station/StationDB.h"
#include "station/StationDataMgr.h"

/*
 * ALLY__ERROR
 * ALLY__WARNING
 * ALLY__INFO
 * ALLY__MESSAGE
 * ALLY__TRACE
 * ALLY__CALL
 * ALLY__CALL_DUMP
 * ALLY__RSP_DUMP
 */

AllianceBound::AllianceBound(EVEServiceManager& mgr, AllianceRegistry& parent, AllianceDB& db, uint32 allyID)
    : EVEBoundObject(mgr, parent),
      m_db(db)
{
    this->Add("GetAlliance", &AllianceBound::GetAlliance);
    this->Add("AddBulletin", &AllianceBound::AddBulletin);
    this->Add("GetBulletins", &AllianceBound::GetBulletins);
    this->Add("DeleteBulletin", &AllianceBound::DeleteBulletin);
    this->Add("CreateLabel", &AllianceBound::CreateLabel);
    this->Add("GetLabels", &AllianceBound::GetLabels);
    this->Add("DeleteLabel", &AllianceBound::DeleteLabel);
    this->Add("EditLabel", &AllianceBound::EditLabel);
    this->Add("AssignLabels", &AllianceBound::AssignLabels);
    this->Add("RemoveLabels", &AllianceBound::RemoveLabels);
    this->Add("GetMembers", &AllianceBound::GetMembers);
    this->Add("DeclareExecutorSupport", &AllianceBound::DeclareExecutorSupport);
    this->Add("DeleteMember", &AllianceBound::DeleteMember);
    this->Add("GetApplications", &AllianceBound::GetApplications);
    this->Add("UpdateApplication", &AllianceBound::UpdateApplication);
    this->Add("AddToVoiceChat", &AllianceBound::AddToVoiceChat);
    this->Add("PayBill", &AllianceBound::PayBill);
    this->Add("GetBills", &AllianceBound::GetBills);
    this->Add("GetBillsReceivable", &AllianceBound::GetBillsReceivable);
    this->Add("GetAllianceContacts", &AllianceBound::GetAllianceContacts);
    this->Add("AddAllianceContact", &AllianceBound::AddAllianceContact);
    this->Add("EditAllianceContact", &AllianceBound::EditAllianceContact);
    this->Add("RemoveAllianceContacts", &AllianceBound::RemoveAllianceContacts);
    this->Add("EditContactsRelationshipID", &AllianceBound::EditContactsRelationshipID);
    this->Add("UpdateAlliance", &AllianceBound::UpdateAlliance);

    m_allyID = allyID;
    this->m_cache = this->GetServiceManager().Lookup <ObjCacheService>("objectCaching");
    this->m_lsc = this->GetServiceManager().Lookup <LSCService>("LSC");
}

EVEResult AllianceBound::GetAlliance(EVECallArgs&call)
{
    // Works
    //   self.members = self.GetMoniker().GetAlliance()
    _log(ALLY__CALL, "AllianceBound::Handle_GetAlliance() size=%lli", call.tuple->size());
    call.dump(ALLY__CALL_DUMP);

    // called by member of this alliance
    return m_db.GetAlliance(m_allyID);
}

EVEResult AllianceBound::GetMembers(EVECallArgs&call)
{
    // Works
    //   self.members = self.GetMoniker().GetMembers()
    _log(ALLY__CALL, "AllianceBound::Handle_GetMembers() size=%lli", call.tuple->size());
    call.dump(ALLY__CALL_DUMP);

    return m_db.GetMembers(m_allyID);
}

EVEResult AllianceBound::DeclareExecutorSupport(EVECallArgs&call, PyInt* chosenExecutor)
{
    //   self.GetMoniker().DeclareExecutorSupport(corpID)
    _log(ALLY__CALL, "AllianceBound::Handle_DeclareExecutorSupport() size=%lli", call.tuple->size());
    call.dump(ALLY__CALL_DUMP);

    m_db.DeclareExecutorSupport(call.client->GetCorporationID(), chosenExecutor->value());

    return nullptr;
}

EVEResult AllianceBound::DeleteMember(EVECallArgs&call, PyInt* corporationID)
{
    //  self.GetMoniker().DeleteMember(corpID)
    _log(ALLY__CALL, "AllianceBound::Handle_DeleteMember() size=%lli", call.tuple->size());
    call.dump(ALLY__CALL_DUMP);

    m_db.DeleteMember(m_allyID, corporationID->value());

    return nullptr;
}

EVEResult AllianceBound::GetApplications(EVECallArgs&call)
{
    //   self.applications = self.GetMoniker().GetApplications()
    // Get all applications to our alliance
    _log(ALLY__CALL, "AllianceBound::Handle_GetApplications() size=%lli", call.tuple->size());
    call.dump(ALLY__CALL_DUMP);

    return m_db.GetApplications(m_allyID);
}

EVEResult AllianceBound::UpdateApplication(EVECallArgs&call, PyInt* corporationID, PyString* applicationText, PyInt* state)
{
    //    return self.GetMoniker().UpdateApplication(corpID, applicationText, state)
    _log(ALLY__CALL, "AllianceBound::Handle_UpdateApplication() size=%lli", call.tuple->size());
    call.dump(ALLY__CALL_DUMP);

    //We don't delay joining an alliance so we will simply change AppAccepted to AppEffective
    if (state->value() == EveAlliance::AppStatus::AppAccepted)
    {
        state = new PyInt (EveAlliance::AppStatus::AppEffective);
    }

    //Old application info
    Alliance::ApplicationInfo oldInfo = Alliance::ApplicationInfo();

    if (!m_db.GetCurrentApplicationInfo(m_allyID, corporationID->value(), oldInfo))
    {
        _log(SERVICE__ERROR, "%s: Failed to query application for corp %u alliance %u", call.client->GetName(), corporationID->value(), m_allyID);
        return nullptr;
    }

    //New application info
    Alliance::ApplicationInfo newInfo = oldInfo;
    newInfo.valid = true;
    newInfo.state = state->value();
    if (!m_db.UpdateApplication(newInfo))
    {
        _log(SERVICE__ERROR, "%s: Failed to update application for corp %u alliance %u", call.client->GetName(), corporationID->value(), m_allyID);
        return nullptr;
    }

    OnAllianceApplicationChanged oaac;
    oaac.allianceID = newInfo.allyID;
    oaac.corpID = newInfo.corpID;
    FillOAApplicationChange(oaac, oldInfo, newInfo);

    //Client list for notifications
    std::vector<Client *> list;

    //If we are accepting an application
    if (state->value() == EveAlliance::AppStatus::AppEffective)
    {
        // OnAllianceMemberChanged event
        OnAllianceMemberChange oamc;
        oamc.allyID = newInfo.allyID;
        oamc.corpID = newInfo.corpID;
        FillOAMemberChange(oamc, oldInfo, newInfo);

        //Send to everyone who needs to see it in the applying corp and in the alliance executor corp
        uint32 executorID = AllianceDB::GetExecutorID(newInfo.allyID);

        list.clear();
        sEntityList.GetCorpClients(list, oamc.corpID);
        for (auto cur : list)
        {
            if (cur != nullptr)
            {
                cur->SendNotification("OnAllianceMemberChanged", "clientID", oamc.Encode(), false);
                _log(ALLY__TRACE, "OnAllianceMemberChanged sent to %s (%u)", cur->GetName(), cur->GetCharID());
            }
        }

        list.clear();
        sEntityList.GetCorpClients(list, executorID);
        for (auto cur : list)
        {
            if (cur != nullptr)
            {
                cur->SendNotification("OnAllianceMemberChanged", "clientID", oamc.Encode(), false);
                _log(ALLY__TRACE, "OnAllianceMemberChanged sent to %s (%u)", cur->GetName(), cur->GetCharID());
            }
        }

        //  call to db.AddCorporation() will update eveStaticOwners with new corp
        PyString *cache_name = new PyString("config.StaticOwners");
        this->m_cache->InvalidateCache(cache_name);
        PySafeDecRef(cache_name);

        // join corporation to alliance
        if (!m_db.UpdateCorpAlliance(m_allyID, corporationID->value()))
        {
            codelog(SERVICE__ERROR, "Alliance join failed.");
            return nullptr;
        }

        // Add alliance membership record to corporation
        if (!m_db.AddEmployment(m_allyID, corporationID->value()))
        {
            codelog(SERVICE__ERROR, "Add corp employment failed.");
            return nullptr;
        }

        // Send an evemail to the corporation in question that the application has been accepted
        std::string subject = "Application to alliance accepted";
        std::string message = "Your alliance application has been accepted.";
        subject += call.client->GetName();
        std::vector<int32> recipients;
        recipients.push_back(CorporationDB::GetCorporationCEO(corporationID->value()));
        this->m_lsc->SendMail(CorporationDB::GetCorporationCEO(m_db.GetExecutorID(m_allyID)), recipients, subject, message);

        //Send OnAllianceChanged packet
        OnAllianceChanged ac;
        ac.allianceID = m_allyID;
        if (!m_db.CreateAllianceChangePacket(ac, 0, m_allyID))
        {
            codelog(SERVICE__ERROR, "Failed to create OnAllianceChanged notification stream.");
            call.client->SendErrorMsg("Unable to notify about alliance change.");
            return nullptr;
        }

        //Send to everyone who needs to see it in the applying corp
        list.clear();
        sEntityList.GetCorpClients(list, corporationID->value());
        for (auto cur : list)
        {
            if (cur != nullptr)
            {
                cur->SendNotification("OnAllianceChanged", "clientID", ac.Encode(), false);
                _log(ALLY__TRACE, "OnAllianceChanged sent to %s (%u)", cur->GetName(), cur->GetCharID());
            }
        }
    }

    else if (state->value() == EveAlliance::AppStatus::AppRejected)
    {
        _log(ALLY__TRACE, "Application of %u:%u has been rejected, sending notification...", m_allyID, corporationID->value());
    }

    else
    {
        _log(SERVICE__ERROR, "%s: Sent unhandled status %u ", call.client->GetName(), state->value());
    }

    //Send to everyone who needs to see it in the applying corp and in the alliance executor corp
    uint32 executorID = AllianceDB::GetExecutorID(oaac.allianceID);

    list.clear();
    sEntityList.GetCorpClients(list, oaac.corpID);
    for (auto cur : list)
    {
        if (cur != nullptr)
        {
            cur->SendNotification("OnAllianceApplicationChanged", "clientID", oaac.Encode(), false);
            _log(ALLY__TRACE, "OnAllianceApplicationChanged sent to %s (%u)", cur->GetName(), cur->GetCharID());
        }
    }

    list.clear();
    sEntityList.GetCorpClients(list, executorID);
    for (auto cur : list)
    {
        if (cur != nullptr)
        {
            cur->SendNotification("OnAllianceApplicationChanged", "clientID", oaac.Encode(), false);
            _log(ALLY__TRACE, "OnAllianceApplicationChanged sent to %s (%u)", cur->GetName(), cur->GetCharID());
        }
    }

    return nullptr;
}

void AllianceBound::FillOAApplicationChange(OnAllianceApplicationChanged &OAAC, const Alliance::ApplicationInfo &Old, const Alliance::ApplicationInfo &New)
{
    if (Old.valid)
    {
        OAAC.applicationIDOld = new PyInt(Old.appID);
        OAAC.applicationTextOld = new PyString(Old.appText);
        OAAC.corporationIDOld = new PyInt(Old.corpID);
        OAAC.allianceIDOld = new PyInt(Old.allyID);
        OAAC.stateOld = new PyInt(Old.state);
        OAAC.applicationDateTimeOld = new PyInt(Old.appTime);
        if (Old.deleted == true)
        {
            OAAC.deletedOld = new PyBool(true);
        }
    }
    if (New.valid)
    {
        OAAC.applicationIDNew = new PyInt(New.appID);
        OAAC.applicationTextNew = new PyString(New.appText);
        OAAC.corporationIDNew = new PyInt(New.corpID);
        OAAC.allianceIDNew = new PyInt(New.allyID);
        OAAC.stateNew = new PyInt(New.state);
        OAAC.applicationDateTimeNew = new PyInt(New.appTime);
        if (New.deleted == true)
        {
            OAAC.deletedNew = new PyBool(true);
        }
    }
}

void AllianceBound::FillOAMemberChange(OnAllianceMemberChange &oamc, const Alliance::ApplicationInfo &Old, const Alliance::ApplicationInfo &New)
{
    if (Old.valid)
    {
        oamc.corporationIDOld = new PyInt(Old.corpID);
        oamc.corporationNameOld = new PyString(CorporationDB::GetCorpName(Old.corpID));
        oamc.chosenExecutorIDOld = new PyInt(AllianceDB::GetExecutorID(Old.allyID));
    }
    if (New.valid)
    {
        oamc.corporationIDNew = new PyInt(New.corpID);
        oamc.corporationNameNew = new PyString(CorporationDB::GetCorpName(New.corpID));
        oamc.chosenExecutorIDNew = new PyInt(AllianceDB::GetExecutorID(New.allyID));
    }
}

EVEResult AllianceBound::AddToVoiceChat(EVECallArgs&call, PyString* channelName)
{
    //    success = moniker.GetAlliance().AddToVoiceChat(vivoxChannelName)
    _log(ALLY__CALL, "AllianceBound::Handle_AddToVoiceChat() size=%lli", call.tuple->size());
    call.dump(ALLY__CALL_DUMP);
    return nullptr;
}

EVEResult AllianceBound::PayBill(EVECallArgs&call, PyInt* billID, PyInt* fromAccountKey)
{
    //   return self.GetMoniker().PayBill(billID, fromAccountKey)
    _log(ALLY__CALL, "AllianceBound::Handle_PayBill() size=%lli", call.tuple->size());
    call.dump(ALLY__CALL_DUMP);

    return nullptr;
}

EVEResult AllianceBound::GetBillBalance(EVECallArgs&call, PyInt* billID)
{
    //   return self.GetMoniker().GetBillBalance(billID)
    _log(ALLY__CALL, "AllianceBound::Handle_GetBillBalance() size=%lli", call.tuple->size());
    call.dump(ALLY__CALL_DUMP);

    return nullptr;
}

EVEResult AllianceBound::GetBills(EVECallArgs&call)
{
    //   return self.GetMoniker().GetBills()
    _log(ALLY__CALL, "AllianceBound::Handle_GetBills() size=%lli", call.tuple->size());
    call.dump(ALLY__CALL_DUMP);

    return nullptr;
}

EVEResult AllianceBound::GetBillsReceivable(EVECallArgs&call)
{
    //   return self.GetMoniker().GetBillsReceivable()
    _log(ALLY__CALL, "AllianceBound::Handle_GetBillsReceivable() size=%lli", call.tuple->size());
    call.dump(ALLY__CALL_DUMP);

    return nullptr;
}

EVEResult AllianceBound::AddBulletin(EVECallArgs&call, PyString* title, PyString* body)
{
    //   sm.GetService('alliance').GetMoniker().AddBulletin(title, body)
    //  sm.GetService('alliance').GetMoniker().AddBulletin(title, body, bulletinID=bulletinID, editDateTime=editDateTime)  <-- this is to update bulletin
    _log(ALLY__CALL, "AllianceBound::Handle_AddBulletin() size=%lli", call.tuple->size());
    call.dump(ALLY__CALL_DUMP);

    m_db.AddBulletin(m_allyID, call.client->GetCorporationID(), call.client->GetCharacterID(), title->content(), body->content());

    return nullptr;
}

EVEResult AllianceBound::DeleteBulletin(EVECallArgs&call, PyInt* id)
{
    //   sm.GetService('alliance').GetMoniker().DeleteBulletin(id)
    _log(ALLY__CALL, "AllianceBound::Handle_DeleteBulletin() size=%lli", call.tuple->size());
    call.dump(ALLY__CALL_DUMP);

    m_db.DeleteBulletin(id->value());
    return nullptr;
}

EVEResult AllianceBound::GetBulletins(EVECallArgs&call)
{
    //   self.bulletins = self.GetMoniker().GetBulletins()
    _log(ALLY__CALL, "AllianceBound::Handle_GetBulletins() size=%lli", call.tuple->size());
    call.dump(ALLY__CALL_DUMP);
    return m_db.GetBulletins(m_allyID);
}

EVEResult AllianceBound::GetAllianceContacts(EVECallArgs&call)
{
    //    return self.GetMoniker().GetAllianceContacts()
    _log(ALLY__CALL, "AllianceBound::Handle_GetAllianceContacts() size=%lli", call.tuple->size());
    call.dump(ALLY__CALL_DUMP);

    return m_db.GetContacts(m_allyID);
}

EVEResult AllianceBound::AddAllianceContact(EVECallArgs&call, PyInt* contactID, PyInt* relationshipID)
{
    //   self.GetMoniker().AddAllianceContact(contactID, relationshipID)
    _log(ALLY__CALL, "AllianceBound::Handle_AddAllianceContact() size=%lli", call.tuple->size());
    call.dump(ALLY__CALL_DUMP);

    m_db.AddContact(m_allyID, contactID->value(), relationshipID->value());

    return nullptr;
}

EVEResult AllianceBound::EditAllianceContact(EVECallArgs&call, PyInt* contactID, PyInt* relationshipID)
{
    //   self.GetMoniker().EditAllianceContact(contactID, relationshipID)
    _log(ALLY__CALL, "AllianceBound::Handle_EditAllianceContact() size=%lli", call.tuple->size());
    call.dump(ALLY__CALL_DUMP);

    m_db.UpdateContact(relationshipID->value(), contactID->value(), m_allyID);

    return nullptr;
}

EVEResult AllianceBound::RemoveAllianceContacts(EVECallArgs&call, PyList* contactIDs)
{
    //   self.GetMoniker().RemoveAllianceContacts(contactIDs)
    _log(ALLY__CALL, "AllianceBound::Handle_RemoveAllianceContacts() size=%lli", call.tuple->size());
    call.dump(ALLY__CALL_DUMP);

    for (PyList::const_iterator itr = contactIDs->begin(); itr != contactIDs->end(); ++itr)
    {
        m_db.RemoveContact((*itr)->u32(), m_allyID);
    }

    return nullptr;
}

EVEResult AllianceBound::EditContactsRelationshipID(EVECallArgs&call, PyList* contactIDs, PyInt* relationshipID)
{
    //    self.GetMoniker().EditContactsRelationshipID(contactIDs, relationshipID)
    _log(ALLY__CALL, "AllianceBound::Handle_EditContactsRelationshipID() size=%lli", call.tuple->size());
    call.dump(ALLY__CALL_DUMP);

    for (PyList::const_iterator itr = contactIDs->begin(); itr != contactIDs->end(); ++itr)
    {
        m_db.UpdateContact(relationshipID->value(), (*itr)->u32(), m_allyID);
    }

    return nullptr;
}

EVEResult AllianceBound::GetLabels(EVECallArgs&call)
{
    //   return self.GetMoniker().GetLabels()
    _log(ALLY__CALL, "AllianceBound::Handle_GetLabels() size=%lli", call.tuple->size());
    call.dump(ALLY__CALL_DUMP);
    return nullptr;
}

EVEResult AllianceBound::CreateLabel(EVECallArgs&call, PyString* name, std::optional<PyInt*> color)
{
    //   return self.GetMoniker().CreateLabel(name, color)
    _log(ALLY__CALL, "AllianceBound::Handle_CreateLabel() size=%lli", call.tuple->size());
    call.dump(ALLY__CALL_DUMP);
    return nullptr;
}

EVEResult AllianceBound::DeleteLabel(EVECallArgs&call, PyInt* labelID)
{
    //   self.GetMoniker().DeleteLabel(labelID)
    _log(ALLY__CALL, "AllianceBound::Handle_DeleteLabel() size=%lli", call.tuple->size());
    call.dump(ALLY__CALL_DUMP);
    return nullptr;
}

EVEResult AllianceBound::EditLabel(EVECallArgs&call, PyInt* labelID, PyString* name, std::optional<PyInt*> color)
{
    //   self.GetMoniker().EditLabel(labelID, name, color)
    _log(ALLY__CALL, "AllianceBound::Handle_EditLabel() size=%lli", call.tuple->size());
    call.dump(ALLY__CALL_DUMP);
    return nullptr;
}

EVEResult AllianceBound::AssignLabels(EVECallArgs&call, PyList* contactIDs, PyInt* labelMask)
{
    //   self.GetMoniker().AssignLabels(contactIDs, labelMask)
    _log(ALLY__CALL, "AllianceBound::Handle_AssignLabels() size=%lli", call.tuple->size());
    call.dump(ALLY__CALL_DUMP);
    return nullptr;
}

EVEResult AllianceBound::RemoveLabels(EVECallArgs&call, PyList* contactIDs, PyInt* labelMask)
{
    //   self.GetMoniker().RemoveLabels(contactIDs, labelMask)
    _log(ALLY__CALL, "AllianceBound::Handle_RemoveLabels() size=%lli", call.tuple->size());
    call.dump(ALLY__CALL_DUMP);
    return nullptr;
}

EVEResult AllianceBound::UpdateAlliance(EVECallArgs&call, PyString* description, PyString* url)
{
    //    return self.GetMoniker().UpdateAlliance(description, url)
    _log(ALLY__CALL, "AllianceBound::Handle_UpdateAlliance() size=%lli", call.tuple->size());
    call.dump(ALLY__CALL_DUMP);

    m_db.UpdateAlliance(m_allyID, description->content(), url->content());

    //Send OnAllianceChanged packet
    OnAllianceChanged ac;
    ac.allianceID = m_allyID;
    if (!m_db.CreateAllianceChangePacket(ac, 0, m_allyID))
    {
        codelog(SERVICE__ERROR, "Failed to create OnAllianceChanged notification stream.");
        call.client->SendErrorMsg("Unable to notify about alliance change.");
        return nullptr;
    }

    //Send notification to the client about the alliance change which just occurred
    call.client->SendNotification("OnAllianceChanged", "clientID", ac.Encode(), false);
    _log(SOV__DEBUG, "OnAllianceChanged sent to %s(%u)", call.client->GetName(), call.client->GetCharID());

    return nullptr;
}
