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

#include "EVENotificationStream.h"

#include "python/PySubStream.h"
#include "python/PyString.h"
#include "python/PyInt.h"

#include "python/PyStatic.h"
/*
TODO: REMOVE THIS? DOESN'T SEEM TO BE USED ANYWHERE
EVENotificationStream::EVENotificationStream (const std::string& type, const std::string& notifyType, PyDataType*& payload) {
    PyDataType* base = payload;
    const PyDataType* packet = base;
    payload = nullptr;

    if (packet == nullptr)
        throw std::runtime_error ("EVEPacket::EVEPacket () - packet is null");

    try {
        if (type != "macho.Notification")
            throw std::runtime_error ("EVENotificationStream::EVENotificationStream - notification payload has unknown type");

        auto tuple = packet->as<PyTuple>();

        if (tuple->size () != 2)
            throw std::runtime_error ("EVENotificationStream::EVENotificationStream - invalid tuple length");

        auto payload2 = tuple->at <PyTuple> (0);

        if (payload2->size () != 2)
            throw std::runtime_error ("EVENotificationStream::EVENotificationStream - invalid payload2 length");

        auto ss = payload2->at <PySubStream> (1);
        auto robjt = ss->decoded()->as<PyTuple>();

        if (robjt->size () != 2)
            throw std::runtime_error ("EVENotificationStream::EVENotificationStream - invalid robjt length");

        if (robjt->at(0)->is<PyInt>()) {
            this->remoteObject = robjt->at (0)->u32 ();
            this->remoteObjectStr = "";
        } else if (robjt->at(0)->is<PyString>()) {
            this->remoteObject = 0;
            this->remoteObjectStr = robjt->at (0)->string ();
        } else {
            throw std::runtime_error ("EVENotificationStream::EVENotificationStream - cannot determine wrapper");
        }

        auto subt = robjt->at <PyTuple> (1);

        if (subt->size () != 2)
            throw std::runtime_error ("EVENotificationStream::EVENotificationStream - invalid subt length");

        if (subt->at(0)->is<PyInt>() == false)
            throw std::runtime_error ("EVENotificationStream::EVENotificationStream - invalid subt first element");

        this->args = subt->at <PyTuple> (1);
        this->notifyType = notifyType;

        // ensure args are not deleted when decreasing refs to the base object
        PyIncRef (this->args);
    } catch (const std::runtime_error& ex) {
        PyDecRef (base);
        throw ex;
    }

    PyDecRef (base);
}
*/

EVENotificationStream::~EVENotificationStream () {
    PySafeDecRef (this->args);
}

PyTuple* EVENotificationStream::encode (PythonArena* arena) const {
    return arena->Tuple ({
        arena->Tuple ({
            arena->Int (0),
            arena->SubStream (
                arena->Tuple ({
                    arena->Int (0),
                    arena->Tuple ({
                        arena->Int (1),
                        // TODO: GET RID OF THIS CLONE ONCE THE ARENAS ARE PROPERLY SETUP FOR EVERYTHING
                        this->args->clone (arena)
                    })
                })
            )
        })
    });
}
