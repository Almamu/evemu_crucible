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


#ifndef __DOGMAIM_SERVICE_H_INCL__
#define __DOGMAIM_SERVICE_H_INCL__

#include "services/BoundService.h"
#include "Client.h"

class PyRep;
class DogmaIMBound;

class DogmaIMService : public BindableService <DogmaIMService, DogmaIMBound>
{
public:
    DogmaIMService(EVEServiceManager& mgr);

    void BoundReleased (DogmaIMBound* bound) override;

protected:
  EVEResult GetAttributeTypes(EVECallArgs& call);

    //overloaded in order to support bound objects:
    BoundDispatcher* BindObject(Client *client, PyDataType* bindParameters);

private:
    ObjCacheService* m_cache;
};


/** this is either DogmaLM (Location Manager) or DogmaIM (Instance Manager) for bound objects.
 * it depends on the object, location, and calling function
 *    i see no reason to change it at this point.
 */
class DogmaIMBound : public EVEBoundObject <DogmaIMBound>
{
public:
    DogmaIMBound(EVEServiceManager& mgr, DogmaIMService& parent, uint32 locationID, uint32 groupID);

protected:
  EVEResult ChangeDroneSettings(EVECallArgs& call, PyDict* settings);
  EVEResult LinkWeapons(EVECallArgs& call, PyInt* shipID, PyInt* masterID, PyInt* fromID);
  EVEResult LinkAllWeapons(EVECallArgs& call, PyInt* shipID);
  EVEResult UnlinkModule(EVECallArgs& call, PyInt* shipID, PyInt* moduleID);
  EVEResult UnlinkAllModules(EVECallArgs& call, PyInt* shipID);
  EVEResult OverloadRack(EVECallArgs& call, PyInt* itemID);
  EVEResult StopOverloadRack(EVECallArgs& call, PyInt* itemID);
  EVEResult CharGetInfo(EVECallArgs& call);
  EVEResult ItemGetInfo(EVECallArgs& call, PyInt* itemID);
  EVEResult GetAllInfo(EVECallArgs& call, PyBool* getCharInfo, PyBool* getShipInfo);
  EVEResult DestroyWeaponBank(EVECallArgs& call, PyInt* shipID, PyInt* itemID);
  EVEResult GetCharacterBaseAttributes(EVECallArgs& call);
  EVEResult Activate(EVECallArgs& call, PyInt* itemID, PyInt* effectID);
  EVEResult Activate(EVECallArgs& call, PyInt* itemID, PyString* effectName, std::optional <PyInt*> target, PyInt* repeat);
  EVEResult Deactivate(EVECallArgs& call, PyInt* itemID, PyInt* effect);
  EVEResult Deactivate(EVECallArgs& call, PyInt* itemID, PyString* effectName);
  EVEResult Overload(EVECallArgs& call, PyInt* itemID, PyInt* effectID);
  EVEResult StopOverload(EVECallArgs& call, PyInt* itemID, PyInt* effectID);
  EVEResult CancelOverloading(EVECallArgs& call, PyInt* itemID);
  EVEResult SetModuleOnline(EVECallArgs& call, PyInt* locationID, PyInt* moduleID);
  EVEResult TakeModuleOffline(EVECallArgs& call, PyInt* locationID, PyInt* moduleID);
  EVEResult LoadAmmoToBank(EVECallArgs& call, PyInt* shipID, PyInt* masterID, PyInt* chargeTypeID, PyList* cItemIDs, PyInt* chargeLocationID, std::optional <PyInt*> qty);
  EVEResult LoadAmmoToModules(EVECallArgs& call, PyInt* shipID, PyList* cModuleIDs, PyInt* chargeTypeID, PyInt* itemID, PyInt* ammoLocationId);
  EVEResult GetTargets(EVECallArgs& call);
  EVEResult GetTargeters(EVECallArgs& call);
  EVEResult AddTarget(EVECallArgs& call, PyInt* targetID);
  EVEResult RemoveTarget(EVECallArgs& call, PyInt* targetID);
  EVEResult ClearTargets(EVECallArgs& call);
  EVEResult InitiateModuleRepair(EVECallArgs& call, PyInt* itemID);
  EVEResult StopModuleRepair(EVECallArgs& call, PyInt* itemID);
  EVEResult MergeModuleGroups(EVECallArgs& call, PyInt* shipID, PyInt* masterID, PyInt* slaveID);
  EVEResult PeelAndLink(EVECallArgs& call, PyInt* shipID, PyInt* masterID, PyInt* slaveID);

    /*  OBO == ??  (pos targeting)
     * flag, targetList = self.GetDogmaLM().AddTargetOBO(sid, tid) (structureID, targetID)
     * self.GetDogmaLM().RemoveTargetOBO(sid, tid)  (structureID, targetID)
    */
private:

    uint32 m_locationID;
    uint32 m_groupID;
};

#endif
