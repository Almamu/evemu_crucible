
/**
 * @name PosMgr.cpp
 *   Class for pos manager calls.
 *
 * @Author:         Allan
 * @date:   28 December 17
 */

/*
 * POS__ERROR
 * POS__WARNING
 * POS__MESSAGE
 * POS__DUMP
 * POS__DEBUG
 * POS__DESTINY
 * POS__SLIMITEM
 * POS__TRACE
 */


#include "eve-server.h"



#include "planet/Moon.h"
#include "pos/PosMgr.h"
#include "pos/Array.h"
#include "pos/Battery.h"
#include "pos/Structure.h"
#include "pos/Tower.h"
#include "pos/Weapon.h"
#include "system/SystemManager.h"
#include "services/ServiceManager.h"

PosMgr::PosMgr(EVEServiceManager &mgr) :
    BindableService("posMgr", mgr)
{
    this->Add("GetJumpArrays", &PosMgr::GetJumpArrays);
    this->Add("GetControlTowers", &PosMgr::GetControlTowers);
    this->Add("GetControlTowerFuelRequirements", &PosMgr::GetControlTowerFuelRequirements);
}

BoundDispatcher* PosMgr::BindObject(Client* client, PyDataType* bindParameters) {
    _log( POS__DUMP, "PosMgr bind request for:" );
    bindParameters->dump( POS__DUMP, "    " );

    if (!bindParameters->is<PyInt>()){
        sLog.Error( "PosMgr::CreateBoundObject", "%s: bind_args is not int: '%s'. ", client->GetName(), bindParameters->TypeString() );
        return nullptr;
    }

    uint32 systemID = bindParameters->as<PyInt>()->value();
    auto it = this->m_instances.find (systemID);

    if (it != this->m_instances.end ())
        return it->second;

    PosMgrBound* bound = new PosMgrBound(this->GetServiceManager(), *this, systemID);

    this->m_instances.insert_or_assign (systemID, bound);

    return bound;
}

void PosMgr::BoundReleased (PosMgrBound* bound) {
    auto it = this->m_instances.find (bound->GetSystemID());

    if (it == this->m_instances.end ())
        return;

    this->m_instances.erase (it);
}

EVEResult PosMgr::GetControlTowerFuelRequirements(EVECallArgs&call) {
    /** @todo put this in static data */
    DBQueryResult res;
    m_db.GetControlTowerFuelRequirements(res);

    DBResultRow row;
    PyList* list = new PyList();
    while (res.GetRow(row)) {
        //SELECT controlTowerTypeID, resourceTypeID, purpose, quantity, minSecurityLevel, factionID, wormholeClassID
        list->add(new PyObject("util.KeyVal", new PyDict {
            {"controlTowerTypeID", new PyInt (row.GetInt (0))},
            {"resourceTypeID", new PyInt (row.GetInt (1))},
            {"purpose", new PyInt (row.GetInt (2))},
            {"quantity", new PyInt (row.GetInt (3))},
            {"minSecurityLevel", (row.IsNull (4) ? PyStatic.NewNone () : new PyFloat (row.GetDouble (4)))},
            {"factionID", (row.IsNull (5) ? PyStatic.NewNone () : new PyInt (row.GetInt (5)))},
            {"wormholeClassID", (row.IsNull (6) ? PyStatic.NewNone () : new PyInt (row.GetInt (6)))},
        }));
    }

    return list;
}

EVEResult PosMgr::GetControlTowers(EVECallArgs&call) {
    /*  ct = sm.RemoteSvc('posMgr').GetControlTowers()
     *        for row in ct:
     *            typeID, structureID, solarSystemID = row[0:3]
     *
     *                        [CRowSet]
     *                            [DBRowDescriptor]
     *                                ["typeID" =>  [I4] ]
     *                                ["itemID" =>  [I8] ]
     *                                ["locationID" =>  [I8] ]
     *                            Rows:
     *                                [PyPackedRow 21 bytes]
     *                                    ["typeID" =>  [I4] <20062>]
     *                                    ["itemID" =>  [I8] <1013581844534>]
     *                                    ["locationID" =>  [I8] <30002206>]
     *                                [PyPackedRow 21 bytes]
     *                                    ["typeID" =>  [I4] <27606>]
     *                                    ["itemID" =>  [I8] <1010759458081>]
     *                                    ["locationID" =>  [I8] <30002225>]
     */

    _log(POS__TRACE,  "PosMgr::Handle_GetControlTowers()");
    call.dump(POS__DUMP);

    return m_db.GetCorpControlTowers(call.client->GetCorporationID());
}

EVEResult PosMgr::GetJumpArrays(EVECallArgs&call) {
    /*        jb = sm.RemoteSvc('posMgr').GetJumpArrays()
     *
     *            for data in jb:
     *                solarSystemID, subData = data (fromSystem, solarSystemData)     <-- tuple (to solarSystemID, StructureData)
     *                    ssid = subData.keys()[0]                                    <-- jb itemID (from)
     *                    tsid = subData.values()[0][1]                               <-- jb itemID (to)
     *
     *                    fromStructure = solarSystemData.keys()[0]                   <-- indexed dict of tuple
     *                    toSystem = cfg.evelocations.Get(solarSystemData.values()[0][0])     <-- to solarSystemID
     *                    toStructure = solarSystemData.values()[0][1]                       <-- jb itemID (to)
     *                    toStructureType = solarSystemData.values()[0][2]                   <-- jb typeID
     */
    DBQueryResult res;
    m_db.GetCorpJumpArrays(call.client->GetCorporationID(), res);

    DBResultRow row;
    PyList* list = new PyList();
    while (res.GetRow(row)) {
        //SELECT itemID, systemID, toItemID, toTypeID, toSystemID
        list->add(new PyTuple {
            new PyInt (row.GetInt (1)),
            row.GetInt(2) <= 0 ? PyStatic.NewNone() : new PyDict {
                {new PyInt (row.GetInt (0)), new PyTuple {
                    new PyInt (row.GetInt(4)),
                    new PyInt (row.GetInt(2)),
                    new PyInt (row.GetInt(3))
                    }
                }
            }
        });
    }

    list->dump(POS__RSP_DUMP, "    ");
    return list;
}

PosMgrBound::PosMgrBound(EVEServiceManager& mgr, PosMgr& parent, uint32 systemID) :
    EVEBoundObject(mgr, parent)
{
    m_systemID = systemID;

    this->Add("GetMoonForTower", &PosMgrBound::GetMoonForTower);
    this->Add("SetTowerPassword", static_cast <EVEResult (PosMgrBound::*)(EVECallArgs&, PyInt*, PyDataType*, PyBool*, PyBool*)> (&PosMgrBound::SetTowerPassword));
    this->Add("SetTowerPassword", static_cast <EVEResult (PosMgrBound::*)(EVECallArgs&, PyInt*, PyDataType*)> (&PosMgrBound::SetTowerPassword));
    this->Add("SetShipPassword", &PosMgrBound::SetShipPassword);
    this->Add("GetSiloCapacityByItemID", &PosMgrBound::GetSiloCapacityByItemID);
    this->Add("AnchorOrbital", &PosMgrBound::AnchorOrbital);
    this->Add("UnanchorOrbital", &PosMgrBound::UnanchorOrbital);
    this->Add("OnlineOrbital", &PosMgrBound::OnlineOrbital);
    this->Add("GMUpgradeOrbital", &PosMgrBound::GMUpgradeOrbital);
    this->Add("AnchorStructure", &PosMgrBound::AnchorStructure);
    this->Add("UnanchorStructure", &PosMgrBound::UnanchorStructure);
    this->Add("AssumeStructureControl", &PosMgrBound::AssumeStructureControl);
    this->Add("RelinquishStructureControl", &PosMgrBound::RelinquishStructureControl);
    this->Add("ChangeStructureProvisionType", &PosMgrBound::ChangeStructureProvisionType);
    this->Add("CompleteOrbitalStateChange", &PosMgrBound::CompleteOrbitalStateChange);
    this->Add("GetMoonProcessInfoForTower", &PosMgrBound::GetMoonProcessInfoForTower);
    this->Add("LinkResourceForTower", &PosMgrBound::LinkResourceForTower);
    this->Add("RunMoonProcessCycleforTower", &PosMgrBound::RunMoonProcessCycleforTower);
    this->Add("GetStarbasePermissions", &PosMgrBound::GetStarbasePermissions);
    this->Add("SetStarbasePermissions", &PosMgrBound::SetStarbasePermissions);
    this->Add("GetTowerNotificationSettings", &PosMgrBound::GetTowerNotificationSettings);
    this->Add("SetTowerNotifications", &PosMgrBound::SetTowerNotifications);
    this->Add("GetTowerSentrySettings", &PosMgrBound::GetTowerSentrySettings);
    this->Add("SetTowerSentrySettings", &PosMgrBound::SetTowerSentrySettings);
    this->Add("InstallJumpBridgeLink", &PosMgrBound::InstallJumpBridgeLink);
    this->Add("UninstallJumpBridgeLink", &PosMgrBound::UninstallJumpBridgeLink);
}

EVEResult PosMgrBound::InstallJumpBridgeLink(EVECallArgs&call, PyInt* localItemID, PyInt* remoteSolarSystemID, PyInt* remoteItemID) {
    /**
     *    def BridgePortals(self, localItemID, remoteSolarSystemID, remoteItemID):
     *        posLocation = util.Moniker('posMgr', session.solarsystemid)
     *        posLocation.InstallJumpBridgeLink(localItemID, remoteSolarSystemID, remoteItemID)
     *
     * 01:15:52 W PosMgrBound::Handle_InstallJumpBridgeLink(): size=3
     * 01:15:52 [POS:Dump]   Call Arguments:
     * 01:15:52 [POS:Dump]      Tuple: 3 elements
     * 01:15:52 [POS:Dump]       [ 0]    Integer: 140000332     <-- itemID
     * 01:15:52 [POS:Dump]       [ 1]    Integer: 30000053      <-- toSystemID
     * 01:15:52 [POS:Dump]       [ 2]    Integer: 140000064     <-- toItemID
     *
     */
    _log(POS__TRACE,  "PosMgrBound::Handle_InstallJumpBridgeLink()");
    call.dump(POS__DUMP);

    // Install jump bridge link both ways
    m_db.InstallBridgeLink(localItemID->value(), remoteSolarSystemID->value(), remoteItemID->value());
    m_db.InstallBridgeLink(remoteItemID->value(), call.client->GetSystemID(), localItemID->value());

    return PyStatic.NewNone();
}

EVEResult PosMgrBound::UninstallJumpBridgeLink(EVECallArgs&call, PyInt* itemID) {
    /**
     *    def UnbridgePortal(self, itemID):
     *        posLocation = util.Moniker('posMgr', session.solarsystemid)
     *        posLocation.UninstallJumpBridgeLink(itemID)
     *
     */
    _log(POS__TRACE,  "PosMgrBound::Handle_UninstallJumpBridgeLink()");
    call.dump(POS__DUMP);

    m_db.UninstallRemoteBridgeLink(itemID->value());
    m_db.UninstallBridgeLink(itemID->value());

    return PyStatic.NewNone();
}

EVEResult PosMgrBound::GetSiloCapacityByItemID(EVECallArgs&call, PyInt* itemID) {
    _log(POS__TRACE,  "PosMgrBound::Handle_GetSiloCapacityByItemID()");
    call.dump(POS__DUMP);

    uint16 typeID = 0;
    /** @todo  put this in static data */

    return m_db.GetSiloCapacityForType(typeID);
}

EVEResult PosMgrBound::GetTowerNotificationSettings(EVECallArgs&call, PyInt* itemID) {
    /*
     *        notifySettings = self.posMgr.GetTowerNotificationSettings(self.slimItem.itemID)
     *        self.fuelNotifyCheckbox.SetChecked(notifySettings.sendFuelNotifications, 0)
     *        self.calendarCheckbox.SetChecked(notifySettings.showInCalendar, 0)
     */
    _log(POS__TRACE,  "PosMgrBound::Handle_GetTowerNotificationSettings()");
    call.dump(POS__DUMP);

    PyDict* dict = new PyDict {
        {"header", new PyList {
               new PyString ("sendFueldNotifications"),
               new PyString ("showInCalendar")
           }
        }
    };

    SystemManager* pSystem = call.client->SystemMgr();
    if (pSystem == nullptr) {
        codelog(CLIENT__ERROR, "%s: Client has no system manager!", call.client->GetName());
        return new PyObject("util.Row", dict);
    }

    TowerSE* pTSE = pSystem->GetSE(itemID->value())->GetTowerSE();
    if (pTSE == nullptr)
        return new PyObject("util.Row", dict);

    dict->set ("line", new PyList {
        new PyBool (pTSE->SendFuelNotifications()),
        new PyBool (pTSE->ShowInCalendar())
    });

    return new PyObject("util.Row", dict);
}

EVEResult PosMgrBound::SetTowerNotifications(EVECallArgs&call, PyInt* itemID, PyBool* showInCalendar, PyBool* sendFuelNotifications) {
    //self.posMgr.SetTowerNotifications(self.slimItem.itemID, showInCalendar, sendFuelNotifications)

    SystemManager* pSystem = call.client->SystemMgr();
    if (pSystem == nullptr) {
        codelog(CLIENT__ERROR, "%s: Client has no system manager!", call.client->GetName());
        return PyStatic.NewNone();
    }

    TowerSE* pTSE = pSystem->GetSE(itemID->value())->GetTowerSE();
    if (pTSE == nullptr)
        return PyStatic.NewNone();

    pTSE->SetSendFuelNotifications(sendFuelNotifications->value());
    pTSE->SetShowInCalendar(showInCalendar->value());
    pTSE->UpdateNotify();
    return PyStatic.NewNone();
}

EVEResult PosMgrBound::GetTowerSentrySettings(EVECallArgs&call, PyInt* itemID) {
    //  standing, status, statusDrop, war, standingOwnerID = self.sentrySettings = self.posMgr.GetTowerSentrySettings(self.slimItem.itemID)
    PyDict* data = new PyDict {
        {"header", new PyList {
                new PyString ("standing"),
                new PyString ("status"),
                new PyString ("statusDrop"),
                new PyString ("corpWar"),
                new PyString ("standingOwnerID")
            }
        }
    };

    SystemManager* pSystem = call.client->SystemMgr();
    if (pSystem == nullptr) {
        codelog(CLIENT__ERROR, "%s: Client has no system manager!", call.client->GetName());
        return new PyObject("util.Row", data);
    }

    TowerSE* pTSE = pSystem->GetSE(itemID->value())->GetTowerSE();
    if (pTSE == nullptr)
        return new PyObject("util.Row", data);

    data->set ("line", new PyList {
        new PyFloat (pTSE->GetStanding()),
        new PyFloat (pTSE->GetStatus()),
        new PyBool (pTSE->GetStatusDrop()),
        new PyBool (pTSE->GetCorpWar()),
        new PyInt (pTSE->GetStandingOwnerID())
    });

    return new PyObject("util.Row", data);
}

EVEResult PosMgrBound::SetTowerSentrySettings(EVECallArgs&call, PyInt* itemID, PyFloat* standing, PyFloat* status, PyBool* statusDrop, PyBool* corpWar, PyBool* useAllianceStandings) {
    //  self.posMgr.SetTowerSentrySettings(self.slimItem.itemID, standing, status, statusDrop, war, useAllianceStandings)
    SystemManager* pSystem = call.client->SystemMgr();
    if (pSystem == nullptr) {
        codelog(CLIENT__ERROR, "%s: Client has no system manager!", call.client->GetName());
        return PyStatic.NewNone();
    }

    TowerSE* pTSE = pSystem->GetSE(itemID->value())->GetTowerSE();
    if (pTSE == nullptr)
        return PyStatic.NewNone();

    pTSE->SetStanding(standing->value());
    pTSE->SetStatus(status->value());
    pTSE->SetStatusDrop(statusDrop->value());
    pTSE->SetCorpWar(corpWar->value());
    pTSE->SetUseAllyStandings(useAllianceStandings->value());
    pTSE->UpdateSentry();

    return PyStatic.NewNone();
}

EVEResult PosMgrBound::GetStarbasePermissions(EVECallArgs&call, PyInt* itemID) {
    //  deployFlags, usageFlagsList = self.posMgr.GetStarbasePermissions(self.slimItem.itemID)
    SystemManager* pSystem = call.client->SystemMgr();
    if (pSystem == nullptr) {
        codelog(CLIENT__ERROR, "%s: Client has no system manager!", call.client->GetName());
        return PyStatic.NewNone();
    }

    TowerSE* pTSE = pSystem->GetSE(itemID->value())->GetTowerSE();
    if (pTSE == nullptr)
        return PyStatic.NewNone();

    return new PyTuple {
        pTSE->GetDeployFlags(), // deployFlags
        pTSE->GetUsageFlagList() // usageFlagsList
    };
}

EVEResult PosMgrBound::SetStarbasePermissions(EVECallArgs&call, PyInt* itemID, PyObject* deployFlags, PyObject* usageFlagsList) {
    //  self.posMgr.SetStarbasePermissions(self.slimItem.itemID, self.sr.deployFlags, self.sr.usageFlagsList)
    /* values....
     * 0 - corp role config equip.
     * 1 - corp members
     * 2 - ally members
     * 3 - corp role fuel tech
     */

    _log(POS__TRACE,  "PosMgrBound::Handle_SetStarbasePermissions()");
    call.dump(POS__DUMP);

    SystemManager* pSystem = call.client->SystemMgr();
    if (pSystem == nullptr) {
        codelog(CLIENT__ERROR, "%s: Client has no system manager!", call.client->GetName());
        return PyStatic.NewNone();
    }

    TowerSE* pTSE = pSystem->GetSE(itemID->value())->GetTowerSE();
    if (pTSE == nullptr)
        return PyStatic.NewNone();

    // decode deployFlags object
    PyDataType* rep = deployFlags->arguments()->as<PyDict>()->get("line");
    if (!rep->is<PyList>()) {
        codelog(POS__ERROR, "deployFlags 'line' item is not PyList: %s", rep->TypeString());
        return nullptr;
    }

    PyList* list = rep->as<PyList>();
    //list->Dump(POS__RSP_DUMP, "    ");
    pTSE->SetDeployFlags(list->at (0)->as<PyInt>()->value(),
                         list->at (1)->as<PyInt>()->value(),
                         list->at (2)->as<PyInt>()->value(),
                         list->at (3)->as<PyInt>()->value() );


    // decode usageFlagsList object
    PyDataType* rep2 = usageFlagsList->arguments()->as<PyDict>()->get("lines");
    if (!rep2->is<PyList>()) {
        codelog(POS__ERROR, "usageFlagsList 'lines' item is not PyList: %s", rep2->TypeString());
        return nullptr;
    }

    PyList* list2 = rep2->as<PyList>();
    //list2->Dump(POS__RSP_DUMP, "    ");
    for (PyList::const_iterator itr = list2->begin(); itr != list2->end(); ++itr) {
        if (!(*itr)->is<PyList>()) {
            codelog(POS__ERROR, "usageFlagsList - itr item is not PyList: %s", (*itr)->TypeString());
            continue;
        }
        pTSE->SetUseFlags((*itr)->as<PyList>()->at (0)->as<PyInt>()->value(),
                          (*itr)->as<PyList>()->at (1)->as<PyInt>()->value(),
                          (*itr)->as<PyList>()->at (2)->as<PyInt>()->value(),
                          (*itr)->as<PyList>()->at (3)->as<PyInt>()->value());
    }

    return PyStatic.NewNone();
}

EVEResult PosMgrBound::GetMoonForTower(EVECallArgs&call, PyInt* itemID) {
  /*
13:13:06 L PosMgrBound::Handle_GetMoonForTower(): size= 1
13:13:06 [SvcCall]   Call Arguments:
13:13:06 [SvcCall]       Tuple: 1 elements
13:13:06 [SvcCall]         [ 0] Integer field: 140001260

self.moonID = self.moon[0]
if self.moon[1] is not None:
    for typeID, quantity in self.moon[1]:

    returns
        tuple
            moonID
            tuple
                resource typeID
                resource quantity

  _log(POS__TRACE,  "PosMgrBound::Handle_GetMoonForTower()");
  call.dump(POS__DUMP);
  */

    SystemManager* pSystem = call.client->SystemMgr();
    if (pSystem == nullptr) {
        codelog(CLIENT__ERROR, "%s: Client has no system manager!", call.client->GetName());
        return PyStatic.NewNone();
    }

    TowerSE* pTSE = pSystem->GetSE(itemID->value())->GetTowerSE();
    if (pTSE == nullptr)
        return PyStatic.NewNone();
    MoonSE* pMSE = pTSE->GetMoonSE();
    if (pMSE == nullptr)
        return PyStatic.NewNone();

    std::map<uint16, uint8>::iterator itr = pMSE->GooBegin(), end = pMSE->GooEnd();
    PyList* list = new PyList();
    while (itr != end) {
        list->add(new PyTuple {
            new PyInt (itr->first),
            new PyInt (itr->second)
        });
        ++itr;
    }

    return new PyTuple {
        new PyInt (pMSE->GetID()),
        list
    };
}

EVEResult PosMgrBound::SetShipPassword(EVECallArgs&call, PyString* password) {
    /*
     * 13:16:17 L PosMgrBound::Handle_SetShipPassword(): size= 1
     * 13:16:17 [SvcCall]   Call Arguments:
     * 13:16:17 [SvcCall]       Tuple: 1 elements
     * 13:16:17 [SvcCall]         [ 0] WString: 'test'             << password
     */

    // havent been able to call this while docked, but there is an error msg for it.
    if (call.client->IsDocked())
        throw UserError("CannotSetShieldHarmonicPassword");

    call.client->GetShipSE()->SetPassword(password->string());

    return PyStatic.NewNone();
}

EVEResult PosMgrBound::SetTowerPassword(EVECallArgs& call, PyInt* itemID, PyDataType* password) {
    SystemManager* pSystem = call.client->SystemMgr();
    if (pSystem == nullptr) {
        codelog(CLIENT__ERROR, "%s: Client has no system manager!", call.client->GetName());
        return PyStatic.NewNone();
    }

    TowerSE* pTSE = pSystem->GetSE(itemID->value())->GetTowerSE();
    if (pTSE == nullptr)
        return PyStatic.NewNone();

    if (password->is<PyString>() or password->is<PyString>())
        pTSE->SetPassword(password->string());
    pTSE->UpdatePassword();

    return PyStatic.NewNone();
}

EVEResult PosMgrBound::SetTowerPassword(EVECallArgs&call, PyInt* itemID, PyDataType* password, PyBool* allowCorp, PyBool* allowAlliance) {
    SystemManager* pSystem = call.client->SystemMgr();
    if (pSystem == nullptr) {
        codelog(CLIENT__ERROR, "%s: Client has no system manager!", call.client->GetName());
        return PyStatic.NewNone();
    }

    TowerSE* pTSE = pSystem->GetSE(itemID->value())->GetTowerSE();
    if (pTSE == nullptr)
        return PyStatic.NewNone();

    if (password->is<PyString>() or password->is<PyString>()) {
        pTSE->SetPassword(password->string());
        pTSE->UpdatePassword();
    }

    pTSE->SetCorpAccess(allowCorp->value());
    pTSE->SetAllyAccess(allowAlliance->value());
    pTSE->UpdateAccess();

    // set harmonic for ship to 'offline' (0)   -according to packet data
    call.client->GetShipSE()->SetHarmonic(EVEPOS::Harmonic::Offline);

    // at this point, if ship password isnt updated, it should be kicked out of tower's ff.
    //  not sure how we're gonna do this yet.  will have to wait till system matures

    return PyStatic.NewNone();
}

EVEResult PosMgrBound::UnanchorStructure(EVECallArgs&call, PyInt* itemID) {
    _log(POS__TRACE,  "PosMgrBound::Handle_UnanchorStructure()");
    call.dump(POS__DUMP);

    StructureSE* pTSE(nullptr);

    //pTSE->PullAnchor();

    return PyStatic.NewNone();
}

EVEResult PosMgrBound::AnchorStructure(EVECallArgs&call, PyInt* structureID, PyTuple* position) {
    _log(POS__TRACE, "POS Mgr::Anchor()");
    call.dump(POS__DUMP);
    SystemManager* pSystem = call.client->SystemMgr();
    if (pSystem == nullptr) {
        codelog(CLIENT__ERROR, "%s: Client has no system manager!", call.client->GetName());
        return PyStatic.NewNone();
    }

    double posX = position->at (0)->i64();
    double posY = position->at (1)->i64();
    double posZ = position->at (2)->i64();

    StructureSE* pTSE = pSystem->GetSE(structureID->value())->GetPOSSE();
    if (pTSE == nullptr)
        return PyStatic.NewNone();

    GPoint pos(posX, posY, posZ);
    pTSE->SetAnchor(call.client, pos);

    // auto warp to new POS position?  config option?
    if (pTSE->IsTowerSE()) {
        uint32 dist = pTSE->GetSelf()->radius() + call.client->GetShip()->radius();
        call.client->GetShipSE()->DestinyMgr()->WarpTo(pos, dist);
        /** @todo add tower anchor position to bookmark */
    }
    // returns nodeID and timestamp
    return this->GetOID();
}

EVEResult PosMgrBound::GetMoonProcessInfoForTower(EVECallArgs&call, PyInt* itemID) {
    SystemManager* pSystem = call.client->SystemMgr();
    if (pSystem == nullptr) {
        codelog(CLIENT__ERROR, "%s: Client has no system manager!", call.client->GetName());
        return PyStatic.NewNone();
    }

    TowerSE* pTSE = pSystem->GetSE(itemID->value())->GetTowerSE();
    if (pTSE == nullptr)
        return PyStatic.NewNone();

    return pTSE->GetProcessInfo();
}

EVEResult PosMgrBound::AssumeStructureControl(EVECallArgs&call, PyInt* itemID) {
    // NOTE:  this is for controlling pos defences
    /*
        posMgr = moniker.GetPOSMgr()
        posMgr.AssumeStructureControl(item.itemID)

        02:02:07 W PosMgrBound::Handle_AssumeStructureControl(): size=1
        02:02:07 [POS:Dump]   Call Arguments:
        02:02:07 [POS:Dump]      Tuple: 1 elements
        02:02:07 [POS:Dump]       [ 0]    Integer: 140000311

        'OnAssumeStructureControl',
        'OnRelinquishStructureControl',

    */
    _log(POS__TRACE,  "PosMgrBound::Handle_AssumeStructureControl()");
    call.dump(POS__DUMP);

    return PyStatic.NewNone();
}

EVEResult PosMgrBound::RelinquishStructureControl(EVECallArgs&call, PyInt* itemID) {
    /*
        posMgr = moniker.GetPOSMgr()
        posMgr.RelinquishStructureControl(item.itemID)
    */
    _log(POS__TRACE,  "PosMgrBound::Handle_RelinquishStructureControl()");
    call.dump(POS__DUMP);

    return PyStatic.NewNone();
}

EVEResult PosMgrBound::AnchorOrbital(EVECallArgs&call, PyInt* itemID) {
    /*
     *  def AnchorOrbital(self, itemID):
     *      posMgr = util.Moniker('posMgr', session.solarsystemid)
     *      posMgr.AnchorOrbital(itemID)
     */

    _log(POS__TRACE,  "PosMgrBound::Handle_()");
    call.dump(POS__DUMP);

    return PyStatic.NewNone();
}

EVEResult PosMgrBound::UnanchorOrbital(EVECallArgs&call, PyInt* itemID) {
    /*
     *  def UnanchorOrbital(self, itemID):
     *      posMgr = util.Moniker('posMgr', session.solarsystemid)
     *      posMgr.UnanchorOrbital(itemID)
     */
    _log(POS__TRACE,  "PosMgrBound::Handle_UnanchorOrbital()");
    call.dump(POS__DUMP);

    return PyStatic.NewNone();
}

EVEResult PosMgrBound::OnlineOrbital(EVECallArgs&call, PyInt* itemID) {
    _log(POS__TRACE,  "PosMgrBound::Handle_OnlineOrbital()");
    call.dump(POS__DUMP);

    return PyStatic.NewNone();
}

EVEResult PosMgrBound::ChangeStructureProvisionType(EVECallArgs&call, PyInt* towerID, PyInt* itemID, PyInt* typeID) {
    // this changes silo content or mining product
    /*
     * 03:11:32 W PosMgrBound::Handle_ChangeStructureProvisionType(): size=3
     * 03:11:32 [POS:Dump]   Call Arguments:
     * 03:11:32 [POS:Dump]      Tuple: 3 elements
     * 03:11:32 [POS:Dump]       [ 0]    Integer: 140000061     <-- towerID
     * 03:11:32 [POS:Dump]       [ 1]    Integer: 140000224     <-- itemID
     * 03:11:32 [POS:Dump]       [ 2]    Integer: 16634         <-- typeID
     */
    _log(POS__TRACE,  "PosMgrBound::Handle_ChangeStructureProvisionType()");
    call.dump(POS__DUMP);

    /** @todo  finish this.. */

    return PyStatic.NewNone();
}

EVEResult PosMgrBound::LinkResourceForTower(EVECallArgs&call, PyInt* itemID, PyList* connections) {
    _log(POS__TRACE,  "PosMgrBound::Handle_LinkResourceForTower()");
    call.dump(POS__DUMP);

    return PyStatic.NewNone();
}

EVEResult PosMgrBound::RunMoonProcessCycleforTower(EVECallArgs&call, PyInt* itemID) {
    // this sets cycle 'on' for given towerID
    /*
     * 03:14:25 W PosMgrBound::Handle_RunMoonProcessCycleforTower(): size=1
     * 03:14:25 [POS:Dump]   Call Arguments:
     * 03:14:25 [POS:Dump]      Tuple: 1 elements
     * 03:14:25 [POS:Dump]       [ 0]    Integer: 140000061     <-- towerID
     */
    _log(POS__TRACE,  "PosMgrBound::Handle_RunMoonProcessCycleforTower()");
    call.dump(POS__DUMP);

    return PyStatic.NewNone();
}

EVEResult PosMgrBound::GMUpgradeOrbital(EVECallArgs&call, PyInt* itemID) {
    /*
     *  def GMUpgradeOrbital(self, itemID):
     *      posMgr = util.Moniker('posMgr', session.solarsystemid)
     *      posMgr.GMUpgradeOrbital(itemID)
     */
    _log(POS__TRACE,  "PosMgrBound::Handle_GMUpgradeOrbital()");
    call.dump(POS__DUMP);

    return PyStatic.NewNone();
}

EVEResult PosMgrBound::CompleteOrbitalStateChange(EVECallArgs&call, PyInt* itemID) {
    /*
     *  def CompleteOrbitalStateChange(self, itemID):
     *      posMgr = util.Moniker('posMgr', session.solarsystemid)
     *      posMgr.CompleteOrbitalStateChange(itemID)
     */
    _log(POS__TRACE,  "PosMgrBound::Handle_CompleteOrbitalStateChange()");
    call.dump(POS__DUMP);

    return PyStatic.NewNone();
}

