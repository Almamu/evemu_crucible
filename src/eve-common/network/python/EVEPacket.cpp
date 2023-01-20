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

#include "EVEPacket.h"

#include "python/PyStatic.h"
#include "python/PyInt.h"

const char* MACHONETMSG_TYPE_NAMES[MACHONETMSG_TYPE_COUNT] =
{
    "AUTHENTICATION_REQ",
    "AUTHENTICATION_RSP",
    "IDENTIFICATION_REQ",
    "IDENTIFICATION_RSP",
    "U4",
    "U5",
    "CALL_REQ",
    "CALL_RSP",
    "TRANSPORTCLOSED",
    "U9",
    "RESOLVE_REQ",
    "RESOLVE_RSP",
    "NOTIFICATION",
    "U13",
    "U14",
    "ERRORRESPONSE",
    "SESSIONCHANGENOTIFICATION",
    "U17",
    "SESSIONINITIALSTATENOTIFICATION",
    "U19",
    "PING_REQ",
    "PING_RSP"
};

EVEPacket* EVEPacket::from (PyDataType* origin) {
    if (origin == nullptr)
        throw std::runtime_error ("EVEPacket::EVEPacket () - packet is null");
    if (origin->is<PySubStream>() == true) // checksumed stream
        origin = origin->as<PySubStream>()->decoded ();
    if (origin->is<PySubStream>() == true) // normal stream
        origin = origin->as<PySubStream>()->decoded ();

    auto obj = origin->as<PyObject>();
    auto args = obj->arguments()->as<PyTuple>();

    if (args->size () != 7)
        throw std::runtime_error ("EVEPacket::EVEPacket () - packet body tuple does not contain 7 elements");

    switch (args->at (0)->i64 ()) {
        case AUTHENTICATION_REQ:
        case AUTHENTICATION_RSP:
        case IDENTIFICATION_REQ:
        case IDENTIFICATION_RSP:
        case CALL_REQ:
        case CALL_RSP:
        case TRANSPORTCLOSED:
        case RESOLVE_REQ:
        case RESOLVE_RSP:
        case NOTIFICATION:
        case ERRORRESPONSE:
        case SESSIONCHANGENOTIFICATION:
        case SESSIONINITIALSTATENOTIFICATION:
        case PING_REQ:
        case PING_RSP:
            break;

        default:
            throw std::runtime_error ("EVEPacket::EVEPacket () - unknown packet type");
    }

    std::optional<PyDict*> named_payload = std::nullopt;
    std::optional<PyDataType*> contextKey = std::nullopt;

    if (args->at(5)->is<PyDict>()) {
        named_payload = args->at<PyDict>(5);
    }

    if (args->at(6)->is<PyNone>() == false) {
        contextKey = args->at(6);
    }

    return new EVEPacket {
        .type_string = obj->type()->content(),
        .type = (MACHONETMSG_TYPE) args->at (0)->i64 (),
        .source = EVEPacketAddress::from (args->at(1)),
        .dest = EVEPacketAddress::from (args->at(2)),
        .userid = args->at(3)->u32(),
        .payload = args->at <PyTuple> (4),
        .named_payload = named_payload,
        .contextKey = contextKey
    };
}

EVEPacket::~EVEPacket () {
    PySafeDecRef (this->payload);
    PyOptionalSafeDecRef (this->named_payload);
    PyOptionalSafeDecRef (this->contextKey);
}

PyDataType* EVEPacket::encode (PythonArena* arena) const {
    return arena->Object (
        arena->String (this->type_string),
        arena->Tuple ({
            arena->Int (this->type),
            this->source.encode(arena),
            this->dest.encode(arena),
            this->userid == 0 ? (PyDataType*) arena->None() : arena->Int (this->userid),
            this->payload,
            this->named_payload.has_value () ? (PyDataType*) this->named_payload.value () : arena->None(),
            this->contextKey.has_value () ? (PyDataType*) this->contextKey.value () : arena->None ()
        })
    );
}