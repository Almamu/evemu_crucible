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
    Author:        Reve
    Updates:    Allan
*/

#ifndef EVEMU_PLANET_PLANETMGR_BOUND_H_
#define EVEMU_PLANET_PLANETMGR_BOUND_H_

#include "services/BoundService.h"
#include "planet/PlanetDB.h"
#include "planet/PlanetMgr.h"
#include "Client.h"

class PlanetMgrBound;

class PlanetMgrService : public BindableService <PlanetMgrService, PlanetMgrBound>
{
public:
    PlanetMgrService(EVEServiceManager& mgr);

    void BoundReleased (PlanetMgrBound* bound) override;
protected:
    BoundDispatcher* BindObject(Client *client, PyDataType* bindParameters);

    EVEResult GetPlanetsForChar(EVECallArgs& call);
    EVEResult GetMyLaunchesDetails(EVECallArgs& call);
    EVEResult GetPlanet(EVECallArgs& call, PyInt* planetID);
    EVEResult DeleteLaunch(EVECallArgs& call, PyInt* launchID);

private:
    std::map<uint32, PlanetMgrBound*> m_instances;
};


class PlanetMgrBound : public EVEBoundObject <PlanetMgrBound>
{
public:
    PlanetMgrBound(EVEServiceManager& mgr, PlanetMgrService& parent, Client* client, PlanetSE* planet);

    uint32 GetPlanetID () { return this->m_planet->GetID(); }

protected:
  EVEResult GetPlanetResourceInfo(EVECallArgs& call);
  EVEResult GetPlanetInfo(EVECallArgs& call);
  EVEResult GetExtractorsForPlanet(EVECallArgs& call, PyInt* planetID);
  EVEResult UserUpdateNetwork(EVECallArgs& call, PyList* commandList);
  EVEResult GetProgramResultInfo(EVECallArgs& call, PyInt* ecuID, PyInt* typeID, PyList* heads, PyFloat* headRadius);
  EVEResult GetResourceData(EVECallArgs& call, PyObject* info);
  EVEResult UserAbandonPlanet(EVECallArgs& call);
  EVEResult UserLaunchCommodities(EVECallArgs& call, PyInt* commandPinID, PyDict* commoditiesToLaunch);
  EVEResult UserTransferCommodities(EVECallArgs& call, PyList* path, PyDict* commodities);
  EVEResult GetCommandPinsForPlanet(EVECallArgs& call, PyInt* planetID);
  EVEResult GetFullNetworkForOwner(EVECallArgs& call, PyInt* planetID, PyInt* characterID);
  EVEResult GMAddCommodity(EVECallArgs& call, PyInt* pinID, PyInt* typeID, PyInt* quantity);
  EVEResult GMConvertCommandCenter(EVECallArgs& call, PyInt* pinID);
  EVEResult GMForceInstallProgram(EVECallArgs& call, PyInt* pinID, PyInt* typeID, PyInt* cycleTime, PyInt* lifetimeHours, PyInt* qtyPerCycle, PyFloat* radius);
  EVEResult GMGetLocalDistributionReport(EVECallArgs& call, PyInt* planetID, PyTuple* surfacePoint);
  EVEResult GMGetSynchedServerState(EVECallArgs& call, PyInt* characterID);
  EVEResult GMRunDepletionSim(EVECallArgs& call);

    /*

    data = planet.remoteHandler.GMGetCompleteResource(resourceTypeID, layer)
        sh = builder.CreateSHFromBuffer(data.data, data.numBands)

    self.planet.remoteHandler.GMCreateNuggetLayer(self.planetID, typeID)
        self.GMShowResource(typeID, 'nuggets')      {{ 'nuggets' = layer here }}

        */

protected:
    Colony* m_colony;
    PlanetSE* m_planet;
    PlanetMgr* m_planetMgr;
};
#endif  // EVEMU_PLANET_PLANETMGR_BOUND_H_