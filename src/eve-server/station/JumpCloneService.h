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

#ifndef __JUMPCLONE_SERVICE_H_INCL__
#define __JUMPCLONE_SERVICE_H_INCL__

#include "station/StationDB.h"
#include "services/BoundService.h"
#include "Client.h"

class JumpCloneBound;

class JumpCloneService : public BindableService <JumpCloneService, JumpCloneBound>
{
public:
    JumpCloneService(EVEServiceManager& mgr);

    void BoundReleased (JumpCloneBound* bound) override;

protected:
    StationDB m_db;

    //PyCallable_DECL_CALL(GetShipCloneState)

    //overloaded in order to support bound objects:
    BoundDispatcher* BindObject(Client *client, PyDataType* bindParameters);
};

class JumpCloneBound : public EVEBoundObject <JumpCloneBound>
{
public:
    JumpCloneBound(EVEServiceManager& mgr, JumpCloneService& parent, StationDB* db, uint32 locationID);

protected:
  EVEResult GetCloneState(EVECallArgs& call);
  EVEResult GetShipCloneState(EVECallArgs& call);
  EVEResult GetPriceForClone(EVECallArgs& call);
  EVEResult InstallCloneInStation(EVECallArgs& call);
  EVEResult GetStationCloneState(EVECallArgs& call);
  EVEResult OfferShipCloneInstallation(EVECallArgs& call, PyInt* characterID);
  EVEResult DestroyInstalledClone(EVECallArgs& call, PyInt* cloneID);
  EVEResult AcceptShipCloneInstallation(EVECallArgs& call);
  EVEResult CancelShipCloneInstallation(EVECallArgs& call);
  EVEResult CloneJump(EVECallArgs& call, PyInt* locationID);

protected:
    StationDB *const m_db;        //we do not own this

    uint32 m_locationID;
    uint8 m_locGroupID;
};

#endif