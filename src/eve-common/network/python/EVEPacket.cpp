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

EVEPacket::EVEPacket (PyDataType*& content) {
    PyDataType* base = content;
    const PyDataType* packet = base;
    content = nullptr;

    if (packet == nullptr)
        throw std::runtime_error ("EVEPacket::EVEPacket () - packet is null");

    try {
        if (packet->Is <PySubStream> () == true) // checksumed stream
            packet = packet->As <PySubStream> ()->decoded ();
        if (packet->Is <PySubStream> () == true) // normal stream
            packet = packet->As <PySubStream> ()->decoded ();

        auto obj = packet->As <PyObject> ();
        auto args = obj->arguments ()->As <PyTuple> ();

        if (args->size () != 7)
            throw std::runtime_error ("EVEPacket::EVEPacket () - packet body tuple does not contain 7 elements");

        this->type_string = obj->type ()->content ();

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
                type = (MACHONETMSG_TYPE) args->at (0)->i64 ();
                break;

            default:
                throw std::runtime_error ("EVEPacket::EVEPacket () - unknown packet tyep");
        }

        this->source = args->at (1);
        this->dest = args->at (2);
        this->userid = args->at (3)->i64 ();
        this->payload = args->at <PyTuple> (4);

        if (args->at (5)->Is <PyNone> ())
            this->named_payload = std::nullopt;
        else
            this->named_payload = args->at <PyDict> (5);

        if (args->at (6)->Is <PyNone> ())
            this->contextKey = std::nullopt;
        else
            this->contextKey = args->at (6);
    } catch (const std::runtime_error& ex) {
        PyDecRef (base);
        throw ex;
    }

    PyDecRef (base);
}

EVEPacket::~EVEPacket () {
    PySafeDecRef (this->payload);

    if (this->named_payload.has_value ())
        PySafeDecRef (this->named_payload.value ());
    if (this->contextKey.has_value ())
        PySafeDecRef (this->contextKey.value ());
}

PyDataType* EVEPacket::operator= (const EVEPacket& oth) {
    return new PyObject (
        new PyString (oth.type_string),
        new PyTuple {
            new PyInt (oth.type),
            oth.source,
            oth.dest,
            oth.userid == 0 ? PyStatic.NewNone () : new PyInt (oth.userid),
            oth.payload,
            oth.named_payload.has_value () ? oth.named_payload.value () : PyStatic.NewNone (),
            oth.contextKey.has_value () ? oth.contextKey.value () : PyStatic.NewNone ()
        }
    );
}