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
    Rewrite:    Allan
*/

/* Dungeon Logging
 * DUNG__ERROR
 * DUNG__WARNING
 * DUNG__INFO
 * DUNG__MESSAGE
 * DUNG__TRACE
 * DUNG__CALL
 * DUNG__CALL_DUMP
 * DUNG__RSP_DUMP
 * DUNG__DB_ERROR
 * DUNG__DB_WARNING
 * DUNG__DB_INFO
 * DUNG__DB_MESSAGE
 */

#include "eve-server.h"


#include "dungeon/DungeonService.h"
#include "dungeon/DungeonDB.h"
#include "system/cosmicMgrs/DungeonMgr.h"
#include "system/KeeperService.h"
#include "services/ServiceManager.h"

DungeonService::DungeonService() :
    Service("dungeon")
{
    // objects
    this->Add("IsObjectLocked", &DungeonService::IsObjectLocked);
    this->Add("AddObject", &DungeonService::AddObject);
    this->Add("RemoveObject", &DungeonService::RemoveObject);
    this->Add("CopyObject", &DungeonService::CopyObject);
    this->Add("EditObject", &DungeonService::EditObject);
    this->Add("EditObjectName", &DungeonService::EditObjectName);
    this->Add("EditObjectRadius", &DungeonService::EditObjectRadius);
    this->Add("EditObjectXYZ", &DungeonService::EditObjectXYZ);
    this->Add("EditObjectYawPitchRoll", &DungeonService::EditObjectYawPitchRoll);
    // templates
    this->Add("TemplateAdd", &DungeonService::TemplateAdd);
    this->Add("TemplateRemove", &DungeonService::TemplateRemove);
    this->Add("TemplateEdit", &DungeonService::TemplateEdit);
    this->Add("AddTemplateObjects", &DungeonService::AddTemplateObjects);
    this->Add("TemplateObjectAddDungeonList", &DungeonService::TemplateObjectAddDungeonList);
    // others
    this->Add("GetArchetypes", &DungeonService::GetArchetypes);
    this->Add("DEGetFactions", &DungeonService::DEGetFactions);
    this->Add("DEGetDungeons", &DungeonService::DEGetDungeons);
    this->Add("DEGetTemplates", &DungeonService::DEGetTemplates);
    this->Add("DEGetRooms", &DungeonService::DEGetRooms);
    this->Add("DEGetRoomObjectPaletteData", &DungeonService::DEGetRoomObjectPaletteData);
}

EVEResult DungeonService::IsObjectLocked(EVECallArgs& call, PyInt* objectID)
{
    //return sm.RemoteSvc('dungeon').IsObjectLocked(objectID)
    _log(DUNG__CALL,  "DungeonService::Handle_IsObjectLocked size: %lli", call.tuple->size());
    call.dump(DUNG__CALL_DUMP);

    // TODO: IMPLEMENT THE LOCKING MECHANISM

    return new PyTuple {
        new PyBool (false),
        new PyList ()
    };
}

EVEResult DungeonService::AddObject(EVECallArgs& call, PyInt* roomID, PyInt* tupeID, PyFloat* x, PyFloat* y, PyFloat* z, PyFloat* pitch, PyFloat* roll, PyFloat* radius)
{
    // (newObjectID, revisionID,) = sm.RemoteSvc('dungeon').AddObject(roomID, typeID, x, y, z, yaw, pitch, roll, radius)

    _log(DUNG__CALL,  "DungeonService::Handle_AddObject size: %lli", call.tuple->size());
    call.dump(DUNG__CALL_DUMP);

    if(call.tuple->size() != 9) {
        _log(SERVICE__ERROR, "Wrong number of arguments in call to AddObject");
        return nullptr;
    }

    Dungeon::RoomObject newObject;
    newObject.roomID = call.tuple->at (0)->i64();
    newObject.typeID = call.tuple->at (1)->u32();
    newObject.x = call.tuple->at (2)->decimal();
    newObject.y = call.tuple->at (3)->decimal();
    newObject.z = call.tuple->at (4)->decimal();
    newObject.yaw = call.tuple->at (5)->decimal();
    newObject.pitch = call.tuple->at (6)->decimal();
    newObject.roll = call.tuple->at (7)->decimal();
    newObject.radius = call.tuple->at (8)->decimal();

    uint32 groupID = DungeonDB::GetFirstGroupForRoom(newObject.roomID);

    newObject.objectID = DungeonDB::CreateObject(newObject.roomID, newObject.typeID, groupID, newObject.x, newObject.y, newObject.z, newObject.yaw, newObject.pitch, newObject.roll, newObject.radius);

    Client *pClient(call.client);

    GPoint objPos;
    objPos.x = newObject.x + pClient->GetSession()->decimal ("editor_room_x");
    objPos.y = newObject.y + pClient->GetSession()->decimal ("editor_room_y");
    objPos.z = newObject.z + pClient->GetSession()->decimal ("editor_room_z");

    ItemData dData(newObject.typeID, 1/*EVE SYSTEM*/, pClient->GetLocationID(), flagNone, "", objPos);
    InventoryItemRef iRef = InventoryItem::SpawnItem(sItemFactory.GetNextTempID(), dData);
    if (iRef.get() == nullptr) {// Failed to spawn the item
        throw CustomError("Failed to spawn the item");
        return nullptr;
    }

    DungeonEditSE* oSE;
    oSE = new DungeonEditSE(iRef, pClient->services(), pClient->SystemMgr(), newObject);

    // Add the object to the room
    pClient->services().Lookup <KeeperService>("keeper")->GetBound()->AddRoomObject(oSE);

    // Add the entity to the SystemManager
    pClient->SystemMgr()->AddEntity(oSE, false);

    return new PyTuple {
        new PyInt (newObject.objectID),
        new PyInt (1 /* dummy value */)
    };
}

EVEResult DungeonService::RemoveObject(EVECallArgs& call, PyInt* objectID)
{
    //sm.RemoteSvc('dungeon').RemoveObject(objectID)
    _log(DUNG__CALL,  "DungeonService::Handle_RemoveObject  size: %lli", call.tuple->size());
    call.dump(DUNG__CALL_DUMP);

    if(call.tuple->size() != 1) {
        _log(SERVICE__ERROR, "Wrong number of arguments in call to RemoveObject");
        return nullptr;
    }

    // Remove the object from the room
    call.client->services().Lookup <KeeperService>("keeper")->GetBound()->RemoveRoomObject(objectID->value());

    return nullptr;
}

EVEResult DungeonService::CopyObject(EVECallArgs& call, PyInt* objectID, PyInt* roomID, PyFloat* offsetX, PyFloat* offsetY, PyFloat* offsetZ)
{
    //newObjectID = sm.RemoteSvc('dungeon').CopyObject(objectID, roomID, offsetX, offsetY, offsetZ)
    _log(DUNG__CALL,  "DungeonService::Handle_CopyObject  size: %lli", call.tuple->size());
    call.dump(DUNG__CALL_DUMP);

    if(call.tuple->size() != 5) {
        _log(SERVICE__ERROR, "Wrong number of arguments in call to CopyObject");
        return nullptr;
    }

    // Copy the object to the room
    Dungeon::RoomObject newObject;
    newObject.roomID = roomID->value();
    newObject.typeID = call.client->services().Lookup <KeeperService>("keeper")->GetBound()->GetRoomObject(objectID->value())->GetData().typeID;
    newObject.x = call.client->services().Lookup <KeeperService>("keeper")->GetBound()->GetRoomObject(objectID->value())->GetData().x + offsetX->value();
    newObject.y = call.client->services().Lookup <KeeperService>("keeper")->GetBound()->GetRoomObject(objectID->value())->GetData().y + offsetY->value();
    newObject.z = call.client->services().Lookup <KeeperService>("keeper")->GetBound()->GetRoomObject(objectID->value())->GetData().z + offsetZ->value();
    newObject.yaw = call.client->services().Lookup <KeeperService>("keeper")->GetBound()->GetRoomObject(objectID->value())->GetData().yaw;
    newObject.pitch = call.client->services().Lookup <KeeperService>("keeper")->GetBound()->GetRoomObject(objectID->value())->GetData().pitch;
    newObject.roll = call.client->services().Lookup <KeeperService>("keeper")->GetBound()->GetRoomObject(objectID->value())->GetData().roll;
    newObject.radius = call.client->services().Lookup <KeeperService>("keeper")->GetBound()->GetRoomObject(objectID->value())->GetData().radius;

    uint32 groupID = DungeonDB::GetFirstGroupForRoom(newObject.roomID);

    newObject.objectID = DungeonDB::CreateObject(newObject.roomID, newObject.typeID, groupID, newObject.x, newObject.y, newObject.z, newObject.yaw, newObject.pitch, newObject.roll, newObject.radius);

    Client *pClient(call.client);

    GPoint objPos;

    objPos.x = newObject.x + pClient->GetSession()->decimal ("editor_room_x");
    objPos.y = newObject.y + pClient->GetSession()->decimal ("editor_room_y");
    objPos.z = newObject.z + pClient->GetSession()->decimal ("editor_room_z");

    ItemData dData(newObject.typeID, 1/*EVE SYSTEM*/, pClient->GetLocationID(), flagNone, "", objPos);
    InventoryItemRef iRef = InventoryItem::SpawnItem(sItemFactory.GetNextTempID(), dData);
    if (iRef.get() == nullptr) {// Failed to spawn the item
        throw CustomError("Failed to spawn the item");
        return nullptr;
    }

    DungeonEditSE* oSE;
    oSE = new DungeonEditSE(iRef, pClient->services(), pClient->SystemMgr(), newObject);

    // Add the object to the room
    pClient->services().Lookup <KeeperService>("keeper")->GetBound()->AddRoomObject(oSE);

    // Add the entity to the SystemManager
    pClient->SystemMgr()->AddEntity(oSE, false);

    return nullptr;
}

EVEResult DungeonService::EditObject(EVECallArgs& call)
{
    _log(DUNG__CALL,  "DungeonService::Handle_EditObject  size: %lli", call.tuple->size());
    call.dump(DUNG__CALL_DUMP);

    return nullptr;
}

EVEResult DungeonService::EditObjectName(EVECallArgs& call, PyInt* newObjectID, PyString* objectName)
{
    //sm.RemoteSvc('dungeon').EditObjectName(newObjectID, objectName)
    _log(DUNG__CALL,  "DungeonService::Handle_EditObjectName  size: %lli", call.tuple->size());
    call.dump(DUNG__CALL_DUMP);

    if(call.tuple->size() != 2) {
        _log(SERVICE__ERROR, "Wrong number of arguments in call to EditObjectName");
        return nullptr;
    }

    // Update the object name
    DungeonEditSE* oSE = call.client->services().Lookup <KeeperService>("keeper")->GetBound()->GetRoomObject(newObjectID->value());

    std::string newObjectName = objectName->string();
    oSE->Rename(newObjectName.c_str());

    return nullptr;
}

EVEResult DungeonService::EditObjectRadius(EVECallArgs& call)
{
    _log(DUNG__CALL,  "DungeonService::Handle_EditObjectRadius  size: %lli", call.tuple->size());
    call.dump(DUNG__CALL_DUMP);

    uint32 itemID = call.byname["objectID"]->as<PyInt>()->value();
    double radius = call.byname["radius"]->as<PyFloat>()->value();

    SystemEntity* entity = call.client->SystemMgr()->GetEntityByID(itemID);

    if (entity->IsDungeonEditSE() == false) {
        call.client->SendErrorMsg("The selected object is not part of the current dungeon");
        return nullptr;
    }

    DungeonEditSE* dungeonEntity = entity->GetDungeonEditSE();

    dungeonEntity->DestinyMgr()->SetRadius(radius, true);

    // save the position to the database
    DungeonDB::EditObjectRadius(dungeonEntity->GetData().objectID, radius);

    return nullptr;
}

EVEResult DungeonService::EditObjectXYZ(EVECallArgs& call)
{
    _log(DUNG__CALL,  "DungeonService::Handle_EditObjectXYZ  size: %lli", call.tuple->size());
    call.dump(DUNG__CALL_DUMP);

    uint32 itemID = call.byname["objectID"]->as<PyInt>()->value();
    double x = call.byname["x"]->as<PyFloat>()->value();
    double y = call.byname["y"]->as<PyFloat>()->value();
    double z = call.byname["z"]->as<PyFloat>()->value();

    SystemEntity* entity = call.client->SystemMgr()->GetEntityByID(itemID);

    if (entity->IsDungeonEditSE() == false) {
        call.client->SendErrorMsg("The selected object is not part of the current dungeon");
        return nullptr;
    }

    DungeonEditSE* dungeonEntity = entity->GetDungeonEditSE();

    // calculate the new position first
    GPoint roomPos = call.client->GetShipSE()->GetPosition();

    // set the new, correct position in space
    dungeonEntity->DestinyMgr()->SetPosition({
        roomPos.x + x,
        roomPos.y + y,
        roomPos.z + z
    });

    // save the position to the database
    DungeonDB::EditObjectXYZ(dungeonEntity->GetData().objectID, x, y, z);

    return nullptr;
}

EVEResult DungeonService::EditObjectYawPitchRoll(EVECallArgs& call)
{
    _log(DUNG__CALL,  "DungeonService::Handle_EditObjectYawPitchRoll  size: %lli", call.tuple->size());
    call.dump(DUNG__CALL_DUMP);

    uint32 itemID = call.byname["objectID"]->as<PyInt>()->value();
    double yaw = call.byname["yaw"]->as<PyFloat>()->value();
    double pitch = call.byname["pitch"]->as<PyFloat>()->value();
    double roll = call.byname["roll"]->as<PyFloat>()->value();

    SystemEntity* entity = call.client->SystemMgr()->GetEntityByID(itemID);

    if (entity->IsDungeonEditSE() == false) {
        call.client->SendErrorMsg("The selected object is not part of the current dungeon");
        return nullptr;
    }

    DungeonEditSE* dungeonEntity = entity->GetDungeonEditSE();

    // TODO: ADD SUPPORT FOR YAW, PITCH AND ROLL UPDATE

    // save the position to the database
    DungeonDB::EditObjectYawPitchRoll(dungeonEntity->GetData().objectID, yaw, pitch, roll);

    return nullptr;
}

EVEResult DungeonService::TemplateAdd(EVECallArgs& call, PyString* templateName, PyString* templateDescription)
{
    //templateID = sm.RemoteSvc('dungeon').TemplateAdd(templateName, templateDescription)
    _log(DUNG__CALL,  "DungeonService::Handle_TemplateAdd  size: %lli", call.tuple->size());
    call.dump(DUNG__CALL_DUMP);

    // Get the currently edited roomID from KeeperBound
    uint32 roomID = call.client->services().Lookup <KeeperService>("keeper")->GetBound()->GetCurrentRoomID();

    if(call.tuple->size() != 2) {
        _log(SERVICE__ERROR, "Wrong number of arguments in call to TemplateAdd");
        return nullptr;
    }

    // Convert to std::string
    std::string nameString = templateName->string();
    std::string descriptionString = templateDescription->string();

    // Create the new template
    uint32 templateID = DungeonDB::CreateTemplate(nameString, descriptionString, roomID);

    return new PyInt(templateID);
}

EVEResult DungeonService::TemplateRemove(EVECallArgs& call, PyInt* templateID)
{
    //sm.RemoteSvc('dungeon').TemplateRemove(self.sr.node.id)
    _log(DUNG__CALL,  "DungeonService::Handle_TemplateRemove  size: %lli", call.tuple->size());
    call.dump(DUNG__CALL_DUMP);

    DungeonDB::DeleteTemplate(call.tuple->at (0)->as<PyInt>()->value());

    return nullptr;
}

EVEResult DungeonService::TemplateEdit(EVECallArgs& call, PyInt* templateID, PyString* templateName, PyString* templateDescription)
{
    //dungeonSvc.TemplateEdit(self.templateRow.templateID, templateName, templateDescription)
    _log(DUNG__CALL,  "DungeonService::Handle_TemplateEdit  size: %lli", call.tuple->size());
    call.dump(DUNG__CALL_DUMP);

    DungeonDB::EditTemplate(call.tuple->at (0)->as<PyInt>()->value(), call.tuple->at (1)->as<PyString>()->content(), call.tuple->at (2)->as<PyString>()->content());

    return nullptr;
}

EVEResult DungeonService::AddTemplateObjects(EVECallArgs& call, PyInt* roomID, PyInt* objectID, PyTuple* position)
{
    // objectIDs = sm.RemoteSvc('dungeon').AddTemplateObjects(roomID, self.sr.node.id, (posInRoom.x, posInRoom.y, posInRoom.z)

    _log(DUNG__CALL,  "DungeonService::Handle_AddTemplateObjects  size: %lli", call.tuple->size());
    call.dump(DUNG__CALL_DUMP);

    if(call.tuple->size() != 3) {
        _log(SERVICE__ERROR, "Wrong number of arguments in call to AddTemplateObjects");
        return nullptr;
    }

    double posInRoomX = position->at (0)->as<PyFloat>()->value();
    double posInRoomY = position->at (1)->as<PyFloat>()->value();
    double posInRoomZ = position->at (2)->as<PyFloat>()->value();

    std::vector<Dungeon::RoomObject> objects;
    DungeonDB::GetTemplateObjects(objectID->value(), objects);

    Client *pClient(call.client);
    PyList* objectIDs = new PyList();

    uint32 groupID = DungeonDB::GetFirstGroupForRoom(roomID->value());

    // Spawn the items in the object list
    for (auto cur : objects) {
        GPoint objPos;

        // Relative position for the object to be spawned at
        objPos.x = posInRoomX + cur.x + pClient->GetSession()->decimal ("editor_room_x");
        objPos.y = posInRoomY + cur.y + pClient->GetSession()->decimal ("editor_room_y");
        objPos.z = posInRoomZ + cur.z + pClient->GetSession()->decimal ("editor_room_z");

        // Position to be stored in the DB
        double dbPosX = posInRoomX + cur.x;
        double dbPosY = posInRoomY + cur.y;
        double dbPosZ = posInRoomZ + cur.z;

        // Create the object in the database
        DungeonDB::CreateObject(roomID->value(), cur.typeID, groupID, dbPosX, dbPosY, dbPosZ, cur.yaw, cur.pitch, cur.roll, cur.radius);

        ItemData dData(cur.typeID, 1/*EVE SYSTEM*/, pClient->GetLocationID(), flagNone, "", objPos);
        InventoryItemRef iRef = InventoryItem::SpawnItem(sItemFactory.GetNextTempID(), dData);
        if (iRef.get() == nullptr) // Failed to spawn the item
            continue;
        DungeonEditSE* oSE;
        oSE = new DungeonEditSE(iRef, pClient->services(), pClient->SystemMgr(), cur);

        pClient->services().Lookup <KeeperService>("keeper")->GetBound()->AddRoomObject(oSE);
        pClient->SystemMgr()->AddEntity(oSE, false);

        objectIDs->add(new PyInt(oSE->GetData().objectID));
    }

    return objectIDs;
}

EVEResult DungeonService::TemplateObjectAddDungeonList(EVECallArgs& call, PyInt* templateID, PyList* objectIDs)
{
    //sm.RemoteSvc('dungeon').TemplateObjectAddDungeonList(templateID, objectIDList)
    _log(DUNG__CALL,  "DungeonService::Handle_TemplateObjectAddDungeonList  size: %lli", call.tuple->size());
    call.dump(DUNG__CALL_DUMP);

    return nullptr;
}

EVEResult DungeonService::GetArchetypes(EVECallArgs& call)
{
    /**     archetypes = sm.RemoteSvc('dungeon').GetArchetypes()
     *        archetypeOptions = [ (archetype.archetypeName, archetype.archetypeID) for archetype in archetypes ]
     */
    _log(DUNG__CALL,  "DungeonService::Handle_GetArchetypes  size: %lli", call.tuple->size());
    call.dump(DUNG__CALL_DUMP);

    DBQueryResult res;
    DungeonDB::GetArchetypes(res);

    return DBResultToCRowset(res);
}

EVEResult DungeonService::DEGetDungeons(EVECallArgs& call)
{
    /* dungeon = sm.RemoteSvc('dungeon').DEGetDungeons(archetypeID=archetypeID, factionID=factionID)
     * dungeon = sm.RemoteSvc('dungeon').DEGetDungeons(dungeonID=dungeonID)[0]
     * dungeon.dungeonNameID, dungeon.dungeonID, dungeon.factionID
     */

    //PyDataType *result = NULL;
    //dict args:
    // archetypeID, factionID
    // or dungeonVID

    // rows: status (1=RELEASE,2=TESTING,else Working Copy),
    //       dungeonVName
    //       dungeonVID

    _log(DUNG__CALL,  "DungeonService::Handle_DEGetDungeons  size: %lli", call.tuple->size());
    call.dump(DUNG__CALL_DUMP);

    DBQueryResult res;

    if (call.tuple->size() == 0) { // No args
        DungeonDB::GetDungeons(res);
    } else if (call.tuple->size() == 1) { // dungeonID
        DungeonDB::GetDungeons(call.byname["dungeonID"]->as<PyInt>()->value(), res);
    } else if (call.tuple->size() == 2) { // archetypeID, factionID
        DungeonDB::GetDungeons(call.byname["archetypeID"]->as<PyInt>()->value(), call.byname["factionID"]->as<PyInt>()->value(), res);
    }
    return DBResultToCRowset(res);
}

EVEResult DungeonService::DEGetTemplates(EVECallArgs& call)
{
        /*
        self.templateRows = sm.RemoteSvc('dungeon').DEGetTemplates()
        for row in self.templateRows:
            data = {'label': row.templateName,
             'hint': row.description != row.templateName and row.description or '',
             'id': row.templateID,
             'form': self}
             */

    _log(DUNG__CALL,  "DungeonService::Handle_DEGetTemplates  size: %lli", call.tuple->size());
    call.dump(DUNG__CALL_DUMP);

    DBQueryResult res;
    DungeonDB::GetTemplates(res, call.client->GetUserID());

    return DBResultToCRowset(res);
}

EVEResult DungeonService::DEGetRooms(EVECallArgs& call)
{
    //rooms = sm.RemoteSvc('dungeon').DEGetRooms(dungeonID=seldungeon.dungeonID)
    //dict arg: dungeonVID
    //PyDataType *result = NULL;

    //rows: roomName

    _log(DUNG__CALL,  "DungeonService::Handle_DEGetRooms  size: %lli", call.tuple->size());
    call.dump(DUNG__CALL_DUMP);

    DBQueryResult res;
    DungeonDB::GetRooms(call.byname["dungeonID"]->as<PyInt>()->value(), res);

    return DBResultToCRowset(res);
}

EVEResult DungeonService::DEGetRoomObjectPaletteData(EVECallArgs& call)
{
    /* Object Pallete Data = Object Groups 
     * roomObjectGroups = sm.RemoteSvc('dungeon').DEGetRoomObjectPaletteData()
     * id, name  in either dict or list (client can process both)
     */
    _log(DUNG__CALL,  "DungeonService::Handle_DEGetRoomObjectPaletteData  size: %lli", call.tuple->size());
    call.dump(DUNG__CALL_DUMP);

    DBQueryResult res;
    DungeonDB::GetGroups(res);

    std::map<uint16, Inv::TypeData> types;
    sDataMgr.GetTypes(types);

    PyList* list = new PyList();

    for (auto cur : types) {
        Inv::GrpData grp;
        sDataMgr.GetGroup(cur.second.groupID, grp);
        if (grp.catID == EVEDB::invCategories::Celestial) {
            list->add (
                new PyTuple {
                    new PyInt (cur.second.id),
                    new PyString (cur.second.name)
                }
            );
        }
    }

    return list;
}

EVEResult DungeonService::DEGetFactions(EVECallArgs& call)
{
    /* factions = sm.RemoteSvc('dungeon').DEGetFactions()
        factionOptions = [ (cfg.eveowners.Get(faction.factionID).name, faction.factionID) for faction in factions ]
        */
    _log(DUNG__CALL,  "DungeonService::Handle_DEGetFactions  size: %lli", call.tuple->size());
    call.dump(DUNG__CALL_DUMP);

    DBQueryResult res;
    DungeonDB::GetFactions(res);
    return DBResultToCRowset(res);
}

/*{'messageKey': 'DunAuthoringError', 'dataID': 17883918, 'suppressable': False, 'bodyID': 259674, 'messageType': 'warning', 'urlAudio': '', 'urlIcon': '', 'titleID': None, 'messageID': 830}
 * {'messageKey': 'DunBlacklistCannotWarp', 'dataID': 17880454, 'suppressable': False, 'bodyID': 258397, 'messageType': 'info', 'urlAudio': '', 'urlIcon': '', 'titleID': 258396, 'messageID': 2100}
 * {'messageKey': 'DunCantAnchor', 'dataID': 17883867, 'suppressable': False, 'bodyID': 259656, 'messageType': 'notify', 'urlAudio': '', 'urlIcon': '', 'titleID': None, 'messageID': 831}
 * {'messageKey': 'DunCantUseMWD', 'dataID': 17883442, 'suppressable': False, 'bodyID': 259505, 'messageType': 'notify', 'urlAudio': '', 'urlIcon': '', 'titleID': None, 'messageID': 832}
 * {'messageKey': 'DunGateLocked_ManyKeys', 'dataID': 17883928, 'suppressable': False, 'bodyID': 259678, 'messageType': 'info', 'urlAudio': '', 'urlIcon': '', 'titleID': 259677, 'messageID': 834}
 * {'messageKey': 'DunGateLocked_OneKey', 'dataID': 17883923, 'suppressable': False, 'bodyID': 259676, 'messageType': 'info', 'urlAudio': '', 'urlIcon': '', 'titleID': 259675, 'messageID': 835}
 * {'messageKey': 'DunGateLocked_ZeroKey', 'dataID': 17883939, 'suppressable': False, 'bodyID': 259682, 'messageType': 'info', 'urlAudio': '', 'urlIcon': '', 'titleID': 259681, 'messageID': 836}
 * {'messageKey': 'DunGateNPCsAround', 'dataID': 17883447, 'suppressable': False, 'bodyID': 259507, 'messageType': 'info', 'urlAudio': '', 'urlIcon': '', 'titleID': 259506, 'messageID': 837}
 * {'messageKey': 'DunGateNoSkill', 'dataID': 17883915, 'suppressable': False, 'bodyID': 259673, 'messageType': 'info', 'urlAudio': '', 'urlIcon': '', 'titleID': 259672, 'messageID': 838}
 * {'messageKey': 'DunGateTooFarAway', 'dataID': 17883844, 'suppressable': False, 'bodyID': 259648, 'messageType': 'info', 'urlAudio': '', 'urlIcon': '', 'titleID': 259647, 'messageID': 839}
 * {'messageKey': 'DunPodsCannotWarp', 'dataID': 17883452, 'suppressable': False, 'bodyID': 259509, 'messageType': 'info', 'urlAudio': '', 'urlIcon': '', 'titleID': 259508, 'messageID': 840}
 * {'messageKey': 'DunShipCannotWarp', 'dataID': 17883907, 'suppressable': False, 'bodyID': 259670, 'messageType': 'info', 'urlAudio': '', 'urlIcon': '', 'titleID': 259669, 'messageID': 841}
 */