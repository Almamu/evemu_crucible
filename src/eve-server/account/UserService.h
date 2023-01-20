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
    Author:        Bloody.Rabbit
*/

#ifndef __USER_SERVICE__H__INCL__
#define __USER_SERVICE__H__INCL__

#include "services/Service.h"
#include "Client.h"

class UserService : public Service<UserService>
{
public:
    UserService();

protected:
  EVEResult GetRedeemTokens(EVECallArgs& call);
  EVEResult ReverseRedeem(EVECallArgs& call, PyInt* itemID);
  EVEResult GetCreateDate(EVECallArgs& call);
  EVEResult ReportISKSpammer(EVECallArgs& call, PyInt* characterID, PyInt* channelID);
  EVEResult ReportBot(EVECallArgs& call, PyInt* itemID);
  EVEResult ApplyPilotLicence(EVECallArgs& call, PyInt* itemID);
};

class MovementService : public Service<MovementService>
{
public:
    MovementService(EVEServiceManager *mgr);

protected:
    EVEServiceManager* m_manager;

    EVEResult ResolveNodeID(EVECallArgs& call, PyInt* newWorldSpaceId);
};

#endif /* __USER_SERVICE__H__INCL__ */
