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

#ifndef __SCENARIO_SERVICE_H_INCL__
#define __SCENARIO_SERVICE_H_INCL__

#include "system/SystemDB.h"
#include "services/Service.h"

class ScenarioService : public Service <ScenarioService>
{
public:
    ScenarioService();

protected:
    SystemDB m_db;

    EVEResult ResetD(EVECallArgs& call);
    EVEResult PlayDungeon(EVECallArgs& call, PyInt* dungeonVID, PyInt* selectedRoom);
    EVEResult EditRoom(EVECallArgs& call, PyInt* dungeonVID, PyInt* selectedRoom);
    EVEResult GotoRoom(EVECallArgs& call, PyInt* selectedRoom);
    EVEResult GetDunObjects(EVECallArgs& call);
    EVEResult GetSelObjects(EVECallArgs& call);
    EVEResult IsSelectedByObjID(EVECallArgs& call, PyInt* dunObjectID);
    EVEResult DuplicateSelection(EVECallArgs& call, PyInt* amount, PyFloat* x, PyFloat* y, PyFloat* z);
    EVEResult SetSelectionByID(EVECallArgs& call, PyList* ids);
    EVEResult SetSelectedRadius(EVECallArgs& call, PyFloat* minRadius, PyFloat* maxRadius);
    EVEResult SetRotate(EVECallArgs& call, PyFloat* yaw, PyFloat* pitch, PyFloat* roll);
    EVEResult RotateSelected(EVECallArgs& call, PyFloat* yaw, PyFloat* pitch, PyFloat* roll);
    EVEResult JitterSelection(EVECallArgs& call, PyFloat* x, PyFloat* y, PyFloat* z);
    EVEResult ArrangeSelection(EVECallArgs& call, PyFloat* x, PyFloat* y, PyFloat* z);
    EVEResult DeleteSelected(EVECallArgs& call);
    EVEResult RefreshSelection(EVECallArgs& call);
};

#endif