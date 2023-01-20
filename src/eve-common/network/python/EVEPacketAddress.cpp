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

EVEPacketAddress::EVEPacketAddress () :
    type (Invalid) {
}

EVEPacketAddress::EVEPacketAddress(const PyDataType* source) {
    if (source == nullptr)
        throw std::runtime_error ("EVEPacketAddress::EVEPacketAddress () - source is null");

    auto tuple = source->As <PyObject> ()->arguments ()->As <PyTuple> ();

    if (tuple->size () < 3)
        throw std::runtime_error ("EVEPacketAddress::EVEPacketAddress () - not enough elements in address tuple");

    switch (tuple->at <PyInt> (0)->value ())
    {
        case Any:
            this->type = Any;

            if (tuple->size () != 3)
                throw std::runtime_error ("EVEPacketAddress::EVEPacketAddress () - Any must have 3 elements");

            this->service = tuple->at (1)->string ();
            this->callID = tuple->at (2)->i64 ();
            break;

        case Node:
            this->type = Node;

            if (tuple->size () != 4)
                throw std::runtime_error ("EVEPacketAddress::EVEPacketAddress () - Node must have 4 elements");

            this->objectID = tuple->at (1)->i64 ();
            this->service = tuple->at (2)->string ();
            this->callID = tuple->at (3)->i64 ();
            break;

        case Client:
            this->type = Client;

            if (tuple->size () != 4)
                throw std::runtime_error ("EVEPacketAddress::EVEPacketAddress () - Client must have 4 elements");

            this->objectID = tuple->at (1)->i64 ();
            this->callID = tuple->at (2)->i64 ();
            this->service = tuple->at (3)->string ();
            break;

        case Broadcast:
            this->type = Broadcast;

            if (tuple->size () != 4)
                throw std::runtime_error ("EVEPacketAddress::EVEPacketAddress () - Broadcast must have 4 elements");

            // TODO: PROPERLY SUPPORT BROADCAST DATA (PyList for ids)
            this->service = tuple->at (1)->string ();
            this->bcast_idtype = tuple->at (3)->string ();
            break;

        default:
            throw std::runtime_error ("EVEPacketAddress::EVEPacketAddress () - unknown");
    }
}

EVEPacketAddress::~EVEPacketAddress () {}

PyDataType* EVEPacketAddress::operator=(const EVEPacketAddress &oth) {
    switch (oth.type)
    {
        case Any:
            return new PyTuple {
                new PyInt (oth.type),
                oth.service.empty () ? PyStatic.NewNone () : new PyString (oth.service),
                oth.objectID == 0 ? PyStatic.NewNone () : new PyInt (oth.objectID)
            };
            break;

        case Node:
            return new PyTuple {
                new PyInt (oth.type),
                new PyInt (oth.objectID),
                oth.service.empty () ? PyStatic.NewNone () : new PyString (oth.service),
                oth.callID == 0 ? PyStatic.NewNone () : new PyInt (oth.callID)
            };
            break;

        case Client:
            return new PyTuple {
                new PyInt (oth.type),
                new PyInt (oth.objectID),
                oth.callID == 0 ? PyStatic.NewNone () : new PyInt (oth.callID),
                oth.service.empty () ? PyStatic.NewNone () : new PyString (oth.service)
            };
            break;

        case Broadcast:
            return new PyTuple {
                new PyInt (oth.type),
                oth.service.empty () ? PyStatic.NewNone () : new PyString (oth.service),
                new PyList (), // TODO: PROPERLY SUPPORT BROADCAST DATA (PyList for ids)
                new PyString (oth.bcast_idtype)
            };
            break;

        default:
            throw std::runtime_error ("Cannot build PyTuple off an unknown type EVEPacketAddress");
    }
}
