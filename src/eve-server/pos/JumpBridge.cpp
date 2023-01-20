
/**
 * @name JumpBridge.cpp
 *   Class for POS JumpBridge Modules.
 *
 * @Author:           James
 * @date:   13 October 2021
 */

#include "pos/JumpBridge.h"
#include "system/sov/SovereigntyDataMgr.h"
#include "system/SystemManager.h"
#include "planet/Moon.h"

JumpBridgeSE::JumpBridgeSE(StructureItemRef structure, EVEServiceManager& services, SystemManager* system, const FactionData& data)
: StructureSE(structure, services, system, data)
{
    m_bridgeData = EVEPOS::JumpBridgeData();
}

void JumpBridgeSE::Init()
{
    StructureSE::Init();
    m_bridgeData.itemID = m_self->itemID();
    if (!m_db.GetBridgeData(m_bridgeData))
    {
        _log(POS__MESSAGE, "Jump Bridge %s(%u) has no saved bridge data.  Initializing default set.", m_self->name(), m_data.itemID);
        InitData();
        m_db.SaveBridgeData(m_bridgeData);
    }
}

void JumpBridgeSE::InitData()
{
    StructureSE::InitData();
    m_moonSE = m_system->GetClosestMoonSE(GetPosition())->GetMoonSE();
    m_data.anchorpointID = m_moonSE->GetID();

    m_bridgeData.itemID = m_data.itemID;
    m_bridgeData.towerID = m_data.towerID;
    m_bridgeData.corpID = m_corpID;
    m_bridgeData.allyID = m_allyID;
    m_bridgeData.systemID = m_system->GetID();
    m_bridgeData.allowCorp = false;
    m_bridgeData.allowAlliance = false;
}

void JumpBridgeSE::Process()
{
    StructureSE::Process();
}

void JumpBridgeSE::SetOnline()
{
    _log(SOV__DEBUG, "Onlining Jump Bridge...");

    // Checks to see if bridge can be brought online

    // Make sure the system is not being jammed
    SovereigntyData sovData = svDataMgr.GetSovereigntyData(m_self->locationID());
    if (sovData.jammerID != 0) {
        throw CustomError("This system is currently being jammed.");
    }

    // Make sure player is not in high-sec (configurable)
    if (!sConfig.world.highSecCyno) {
        if (m_system->GetSecValue() >= 0.5f) {
            throw CustomError("This module may not be used in high security space.");
        }
    }

    // Reload data from DB changes
    m_db.GetBridgeData(m_bridgeData);

    // Send a slim update which will contain the latest changes
    SendSlimUpdate();

    StructureSE::SetOnline();
}

void JumpBridgeSE::SetOffline()
{
    _log(SOV__DEBUG, "Offlining TCU... Resetting claim's hubID.");
    StructureSE::SetOffline();
}

PyDict* JumpBridgeSE::MakeSlimItem()
{
    _log(SE__SLIMITEM, "MakeSlimItem for JumpBridgeSE %u", m_self->itemID());
    _log(POS__SLIMITEM, "MakeSlimItem for JumpBridgeSE %u", m_self->itemID());

    PyDict *slim = new PyDict();
    // General module slim entries
    slim->set ("name", new PyString(m_self->itemName()));
    slim->set ("itemID", new PyInt(m_data.itemID));
    slim->set ("typeID", new PyInt(m_self->typeID()));
    slim->set ("posState", new PyInt(m_data.state));
    slim->set ("ownerID", new PyInt(m_ownerID));
    slim->set ("corpID", IsCorp(m_corpID) ? new PyInt(m_corpID) : PyStatic.NewNone());
    slim->set ("allianceID", IsAlliance(m_allyID) ? new PyInt(m_allyID) : PyStatic.NewNone());
    slim->set ("warFactionID", IsFaction(m_warID) ? new PyInt(m_warID) : PyStatic.NewNone());
    slim->set ("posTimestamp", new PyInt(m_data.timestamp));
    slim->set ("incapacitated", new PyInt(m_data.state == EVEPOS::StructureState::Incapacitated));
    slim->set ("posDelayTime", new PyInt(m_delayTime));
    slim->set ("controlTowerID", new PyInt(m_data.towerID));

    // Jump Bridge specific entries
    slim->set ("remoteStructureID", new PyInt(m_bridgeData.toItemID));
    slim->set ("remoteSystemID", new PyInt(m_bridgeData.toSystemID));

    if (is_log_enabled(POS__SLIMITEM))
    {
        _log(POS__SLIMITEM, "JumpBridgeSE::MakeSlimItem() - %s(%u)", GetName(), m_data.itemID);
        slim->dump(POS__SLIMITEM, "     ");
    }
    return slim;
}

void JumpBridgeSE::SendSlimUpdate()
{
    PyTuple* sItem = new PyTuple {
        new PyString ("OnSlimItemChange"),
        new PyTuple {
            new PyInt (m_data.itemID),
            new PyObject ("foo.SlimItem", new PyDict {
                {"name", new PyString(m_self->itemName())},
                {"itemID", new PyInt(m_data.itemID)},
                {"typeID", new PyInt(m_self->typeID())},
                {"ownerID", new PyInt(m_ownerID)},
                {"corpID", IsCorp(m_corpID) ? new PyInt(m_corpID) : PyStatic.NewNone()},
                {"allianceID", IsAlliance(m_allyID) ? new PyInt(m_allyID) : PyStatic.NewNone()},
                {"warFactionID", IsFaction(m_warID) ? new PyInt(m_warID) : PyStatic.NewNone()},
                {"posTimestamp", new PyInt(m_data.timestamp)},
                {"posState", new PyInt(m_data.state)},
                {"incapacitated", new PyInt(0)},
                {"posDelayTime", new PyInt(m_delayTime)},
                {"remoteStructureID", new PyInt(m_bridgeData.toItemID)},
                {"remoteSystemID", new PyInt(m_bridgeData.toSystemID)},
            })
        }
    };
    m_destiny->SendSingleDestinyUpdate(&sItem);
}