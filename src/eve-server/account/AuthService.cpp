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
    Author:     Zhur
*/

#include "eve-server.h"

#include "config/EVEServerConfig.h"
#include "account/AuthService.h"

AuthService::AuthService() :
    Service ("authentication")
{
    this->Add("Ping", &AuthService::Ping);
    this->Add("GetPostAuthenticationMessage", &AuthService::GetPostAuthenticationMessage);
    this->Add("AmUnderage", &AuthService::AmUnderage);
    this->Add("AccruedTime", &AuthService::AccruedTime);
    this->Add("SetLanguageID", &AuthService::SetLanguageID);
}

EVEResult AuthService::Ping(EVECallArgs&call) {
    return call.arena.Int(GetFileTimeNow());
}

EVEResult AuthService::GetPostAuthenticationMessage(EVECallArgs&call)
{
    if (sConfig.account.loginMessage.empty())
        return call.arena.None();

    return call.arena.Object ("util.KeyVal", call.arena.Dict ({
        {"message", call.arena.String (sConfig.account.loginMessage)}
    }));
}

EVEResult AuthService::AmUnderage(EVECallArgs&call)
{
    //  return sm.RemoteSvc('authentication').AmUnderage()
    sLog.Warning("AuthService", "Handle_AmUnderage() size=%lli", call.tuple->size());
    call.dump(SERVICE__CALL_DUMP);

    // return boolean
    return call.arena.Bool(false);
}

EVEResult AuthService::AccruedTime(EVECallArgs&call)
{
    // return sm.RemoteSvc('authentication').AccruedTime()
    sLog.Warning("AuthService", "Handle_AccruedTime() size=%lli", call.tuple->size());
    call.dump(SERVICE__CALL_DUMP);

    return nullptr;
}

EVEResult AuthService::SetLanguageID(EVECallArgs&call, PyDataType* languageID)
{
    //sm.RemoteSvc('authentication').SetLanguageID(setlanguageID)
    sLog.Warning("AuthService", "Handle_SetLanguageID() size=%lli", call.tuple->size());
    call.dump(SERVICE__CALL_DUMP);

    return nullptr;
}
