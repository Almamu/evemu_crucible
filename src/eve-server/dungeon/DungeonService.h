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
    Updates:    Allan
*/

#ifndef __DUNGEON_SERVICE_H_INCL__
#define __DUNGEON_SERVICE_H_INCL__

#include "system/SystemDB.h"
#include "services/Service.h"

class DungeonService : public Service <DungeonService> {
public:
    DungeonService();

protected:
  EVEResult IsObjectLocked(EVECallArgs& call, PyInt* objectID);
  EVEResult AddObject(EVECallArgs& call, PyInt* roomID, PyInt* tupeID, PyFloat* x, PyFloat* y, PyFloat* z, PyFloat* pitch, PyFloat* roll, PyFloat* radius);
  EVEResult RemoveObject(EVECallArgs& call, PyInt* objectID);
  EVEResult CopyObject(EVECallArgs& call, PyInt* objectID, PyInt* roomID, PyFloat* offsetX, PyFloat* offsetY, PyFloat* offsetZ);
  EVEResult EditObject(EVECallArgs& call);
  EVEResult EditObjectName(EVECallArgs& call, PyInt* newObjectID, PyString* objectName);
  EVEResult EditObjectRadius(EVECallArgs& call);
  EVEResult EditObjectXYZ(EVECallArgs& call);
  EVEResult EditObjectYawPitchRoll(EVECallArgs& call);
  EVEResult TemplateAdd(EVECallArgs& call, PyString* templateName, PyString* templateDescription);
  EVEResult TemplateRemove(EVECallArgs& call, PyInt* templateID);
  EVEResult TemplateEdit(EVECallArgs& call, PyInt* templateID, PyString* templateName, PyString* templateDescription);
  EVEResult AddTemplateObjects(EVECallArgs& call, PyInt* roomID, PyInt* objectID, PyTuple* position);
  EVEResult TemplateObjectAddDungeonList(EVECallArgs& call, PyInt* templateID, PyList* objectIDs);
  EVEResult GetArchetypes(EVECallArgs& call);
  EVEResult DEGetDungeons(EVECallArgs& call);
  EVEResult DEGetTemplates(EVECallArgs& call);
  EVEResult DEGetRooms(EVECallArgs& call);
  EVEResult DEGetRoomObjectPaletteData(EVECallArgs& call);
  EVEResult DEGetFactions(EVECallArgs& call);
};


#endif


