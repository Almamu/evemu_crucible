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

#ifndef __INVBROKER_SERVICE_H_INCL__
#define __INVBROKER_SERVICE_H_INCL__

#include "inventory/InventoryDB.h"
#include "services/BoundService.h"
#include "Client.h"

class PyRep;
class InvBrokerBound;
class InventoryBound;

class InvBrokerService : public BindableService <InvBrokerService, InvBrokerBound>, public BoundServiceParent<InventoryBound>
{
public:
    InvBrokerService(EVEServiceManager& mgr);

    void BoundReleased (InvBrokerBound* bound) override;
    void BoundReleased (InventoryBound* bound) override;

protected:
  EVEResult GetItemDescriptor(EVECallArgs& call);

    //overloaded in order to support bound objects:
    BoundDispatcher* BindObject(Client *client, PyDataType* bindParameters);
};

class InvBrokerBound : public EVEBoundObject <InvBrokerBound>
{
public:
    InvBrokerBound(EVEServiceManager& mgr, InvBrokerService& parent, uint32 locationID, uint32 groupID);

protected:
  EVEResult GetContainerContents(EVECallArgs& call, PyInt* containerID, PyInt* locationID);
  EVEResult GetInventoryFromId(EVECallArgs& call, PyInt* inventoryID, PyInt* passive);
  EVEResult GetInventory(EVECallArgs& call, PyInt* containerID, std::optional <PyInt*> ownerID);
  EVEResult SetLabel(EVECallArgs& call, PyInt* itemID, PyDataType* itemName);
  EVEResult TrashItems(EVECallArgs& call, PyList* itemIDs, PyInt* locationID);
  EVEResult AssembleCargoContainer(EVECallArgs& call, PyInt* itemID, PyNone* none, PyFloat* zero);
  EVEResult BreakPlasticWrap(EVECallArgs& call);
  EVEResult TakeOutTrash(EVECallArgs& call, PyList* itemIDs);
  EVEResult SplitStack(EVECallArgs& call, PyInt* locationID, PyInt* itemID, PyInt* quantity, PyInt* ownerID);
  EVEResult DeliverToCorpHangar(EVECallArgs& call, PyInt* officeID, PyInt* locationID, PyInt* itemsToDeliver, std::optional <PyInt*> quantity, PyInt* ownerID, PyInt* destinationFlag);
  EVEResult DeliverToCorpMember(EVECallArgs& call, PyInt* corporationMemberID, PyInt* stationID, PyList* itemIDs, std::optional <PyInt*> quantity, PyInt* ownerID);

protected:
    uint32 m_locationID;
    uint32 m_groupID;
};

#endif


