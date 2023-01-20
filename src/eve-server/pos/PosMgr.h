
/**
 * @name PosMgr.h
 *   Class for pos manager calls.
 *
 * @Author:         Allan
 * @date:   28 December 17
 */

#ifndef __POSMGRSVC_SERVICE_H_INCL__
#define __POSMGRSVC_SERVICE_H_INCL__

#include "services/BoundService.h"
#include "pos/PosMgrDB.h"
#include "Client.h"

class PosMgrBound;

class PosMgr : public BindableService <PosMgr, PosMgrBound>
{
public:
    PosMgr(EVEServiceManager& mgr);

    void BoundReleased (PosMgrBound* bound) override;

protected:
    PosMgrDB m_db;

    EVEResult GetJumpArrays(EVECallArgs& call);
    EVEResult GetControlTowers(EVECallArgs& call);
    EVEResult GetControlTowerFuelRequirements(EVECallArgs& call);

    //PyCallable_DECL_CALL()

    //overloaded in order to support bound objects:
    BoundDispatcher* BindObject(Client* client, PyDataType* bindParameters);

private:
    std::map<uint32, PosMgrBound*> m_instances;
};

class PosMgrBound : public EVEBoundObject <PosMgrBound>
{
public:
    PosMgrBound(EVEServiceManager& mgr, PosMgr& parent, uint32 systemID);

    uint32 GetSystemID() { return this->m_systemID; }

protected:
  EVEResult SetTowerPassword(EVECallArgs& call, PyInt* itemID, PyDataType* password);
  EVEResult SetTowerPassword(EVECallArgs& call, PyInt* itemID, PyDataType* password, PyBool* allowCorp, PyBool* allowAlliance);
  EVEResult SetShipPassword(EVECallArgs& call, PyString* password);
  EVEResult GetMoonForTower(EVECallArgs& call, PyInt* itemID);
  EVEResult GetSiloCapacityByItemID(EVECallArgs& call, PyInt* itemID);
  EVEResult AnchorOrbital(EVECallArgs& call, PyInt* itemID);
  EVEResult UnanchorOrbital(EVECallArgs& call, PyInt* itemID);
  EVEResult OnlineOrbital(EVECallArgs& call, PyInt* itemID);
  EVEResult GMUpgradeOrbital(EVECallArgs& call, PyInt* itemID);
  EVEResult AnchorStructure(EVECallArgs& call, PyInt* structureID, PyTuple* position);
  EVEResult UnanchorStructure(EVECallArgs& call, PyInt* itemID);
  EVEResult AssumeStructureControl(EVECallArgs& call, PyInt* itemID);
  EVEResult RelinquishStructureControl(EVECallArgs& call, PyInt* itemID);
  EVEResult ChangeStructureProvisionType(EVECallArgs& call, PyInt* towerID, PyInt* itemID, PyInt* typeID);
  EVEResult CompleteOrbitalStateChange(EVECallArgs& call, PyInt* itemID);
  EVEResult GetMoonProcessInfoForTower(EVECallArgs& call, PyInt* itemID);
  EVEResult LinkResourceForTower(EVECallArgs& call, PyInt* itemID, PyList* connections);
  EVEResult RunMoonProcessCycleforTower(EVECallArgs& call, PyInt* itemID);
  EVEResult GetStarbasePermissions(EVECallArgs& call, PyInt* itemID);
  EVEResult SetStarbasePermissions(EVECallArgs& call, PyInt* itemID, PyObject* deployFlags, PyObject* usageFlagsList);
  EVEResult GetTowerNotificationSettings(EVECallArgs& call, PyInt* itemID);
  EVEResult SetTowerNotifications(EVECallArgs& call, PyInt* itemID, PyBool* showInCalendar, PyBool* sendFuelNotifications);
  EVEResult GetTowerSentrySettings(EVECallArgs& call, PyInt* itemID);
  EVEResult SetTowerSentrySettings(EVECallArgs& call, PyInt* itemID, PyFloat* standing, PyFloat* status, PyBool* statusDrop, PyBool* corpWar, PyBool* useAllianceStandings);
  EVEResult InstallJumpBridgeLink(EVECallArgs& call, PyInt* localItemID, PyInt* remoteSolarSystemID, PyInt* remoteItemID);
  EVEResult UninstallJumpBridgeLink(EVECallArgs& call, PyInt* itemID);

private:
    PosMgrDB m_db;
    uint32 m_systemID;
};


#endif