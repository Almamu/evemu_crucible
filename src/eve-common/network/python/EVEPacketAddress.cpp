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

#include "EVEPacketAddress.h"

#include "python/PyObject.h"
#include "python/PyTuple.h"
#include "python/PyInt.h"
#include "python/PyStatic.h"

EVEPacketAddress EVEPacketAddress::from(PyDataType* source) {
    if (source == nullptr)
        throw std::runtime_error ("EVEPacketAddress::EVEPacketAddress () - source is null");

    auto tuple = source->as<PyObject>()->arguments()->as<PyTuple>();

    if (tuple->size () < 3)
        throw std::runtime_error ("EVEPacketAddress::EVEPacketAddress () - not enough elements in address tuple");

    switch (tuple->at <PyInt> (0)->value ())
    {
        case Any:
            if (tuple->size () != 3)
                throw std::runtime_error ("EVEPacketAddress::EVEPacketAddress () - Any must have 3 elements");

            return {
                .type = Any,
                .callID = tuple->at(2)->i64(),
                .service = tuple->at(1)->string(),
            };

        case Node:
            if (tuple->size () != 4)
                throw std::runtime_error ("EVEPacketAddress::EVEPacketAddress () - Node must have 4 elements");

            return {
                .type = Node,
                .objectID = tuple->at(1)->i64(),
                .callID = tuple->at(3)->i64(),
                .service = tuple->at(2)->string(),
            };

        case Client:
            if (tuple->size () != 4)
                throw std::runtime_error ("EVEPacketAddress::EVEPacketAddress () - Client must have 4 elements");

            return {
                .type = Client,
                .objectID = tuple->at (1)->i64 (),
                .callID = tuple->at (2)->i64 (),
                .service = tuple->at (3)->string (),
            };

        case Broadcast:
            if (tuple->size () != 4)
                throw std::runtime_error ("EVEPacketAddress::EVEPacketAddress () - Broadcast must have 4 elements");

            // TODO: PROPERLY SUPPORT BROADCAST DATA (PyList for ids (tuple with type and id or int))
            return {
                .type = Broadcast,
                .service = tuple->at(1)->string(),
                .bcast_idtype = tuple->at(3)->string(),
            };

        default:
            throw std::runtime_error ("EVEPacketAddress::EVEPacketAddress () - unknown");
    }
}

PyDataType* EVEPacketAddress::encode (PythonArena* arena) const {
    PyTuple* content;

    // TODO: REPLACE THESE NONES?
    switch (this->type) {
        case Any:
            content = arena->Tuple ({
                arena->Int (this->type),
                this->service.empty () ? (PyDataType*) arena->None () : arena->String (this->service),
                this->objectID == 0 ? (PyDataType*) arena->None () : arena->Int (this->objectID)
            });
            break;

        case Node:
            content = arena->Tuple ({
                arena->Int (this->type),
                arena->Int (this->objectID),
                this->service.empty () ? (PyDataType*) arena->None () : arena->String (this->service),
                this->callID == 0 ? (PyDataType*) arena->None () : arena->Int (this->callID)
            });
            break;

        case Client:
            content = arena->Tuple ({
                arena->Int (this->type),
                arena->Int (this->objectID),
                this->callID == 0 ? (PyDataType*) arena->None () : arena->Int (this->callID),
                this->service.empty () ? (PyDataType*) arena->None () : arena->String (this->service)
            });
            break;

        case Broadcast:
            content = arena->Tuple ({
                arena->Int (this->type),
                this->service.empty () ? (PyDataType*) arena->None () : arena->String (this->service),
                arena->List (), // TODO: PROPERLY SUPPORT BROADCAST DATA (PyList for ids)
                arena->String (this->bcast_idtype)
            });
            break;

        default:
            throw std::runtime_error ("Cannot build PyTuple off an unknown type EVEPacketAddress");
    }

    return arena->Object ("macho.MachoAddress", content);
}
