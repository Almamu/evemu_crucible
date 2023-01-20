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


#ifndef __BEYONCE_SERVICE_H_INCL__
#define __BEYONCE_SERVICE_H_INCL__

#include "ship/ShipDB.h"
#include "services/BoundService.h"
#include "system/BookmarkService.h"

class BeyonceBound;

class BeyonceService : public BindableService <BeyonceService, BeyonceBound> {
public:
    BeyonceService(EVEServiceManager& mgr);

    void BoundReleased (BeyonceBound* bound) override;

protected:
    ShipDB m_db;

    EVEResult GetFormations(EVECallArgs& call);

    //overloaded in order to support bound objects:
    BoundDispatcher *BindObject(Client *client, PyDataType* bindParameters) override;
};


class BeyonceBound : public EVEBoundObject <BeyonceBound>
{
public:
    BeyonceBound(EVEServiceManager& mgr, BeyonceService& parent, Client* client);

protected:
  EVEResult CmdFollowBall(EVECallArgs& call, PyInt* ballID, PyDataType* distance);
  EVEResult CmdSetSpeedFraction(EVECallArgs& call, PyFloat* speedFraction);
  EVEResult CmdAlignTo(EVECallArgs& call, PyInt* entityID);
  EVEResult CmdGotoDirection(EVECallArgs& call, PyFloat* x, PyFloat* y, PyFloat* z);
  EVEResult CmdGotoBookmark(EVECallArgs& call, PyInt* bookmarkID);
  EVEResult CmdOrbit(EVECallArgs& call, PyInt* entityID, PyDataType* rangeValue);
  EVEResult CmdWarpToStuff(EVECallArgs& call, PyString* type, PyDataType* id);
  EVEResult CmdWarpToStuffAutopilot(EVECallArgs& call, PyInt* destID);
  EVEResult CmdStop(EVECallArgs& call);
  EVEResult CmdDock(EVECallArgs& call, PyInt* celestialID, PyInt* shipID, std::optional<PyDataType*> paymentRequired);
  EVEResult CmdStargateJump(EVECallArgs& call, PyInt* fromStargateID, PyInt* toStargateID, PyInt* shipID);
  EVEResult CmdAbandonLoot(EVECallArgs& call, PyList* wreckIDs);
  EVEResult UpdateStateRequest(EVECallArgs& call);
  EVEResult CmdJumpThroughFleet(EVECallArgs& call, PyInt* otherCharID, PyInt* otherShipID, PyInt* beaconID, PyInt* solarSystemID);
  EVEResult CmdJumpThroughAlliance(EVECallArgs& call, PyInt* otherShipID, PyInt* beaconID, PyInt* solarSystemID);
  EVEResult CmdJumpThroughCorporationStructure(EVECallArgs& call, PyInt* itemID, PyInt* remoteStructureID, PyInt* remoteSystemID);
  EVEResult CmdBeaconJumpFleet(EVECallArgs& call, PyInt* characterID, PyInt* beaconID, PyInt* solarSystemID);
  EVEResult CmdBeaconJumpAlliance(EVECallArgs& call, PyInt* beaconID, PyInt* solarSystemID);
  EVEResult CmdFleetRegroup(EVECallArgs& call);
  EVEResult CmdFleetTagTarget(EVECallArgs& call, PyInt* itemID, PyString* tag);
    
private:
    BookmarkService* m_bookmark;
};

#endif
