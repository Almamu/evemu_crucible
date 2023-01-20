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
    Author:        Allan
*/

#include "eve-server.h"


#include "system/ScenarioService.h"

ScenarioService::ScenarioService() :
    Service("scenario")
{
    this->Add("ResetD", &ScenarioService::ResetD);
    this->Add("PlayDungeon", &ScenarioService::PlayDungeon);
    this->Add("EditRoom", &ScenarioService::EditRoom);
    this->Add("GotoRoom", &ScenarioService::GotoRoom);
    this->Add("GetDunObjects", &ScenarioService::GetDunObjects);
    this->Add("GetSelObjects", &ScenarioService::GetSelObjects);
    this->Add("IsSelectedByObjID", &ScenarioService::IsSelectedByObjID);
    this->Add("DuplicateSelection", &ScenarioService::DuplicateSelection);
    this->Add("SetSelectionByID", &ScenarioService::SetSelectionByID);
    this->Add("SetSelectedRadius", &ScenarioService::SetSelectedRadius);
    this->Add("SetRotate", &ScenarioService::SetRotate);
    this->Add("RotateSelected", &ScenarioService::RotateSelected);
    this->Add("JitterSelection", &ScenarioService::JitterSelection);
    this->Add("ArrangeSelection", &ScenarioService::ArrangeSelection);
    this->Add("DeleteSelected", &ScenarioService::DeleteSelected);
    this->Add("RefreshSelection", &ScenarioService::RefreshSelection);
}

EVEResult ScenarioService::ResetD(EVECallArgs& call)
{
    _log(DUNG__CALL, "ScenarioService::ResetD()" );
    call.dump(DUNG__CALL_DUMP);

    return nullptr;
}

EVEResult ScenarioService::PlayDungeon(EVECallArgs& call, PyInt* dungeonVID, PyInt* selectedRoom)
{
//PlayDungeon(dungeonVID, selectedRoom)
    _log(DUNG__CALL, "ScenarioService::PlayDungeon()" );
    call.dump(DUNG__CALL_DUMP);

    return nullptr;
}

EVEResult ScenarioService::EditRoom(EVECallArgs& call, PyInt* dungeonVID, PyInt* selectedRoom)
{
    //EditRoom(dungeonVID, selectedRoom)
    _log(DUNG__CALL, "ScenarioService::EditRoom()" );
    call.dump(DUNG__CALL_DUMP);

    return nullptr;
}

EVEResult ScenarioService::GotoRoom(EVECallArgs& call, PyInt* selectedRoom)
{
//GotoRoom(selectedRoom)
    _log(DUNG__CALL, "ScenarioService::GotoRoom()" );
    call.dump(DUNG__CALL_DUMP);

    return nullptr;
}

EVEResult ScenarioService::GetDunObjects(EVECallArgs& call)
{
//GetDunObjects() (returns list of slim items)
    _log(DUNG__CALL, "ScenarioService::GetDunObjects()" );
    call.dump(DUNG__CALL_DUMP);

    return nullptr;
}

EVEResult ScenarioService::GetSelObjects(EVECallArgs& call)
{
//GetSelObjects() (returns list of slim items)
    _log(DUNG__CALL, "ScenarioService::GetSelObjects()" );
    call.dump(DUNG__CALL_DUMP);

    return nullptr;
}

EVEResult ScenarioService::IsSelectedByObjID(EVECallArgs& call, PyInt* dunObjectID)
{
//IsSelectedByObjID(dunObjectID)
    _log(DUNG__CALL, "ScenarioService::IsSelectedByObjID()" );
    call.dump(DUNG__CALL_DUMP);

    return nullptr;
}

EVEResult ScenarioService::DuplicateSelection(EVECallArgs& call, PyInt* amount, PyFloat* x, PyFloat* y, PyFloat* z)
{
//DuplicateSelection(amount, X, Y, Z)
    _log(DUNG__CALL, "ScenarioService::DuplicateSelection()" );
    call.dump(DUNG__CALL_DUMP);

    return nullptr;
}

EVEResult ScenarioService::SetSelectionByID(EVECallArgs& call, PyList* ids)
{
//SetSelectionByID(ids)
    _log(DUNG__CALL, "ScenarioService::SetSelectionByID()" );
    call.dump(DUNG__CALL_DUMP);

    return nullptr;
}

EVEResult ScenarioService::SetSelectedRadius(EVECallArgs& call, PyFloat* minRadius, PyFloat* maxRadius)
{
//SetSelectedRadius(minRadius, maxRadius)
    _log(DUNG__CALL, "ScenarioService::SetSelectedRadius()" );
    call.dump(DUNG__CALL_DUMP);

    return nullptr;
}

EVEResult ScenarioService::SetRotate(EVECallArgs& call, PyFloat* yaw, PyFloat* pitch, PyFloat* roll)
{
//SetRotate(y, p, r)
    _log(DUNG__CALL, "ScenarioService::SetRotate()" );
    call.dump(DUNG__CALL_DUMP);

    return nullptr;
}

EVEResult ScenarioService::RotateSelected(EVECallArgs& call, PyFloat* yaw, PyFloat* pitch, PyFloat* roll)
{
//RotateSelected(yaw, pitch, roll)
    _log(DUNG__CALL, "ScenarioService::RotateSelected()" );
    call.dump(DUNG__CALL_DUMP);

    return nullptr;
}

EVEResult ScenarioService::JitterSelection(EVECallArgs& call, PyFloat* x, PyFloat* y, PyFloat* z)
{
//JitterSelection(X, Y, Z)
    _log(DUNG__CALL, "ScenarioService::JitterSelection()" );
    call.dump(DUNG__CALL_DUMP);

    return nullptr;
}

EVEResult ScenarioService::ArrangeSelection(EVECallArgs& call, PyFloat* x, PyFloat* y, PyFloat* z)
{
//ArrangeSelection(X, Y, Z)
    _log(DUNG__CALL, "ScenarioService::ArrangeSelection()" );
    call.dump(DUNG__CALL_DUMP);

    return nullptr;
}

EVEResult ScenarioService::DeleteSelected(EVECallArgs& call)
{
//DeleteSelected()
    _log(DUNG__CALL, "ScenarioService::DeleteSelected()" );
    call.dump(DUNG__CALL_DUMP);

    return nullptr;
}

EVEResult ScenarioService::RefreshSelection(EVECallArgs& call)
{
    //RefreshSelection()
    _log(DUNG__CALL, "ScenarioService::RefreshSelection()" );
    call.dump(DUNG__CALL_DUMP);

    return nullptr;
}
