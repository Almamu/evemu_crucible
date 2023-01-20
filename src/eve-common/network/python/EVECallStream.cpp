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

#include "EVECallStream.h"

#include "python/PySubStream.h"
#include "python/PyString.h"
#include "python/PyInt.h"

#include "python/PyStatic.h"

EVECallStream::EVECallStream () {
}

EVECallStream::EVECallStream (const std::string &type, PyTuple *payload) {
    if (type != "macho.CallReq")
        throw std::runtime_error ("EVECallStream::EVECallStream - packet payload has an unknown type");

    if (payload->size () != 1)
        throw std::runtime_error ("EVECallStream::EVECallStream - invalid tuple length");

    auto payload2 = payload->at <PyTuple> (0);

    if (payload2->size () != 2)
        throw std::runtime_error ("EVECallStream::EVECallStream - invalid tuple length for wrapper");

    auto ss = payload2->at <PySubStream> (1);
    auto maint = ss->decoded ()->As <PyTuple> ();

    if (maint->size () != 4)
        throw std::runtime_error ("EVECallStream::EVECallStream - main tuple is not 4 elements long");

    if (maint->at (0)->Is <PyInt> ()) {
        this->remoteObject = maint->at (0)->u32 ();
        this->remoteObjectStr = "";
    } else if (maint->at (0)->Is <PyString> ()) {
        this->remoteObject = 0;
        this->remoteObjectStr = maint->at (0)->string ();
    } else {
        throw std::runtime_error ("EVECallStream::EVECallStream - cannot determine wrapper");
    }

    this->method = maint->at (1)->string ();
    this->arg_tuple = maint->at <PyTuple> (2);

    if (maint->at (3)->Is <PyNone> ()) {
        this->arg_dict = std::nullopt;
    } else {
        this->arg_dict = maint->at <PyDict> (3);
    }
}

EVECallStream::~EVECallStream () {
    PySafeDecRef (this->arg_tuple);

    if (this->arg_dict.has_value ())
        PySafeDecRef (this->arg_dict.value ());
}

PyDataType* EVECallStream::operator=(const EVECallStream &oth) {
    return new PyTuple {
        new PyTuple {
            new PyInt (oth.remoteObject == 0 ? 1 : 0),
            new PySubStream (
                new PyTuple {
                        oth.remoteObject == 0 ? (PyDataType*) new PyString (oth.remoteObjectStr) : new PyInt (oth.remoteObject),
                        new PyString (oth.method),
                        oth.arg_tuple,
                        oth.arg_dict.has_value () ? oth.arg_dict.value () : PyStatic.NewNone ()
                }
            )
        },
        PyStatic.NewNone ()
    };
}