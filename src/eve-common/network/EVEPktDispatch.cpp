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
    Rewrite:    Allan
*/

#include "eve-common.h"

#include "network/EVEPktDispatch.h"
#include "python/EVECallStream.h"
#include "python/EVEPacket.h"
#include "python/PyVisitor.h"
#include "python/Types.h"

bool EVEPacketDispatcher::DispatchPacket(EVEPacket& packet) {
    switch(packet.type) {
        case CALL_REQ: {
            EVECallStream call = EVECallStream::from (packet.type_string, packet.payload);

            return Handle_CallReq(packet, call);
        }
        case CALL_RSP: {
            //check the string part, just for good measure
            if (packet.type_string != "macho.CallRsp") {
                sLog.Error("EVEPacketDispatcher","Received CALL_RSP with invalid type string '%s'", packet.type_string.c_str());
                return false;
            }

            //TODO: decode substream in tuple

            return Handle_CallRsp(packet);
        }
        case NOTIFICATION: {
            //check the string part, just for good measure
            if (packet.type_string != "macho.Notification") {
                sLog.Error("EVEPacketDispatcher","Received NOTIFICATION with invalid type string '%s'", packet.type_string.c_str());
                return false;
            }

            return Handle_Notify(packet);
        }
        case PING_REQ: {
            //check the string part, just for good measure
            if (packet.type_string != "macho.PingReq") {
                sLog.Error("EVEPacketDispatcher","Received PING_REQ with invalid type string '%s'", packet.type_string.c_str());
                return false;
            }

            return Handle_PingReq(packet);
        }
        case PING_RSP: {
            //check the string part, just for good measure
            if (packet.type_string != "macho.PingRsp") {
                sLog.Error("EVEPacketDispatcher","Received PING_RSP with invalid type string '%s'", packet.type_string.c_str());
                return false;
            }

            return Handle_PingRsp(packet);
        }
        default:
            return Handle_Other(packet);
    }
}

bool EVEPacketDispatcher::Handle_CallReq(EVEPacket& packet, EVECallStream& req)
{
    sLog.Error("EVEPacketDispatcher","Unhandled Call Request");
    return false;
}
bool EVEPacketDispatcher::Handle_CallRsp(EVEPacket& packet)
{
    sLog.Error("EVEPacketDispatcher","Unhandled Call Response");
    return false;
}
bool EVEPacketDispatcher::Handle_Notify(EVEPacket& packet)
{
    sLog.Error("EVEPacketDispatcher","Unhandled Notification");
    return false;
}
bool EVEPacketDispatcher::Handle_PingReq(EVEPacket& packet)
{
    sLog.Error("EVEPacketDispatcher","Unhandled Ping Request.");
    return false;
}
bool EVEPacketDispatcher::Handle_PingRsp(EVEPacket& packet)
{
    sLog.Error("EVEPacketDispatcher","Unhandled Ping Response.");
    return false;
}

bool EVEPacketDispatcher::Handle_Other(EVEPacket& packet)
{
    sLog.Error("EVEPacketDispatcher","Unhandled Packet of type %s (%i)", MACHONETMSG_TYPE_NAMES[ packet.type ], (int)packet.type);
    return false;
}
