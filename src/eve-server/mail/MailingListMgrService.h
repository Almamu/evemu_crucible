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

#ifndef MAILING_LIST_MGR_SERVICE_H
#define MAILING_LIST_MGR_SERVICE_H

#include "services/Service.h"
#include "mail/MailDB.h"
#include "Client.h"

class MailingListMgrService : public Service <MailingListMgrService>
{
public:
    MailingListMgrService();

protected:
    MailDB m_db;

    EVEResult GetJoinedLists(EVECallArgs& call);
    EVEResult Create(EVECallArgs& call, PyString* name, PyInt* defaultAccess, PyInt* defaultMemberAccess, std::optional<PyInt*> mailCost);
    EVEResult Join(EVECallArgs& call, PyDataType* listName);
    EVEResult Leave(EVECallArgs& call, PyInt* listID);
    EVEResult Delete(EVECallArgs& call, PyInt* listID);
    EVEResult KickMembers(EVECallArgs& call, PyInt* listID, PyList* memberIDs);
    EVEResult GetMembers(EVECallArgs& call, PyInt* listID);
    EVEResult SetEntityAccess(EVECallArgs& call, PyInt* listID, PyInt* entityID, PyInt* access);
    EVEResult ClearEntityAccess(EVECallArgs& call, PyInt* listID, PyInt* entityID);
    EVEResult SetMembersMuted(EVECallArgs& call, PyInt* listID, PyList* memberIDs);
    EVEResult SetMembersOperator(EVECallArgs& call, PyInt* listID, PyList* memberIDs);
    EVEResult SetMembersClear(EVECallArgs& call, PyInt* listID, PyList* memberIDs);
    EVEResult SetDefaultAccess(EVECallArgs& call, PyInt* listID, PyInt* defaultAccess, PyInt* defaultMemberAccess, std::optional<PyInt*> mailCost);
    EVEResult GetInfo(EVECallArgs& call, PyInt* listID);
    EVEResult GetSettings(EVECallArgs& call, PyInt* listID);
    EVEResult GetWelcomeMail(EVECallArgs& call, PyInt* listID);
    EVEResult SaveWelcomeMail(EVECallArgs& call, PyInt* listID, PyString* title, PyString* body);
    EVEResult SendWelcomeMail(EVECallArgs& call, PyInt* listID, PyString* title, PyString* body);
    EVEResult ClearWelcomeMail(EVECallArgs& call, PyInt* listID);
};

#endif /* MAILING_LIST_MGR_SERVICE_H */
