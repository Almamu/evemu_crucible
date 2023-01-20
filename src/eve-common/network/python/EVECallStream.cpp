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

EVECallStream EVECallStream::from (const std::string& type, const PyTuple* payload) {
    if (type != "macho.CallReq")
        throw std::runtime_error ("EVECallStream::EVECallStream - packet payload has an unknown type");

    if (payload->size () != 1)
        throw std::runtime_error ("EVECallStream::EVECallStream - invalid tuple length");

    auto payload2 = payload->at <PyTuple> (0);

    if (payload2->size () != 2)
        throw std::runtime_error ("EVECallStream::EVECallStream - invalid tuple length for wrapper");

    auto ss = payload2->at <PySubStream> (1);
    auto maint = ss->decoded()->as<PyTuple>();

    if (maint->size () != 4)
        throw std::runtime_error ("EVECallStream::EVECallStream - main tuple is not 4 elements long");

    std::optional<PyDict*> arg_dict;

    if (maint->at(3)->is<PyDict>()) {
        arg_dict = maint->at<PyDict>(3);
    }

    return EVECallStream {
        .remoteObject = maint->at(0)->is<PyInt>() ? maint->at(0)->u32() : 0,
        .remoteObjectStr = maint->at (0)->is<PyString>() ? maint->at(0)->string() : "",
        .method = maint->at(1)->string(),
        .arg_tuple = maint->at <PyTuple> (2),
        .arg_dict = arg_dict
    };
}

PyDataType* EVECallStream::encode (PythonArena* arena) const {
    return arena->Tuple ({
        arena->Tuple ({
            arena->Int (this->remoteObject == 0 ? 1 : 0),
            arena->SubStream (
                arena->Tuple ({
                    this->remoteObject == 0 ? (PyDataType*)arena->String (this->remoteObjectStr) : (PyDataType*)arena->Int (this->remoteObject),
                    arena->String (this->method),
                    this->arg_tuple->clone(arena),
                    this->arg_dict.has_value () ? this->arg_dict.value ()->clone(arena) : PyStatic.NewNone ()
                })
            )
        }),
        PyStatic.NewNone ()
    });
}
