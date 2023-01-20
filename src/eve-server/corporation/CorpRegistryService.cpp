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
    Author:     Zhur, Allan
*/

#include "eve-server.h"

#include "config/EVEServerConfig.h"


#include "cache/ObjCacheService.h"
#include "corporation/CorpRegistryService.h"
#include "corporation/CorpRegistryBound.h"

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

CorpRegistryService::CorpRegistryService(EVEServiceManager& mgr) :
    BindableService("corpRegistry", mgr)
{
    /** @note: all of these are skeleton code only */
    this->Add("CreateAlliance", &CorpRegistryService::CreateAlliance);
    this->Add("GetRecentKillsAndLosses", &CorpRegistryService::GetRecentKillsAndLosses);
    this->Add("GetCorporateContacts", &CorpRegistryService::GetCorporateContacts);
    this->Add("AddCorporateContact", &CorpRegistryService::AddCorporateContact);
    this->Add("EditCorporateContact", &CorpRegistryService::EditCorporateContact);
    this->Add("RemoveCorporateContacts", &CorpRegistryService::RemoveCorporateContacts);
    this->Add("EditContactsRelationshipID", &CorpRegistryService::EditContactsRelationshipID);
    this->Add("GetLabels", &CorpRegistryService::GetLabels);
    this->Add("CreateLabel", &CorpRegistryService::CreateLabel);
    this->Add("DeleteLabel", &CorpRegistryService::DeleteLabel);
    this->Add("EditLabel", &CorpRegistryService::EditLabel);
    this->Add("AssignLabels", &CorpRegistryService::AssignLabels);
    this->Add("RemoveLabels", &CorpRegistryService::RemoveLabels);
    this->Add("ResignFromCEO", &CorpRegistryService::ResignFromCEO);
}

BoundDispatcher* CorpRegistryService::BindObject(Client* client, PyDataType* bindParameters)
{
    if (!bindParameters->is<PyTuple>()){
        sLog.Error( "CorpRegistryService::CreateBoundObject", "%s: bind_args is not tuple: '%s'. ", client->GetName(), bindParameters->TypeString() );
        client->SendErrorMsg("Could not bind object for Corp Registry.  Ref: ServerError 02808.");
        return nullptr;
    }

    uint32 corporationID = bindParameters->as<PyTuple>()->at (0)->i64();
    auto it = this->m_instances.find (corporationID);

    if (it != this->m_instances.end ())
        return it->second;

    CorpRegistryBound* bound = new CorpRegistryBound(this->GetServiceManager(), *this, m_db, corporationID);

    this->m_instances.insert_or_assign (corporationID, bound);

    return bound;
}

void CorpRegistryService::BoundReleased (CorpRegistryBound* bound) {
    auto it = this->m_instances.find (bound->GetCorporationID());

    if (it == this->m_instances.end ())
        return;

    this->m_instances.erase (it);
}

EVEResult CorpRegistryService::GetCorporateContacts(EVECallArgs&call)
{
    return m_db.GetContacts(call.client->GetCorporationID());
}

/**     ***********************************************************************
 * @note   these below are partially coded
 */



/**     ***********************************************************************
 * @note   these do absolutely nothing at this time....
 */

EVEResult CorpRegistryService::ResignFromCEO(EVECallArgs&call, PyInt* newCeoID) {
    //    self.GetCorpRegistry().ResignFromCEO(newCeoID)
    _log(CORP__CALL, "CorpRegistryService::Handle_ResignFromCEO()");
    call.dump(CORP__CALL_DUMP);

    return nullptr;
}

EVEResult CorpRegistryService::CreateAlliance(EVECallArgs&call, PyDataType* allianceName, PyDataType* shortName, PyDataType* description, PyDataType* url) {
    _log(CORP__CALL, "CorpRegistryService::Handle_CreateAlliance()");
    call.dump(CORP__CALL_DUMP);

    return nullptr;
}

EVEResult CorpRegistryService::GetRecentKillsAndLosses(EVECallArgs&call) {
    _log(CORP__CALL, "CorpRegistryService::Handle_GetRecentKillsAndLosses()");
    call.dump(CORP__CALL_DUMP);

    return nullptr;
}

EVEResult CorpRegistryService::AddCorporateContact(EVECallArgs&call, PyInt* contactID, PyInt* relationshipID) {
 /*    def AddCorporateContact(self, contactID, relationshipID):
  *        self.GetCorpRegistry().AddCorporateContact(contactID, relationshipID)
  */
    _log(CORP__CALL, "CorpRegistryService::Handle_AddCorporateContact()");
    call.dump(CORP__CALL_DUMP);

    return nullptr;
}

EVEResult CorpRegistryService::EditCorporateContact(EVECallArgs&call, PyInt* contactID, PyInt* relationshipID) {
 /*    def EditCorporateContact(self, contactID, relationshipID):
  *        self.GetCorpRegistry().EditCorporateContact(contactID, relationshipID)
  */
    _log(CORP__CALL, "CorpRegistryService::Handle_EditCorporateContact)");
    call.dump(CORP__CALL_DUMP);

    return nullptr;
}

EVEResult CorpRegistryService::RemoveCorporateContacts(EVECallArgs&call, PyList* contactIDs) {
 /*    def RemoveCorporateContacts(self, contactIDs):
  *        self.GetCorpRegistry().RemoveCorporateContacts(contactIDs)
  */
    _log(CORP__CALL, "CorpRegistryService::Handle_RemoveCorporateContacts()");
    call.dump(CORP__CALL_DUMP);

    return nullptr;
}

EVEResult CorpRegistryService::EditContactsRelationshipID(EVECallArgs&call, PyList* contactIDs, PyInt* relationshipID) {
 /*    def EditContactsRelationshipID(self, contactIDs, relationshipID):
  *        self.GetCorpRegistry().EditContactsRelationshipID(contactIDs, relationshipID)
  */
    _log(CORP__CALL, "CorpRegistryService::Handle_EditContactsRelationshipID()");
    call.dump(CORP__CALL_DUMP);

    return nullptr;
}

EVEResult CorpRegistryService::GetLabels(EVECallArgs&call) {
    _log(CORP__CALL, "CorpRegistryService::Handle_GetLabels()");
    call.dump(CORP__CALL_DUMP);

    return m_db.GetLabels(call.client->GetCorporationID());
}

EVEResult CorpRegistryService::CreateLabel(EVECallArgs&call, PyString* name, std::optional <PyInt*> color) {
 /*    def CreateLabel(self, name, color = 0):
  *        return self.GetCorpRegistry().CreateLabel(name, color)
  */
    _log(CORP__CALL, "CorpRegistryService::Handle_CreateLabel()");
    call.dump(CORP__CALL_DUMP);

    return nullptr;
}

EVEResult CorpRegistryService::DeleteLabel(EVECallArgs&call, PyInt* labelID) {
 /*    def DeleteLabel(self, labelID):
  *        self.GetCorpRegistry().DeleteLabel(labelID)
  */
    _log(CORP__CALL, "CorpRegistryService::Handle_DeleteLabel()");
    call.dump(CORP__CALL_DUMP);

    return nullptr;
}

EVEResult CorpRegistryService::EditLabel(EVECallArgs&call, PyInt* labelID, std::optional <PyString*> name, std::optional <PyInt*> color) {
 /*    def EditLabel(self, labelID, name = None, color = None):
  *        self.GetCorpRegistry().EditLabel(labelID, name, color)
  */
    _log(CORP__CALL, "CorpRegistryService::Handle_EditLabel()");
    call.dump(CORP__CALL_DUMP);

    return nullptr;
}

EVEResult CorpRegistryService::AssignLabels(EVECallArgs&call, PyList* contactIDs, PyInt* labelMask) {
 /*    def AssignLabels(self, contactIDs, labelMask):
  *        self.GetCorpRegistry().AssignLabels(contactIDs, labelMask)
  */
    _log(CORP__CALL, "CorpRegistryService::Handle_AssignLabels()");
    call.dump(CORP__CALL_DUMP);

    return nullptr;
}

EVEResult CorpRegistryService::RemoveLabels(EVECallArgs&call, PyList* contactIDs, PyInt* labelMask) {
/*    def RemoveLabels(self, contactIDs, labelMask):
 *        self.GetCorpRegistry().RemoveLabels(contactIDs, labelMask)
 */
    _log(CORP__CALL, "CorpRegistryService::Handle_RemoveLabels()");
    call.dump(CORP__CALL_DUMP);


    return nullptr;
}
