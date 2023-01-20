/*
    ------------------------------------------------------------------------------------
    LICENSE:
    ------------------------------------------------------------------------------------
    This file is part of EVEmu: EVE Online Server Emulator
    Copyright 2006 - 2023 The EVEmu Team
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
    Author:        Almamu
*/

#ifndef __EVEPACKET_H__
#define __EVEPACKET_H__

#include <optional>

#include "network/packet_types.h"
#include "python/Types.h"

#include "EVEPacketAddress.h"

class EVEPacket {
public:
    ~EVEPacket ();

    std::string type_string;

    MACHONETMSG_TYPE type;
    EVEPacketAddress source;
    EVEPacketAddress dest;
    uint32_t userid;
    PyTuple* payload;
    std::optional <PyDict*> named_payload;

    // this one is not used on client<->server traffic
    // is used as extra context for node<->node traffic
    std::optional <PyDataType*> contextKey;

    static EVEPacket* from (PyDataType* origin);
    PyDataType* encode (PythonArena* arena) const;
};


#endif /* !__EVEPACKET_H__ */
