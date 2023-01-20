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

#ifndef __EVEPACKETADDRESS_H__
#define __EVEPACKETADDRESS_H__

#include "python/PyDataType.h"

class EVEPacketAddress {
public:
    ~EVEPacketAddress () = default;

    enum Type
    {
        Node    = 1,
            /*
             * [0]   addressType
             * [1]   nodeID
             * [2]   service
             * [3]   callID
             */
        Client  = 2,
            /* "because it's ID doesn't match"
             * [0]   addressType
             * [1]   clientID
             * [2]   callID
             * [3]   service
             */
        Broadcast = 4,
            /*
             * [0]   addressType
             * [1]   broadcastID
             * [2]   narrowcast         // this part is not understood yet... something about specific nodes for bcast types
             * [3]   idtype
             */
        Any     = 8,
            /*
             * [0]   addressType
             * [1]   service
             * [2]   callID
             */
        Invalid = 16
    };

    Type type;
    int64_t objectID;
    int64_t callID;

    std::string service;
    std::string bcast_idtype;

    static EVEPacketAddress from (PyDataType* from);
    PyDataType* encode (PythonArena* arena) const;
};


#endif /* !__EVEPACKETADDRESS_H__ */
