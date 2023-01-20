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
    Author:        Luck, caytchen
*/

#include "eve-server.h"


#include "mail/MailingListMgrService.h"

MailingListMgrService::MailingListMgrService() :
    Service("mailingListsMgr", eAccessLevel_Character)
{
    this->Add("GetJoinedLists", &MailingListMgrService::GetJoinedLists);
    this->Add("Create", &MailingListMgrService::Create);
    this->Add("Join", &MailingListMgrService::Join);
    this->Add("Leave", &MailingListMgrService::Leave);
    this->Add("Delete", &MailingListMgrService::Delete);
    this->Add("KickMembers", &MailingListMgrService::KickMembers);
    this->Add("GetMembers", &MailingListMgrService::GetMembers);
    this->Add("SetEntityAccess", &MailingListMgrService::SetEntityAccess);
    this->Add("ClearEntityAccess", &MailingListMgrService::ClearEntityAccess);
    this->Add("SetMembersMuted", &MailingListMgrService::SetMembersMuted);
    this->Add("SetMembersOperator", &MailingListMgrService::SetMembersOperator);
    this->Add("SetMembersClear", &MailingListMgrService::SetMembersClear);
    this->Add("SetDefaultAccess", &MailingListMgrService::SetDefaultAccess);
    this->Add("GetInfo", &MailingListMgrService::GetInfo);
    this->Add("GetSettings", &MailingListMgrService::GetSettings);
    this->Add("GetWelcomeMail", &MailingListMgrService::GetWelcomeMail);
    this->Add("SaveWelcomeMail", &MailingListMgrService::SaveWelcomeMail);
    this->Add("SendWelcomeMail", &MailingListMgrService::SendWelcomeMail);
    this->Add("ClearWelcomeMail", &MailingListMgrService::ClearWelcomeMail);
}

EVEResult MailingListMgrService::GetJoinedLists(EVECallArgs& call)
{
    // @TODO: Test
    // no args
    sLog.Debug("MailingListMgrService", "Called GetJoinedLists stub" );

    return m_db.GetJoinedMailingLists(call.client->GetCharacterID());
}

EVEResult MailingListMgrService::Create(EVECallArgs& call, PyString* name, PyInt* defaultAccess, PyInt* defaultMemberAccess, std::optional<PyInt*> mailCost)
{
    // @TODO: Test
    sLog.Debug("MailingListMgrService", "Called Create stub" );
    uint32 r = m_db.CreateMailingList(call.client->GetCharacterID(), name->content(), defaultAccess->value(),
                                   defaultMemberAccess->value(), mailCost.has_value() ? mailCost.value()->value() : 0);
    if (r >= 0) {
        return new PyInt(r);
    }
    return nullptr;
}

EVEResult MailingListMgrService::Join(EVECallArgs& call, PyDataType* listName)
{
    // @TODO: Stub
    sLog.Debug("MailingListMgrService", "Called Join stub" );

    std::string listNameStr = listName->string();
    // returns mailing list object
    return nullptr;
}

EVEResult MailingListMgrService::Leave(EVECallArgs& call, PyInt* listID)
{
    // @TODO: Stub
    sLog.Debug("MailingListMgrService", "Called Leave stub" );
    // no return values
    return nullptr;
}

EVEResult MailingListMgrService::Delete(EVECallArgs& call, PyInt* listID)
{
    // @TODO: Stub
    sLog.Debug("MailingListMgrService", "Called Delete stub" );
    // no return values
    return nullptr;
}

EVEResult MailingListMgrService::KickMembers(EVECallArgs& call, PyInt* listID, PyList* memberIDs)
{
    // @TODO: Stub
    sLog.Debug("MailingListMgrService", "Called KickMembers stub" );
    for (int i = 0; i < memberIDs->size(); i++) {
        PyDataType *member = memberIDs->at (i);
        member->dump(SERVICE__ERROR, "member item");
    }

    // no return values
    return nullptr;
}

EVEResult MailingListMgrService::GetMembers(EVECallArgs& call, PyInt* listID)
{
    // @TODO: Stub
    sLog.Debug("MailingListMgrService", "Called GetMembers stub" );
    return m_db.GetMailingListMembers(listID->value());
}

EVEResult MailingListMgrService::SetEntityAccess(EVECallArgs& call, PyInt* listID, PyInt* entityID, PyInt* access)
{
    // @TODO: Stub
    sLog.Debug("MailingListMgrService", "Called SetEntityAccess stub" );

    // no return values
    return nullptr;
}

EVEResult MailingListMgrService::ClearEntityAccess(EVECallArgs& call, PyInt* listID, PyInt* entityID)
{
    // @TODO: Stub
    sLog.Debug("MailingListMgrService", "Called ClearEntityAccess stub" );
    // no return values
    return nullptr;
}

EVEResult MailingListMgrService::SetMembersMuted(EVECallArgs& call, PyInt* listID, PyList* memberIDs)
{
    // @TODO: Stub
    sLog.Debug("MailingListMgrService", "Called SetMembersMuted stub" );
    return nullptr;
}

EVEResult MailingListMgrService::SetMembersOperator(EVECallArgs& call, PyInt* listID, PyList* memberIDs)
{
    // @TODO: Stub
    sLog.Debug("MailingListMgrService", "Called SetMembersOperator stub" );
    return nullptr;
}

EVEResult MailingListMgrService::SetMembersClear(EVECallArgs& call, PyInt* listID, PyList* memberIDs)
{
    // @TODO: Stub
    sLog.Debug("MailingListMgrService", "Called SetMembersClear stub" );
    return nullptr;
}

EVEResult MailingListMgrService::SetDefaultAccess(EVECallArgs& call, PyInt* listID, PyInt* defaultAccess, PyInt* defaultMemberAccess, std::optional<PyInt*> mailCost)
{
    // @TODO: Stub
    sLog.Debug("MailingListMgrService", "Called SetDefaultAccess stub" );
    m_db.SetMailingListDefaultAccess(listID->value(), defaultAccess->value(),
                                      defaultMemberAccess->value(), mailCost.has_value() ? mailCost.value()->value() : 0);

    return nullptr;
}

EVEResult MailingListMgrService::GetInfo(EVECallArgs& call, PyInt* listID)
{
    // @TODO: Stub
    sLog.Debug("MailingListMgrService", "Called GetInfo stub" );

    if (listID->value() == 0) {
        return new PyObject("util.KeyVal", new PyDict {
            {"displayName", new PyString("Test")}
        });
    }
    return nullptr;
}

EVEResult MailingListMgrService::GetSettings(EVECallArgs& call, PyInt* listID)
{
    // @TODO: Test
    sLog.Debug("MailingListMgrService", "Called GetSettings stub" );
    // return:
    // .access: list (ownerID, accessLevel)
    // .defaultAccess
    // .defaultMemberAccess

    return m_db.MailingListGetSettings(listID->value());
}

EVEResult MailingListMgrService::GetWelcomeMail(EVECallArgs& call, PyInt* listID)
{
    // @TODO: Stub
    sLog.Debug("MailingListMgrService", "Called GetWelcomeMail stub" );
    return nullptr;
}

EVEResult MailingListMgrService::SaveWelcomeMail(EVECallArgs& call, PyInt* listID, PyString* title, PyString* body)
{
    // @TODO: Stub
    sLog.Debug("MailingListMgrService", "Called SaveWelcomeMail stub" );

    return nullptr;
}

EVEResult MailingListMgrService::SendWelcomeMail(EVECallArgs& call, PyInt* listID, PyString* title, PyString* body)
{
    // @TODO: Stub
    sLog.Debug("MailingListMgrService", "Called SendWelcomeMail stub" );
    return nullptr;
}

EVEResult MailingListMgrService::ClearWelcomeMail(EVECallArgs& call, PyInt* listID)
{
    // @TODO: Stub
    sLog.Debug("MailingListMgrService", "Called ClearWelcomeMail stub" );

    // no return value
    return nullptr;
}
