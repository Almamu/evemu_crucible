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


#ifndef __CORPREGISTRY_SERVICE_H_INCL__
#define __CORPREGISTRY_SERVICE_H_INCL__

#include "corporation/CorporationDB.h"
#include "alliance/AllianceDB.h"
#include "services/BoundService.h"
#include "Client.h"

class CorpRegistryBound;

class CorpRegistryService : public BindableService <CorpRegistryService, CorpRegistryBound>
{
public:
    CorpRegistryService(EVEServiceManager& mgr);

    void BoundReleased (CorpRegistryBound* bound) override;

protected:
    CorporationDB m_db;

    /** @note: none of these fully work, and most are skeleton code only */
    EVEResult CreateAlliance(EVECallArgs& call, PyDataType* allianceName, PyDataType* shortName, PyDataType* description, PyDataType* url);
    EVEResult GetRecentKillsAndLosses(EVECallArgs& call);
    EVEResult GetCorporateContacts(EVECallArgs& call);
    EVEResult AddCorporateContact(EVECallArgs& call, PyInt* contactID, PyInt* relationshipID);
    EVEResult EditCorporateContact(EVECallArgs& call, PyInt* contactID, PyInt* relationshipID);
    EVEResult RemoveCorporateContacts(EVECallArgs& call, PyList* contactIDs);
    EVEResult EditContactsRelationshipID(EVECallArgs& call, PyList* contactIDs, PyInt* relationshipID);
    EVEResult GetLabels(EVECallArgs& call);
    EVEResult CreateLabel(EVECallArgs& call, PyString* name, std::optional <PyInt*> color);
    EVEResult DeleteLabel(EVECallArgs& call, PyInt* labelID);
    EVEResult EditLabel(EVECallArgs& call, PyInt* labelID, std::optional <PyString*> name, std::optional <PyInt*> color);
    EVEResult AssignLabels(EVECallArgs& call, PyList* contactIDs, PyInt* labelMask);
    EVEResult RemoveLabels(EVECallArgs& call, PyList* contactIDs, PyInt* labelMask);
    EVEResult ResignFromCEO(EVECallArgs& call, PyInt* newCeoID);

    //overloaded in order to support bound objects:
    BoundDispatcher* BindObject(Client *client, PyDataType* bindParameters);

private:
    std::map<uint32, CorpRegistryBound*> m_instances;
};

#endif
