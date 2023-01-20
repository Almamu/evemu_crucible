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
    Updates:    Allan
    Rewrite:    Almamu
*/

#include "PySubStream.h"

#include "marshal/EVEMarshal.h"
#include "marshal/EVEUnmarshal.h"

PySubStream::PySubStream (PyDataType* content, bool checksumed, bool checkArenaOwnership) :
    PyDataType (Type),
    mContentDecoded (content),
    mChecksumed (checksumed) {
    if (checkArenaOwnership) {
        this->_checkArenaOwnership (content);
    }
}

PySubStream::PySubStream (const Buffer& content, bool checksumed) :
    PyDataType (Type),
    mContentEncoded (content),
    mChecksumed (checksumed) {
}

PySubStream::PySubStream (std::optional<Buffer> encoded, std::optional<PyDataType*> decoded, bool checksumed, bool checkArenaOwnership) :
    PyDataType (Type),
    mContentEncoded (encoded),
    mContentDecoded (decoded),
    mChecksumed (checksumed) {
    if (decoded.has_value() && checkArenaOwnership) {
        this->_checkArenaOwnership (decoded.value());
    }
}

bool PySubStream::visit (PyVisitor& v) const {
    return v.VisitSubStream (this);
}

bool PySubStream::equals (const PyDataType* right) const {
    if (PyDataType::equals (right) == false)
        return false;

    PySubStream* other = right->as<PySubStream>();

    return
        other->isChecksumed () == this->isChecksumed () &&
        other->decoded ()->equals (this->decoded ());
}

PySubStream* PySubStream::clone () const {
    return this->clone (HeapPythonArena::instance);
}

PySubStream* PySubStream::clone (PythonArena* arena) const {
    return arena->SubStream (mContentEncoded, mContentDecoded, this->isChecksumed(), true);
}

PyDataType* PySubStream::decoded () const {
    if (mContentDecoded.has_value()) {
        return mContentDecoded.value();
    }

    if (mContentEncoded.has_value() == false) {
        // this should never happen, but you never know
        throw std::runtime_error ("SubStream doesn't have a buffer nor an encoded value");
    }

    mContentDecoded = Unmarshal (mContentEncoded.value(), this->arena());
    return mContentDecoded.value();
}

const Buffer& PySubStream::encoded () const {
    if (mContentEncoded.has_value()) {
        return mContentEncoded.value();
    }

    if (mContentDecoded.has_value() == false) {
        // this should never happen, but you never know
        throw std::runtime_error ("SubStream doesn't have a buffer nor an encoded value");
    }

    mContentEncoded = Buffer();

    if (Marshal (mContentDecoded.value(), mContentEncoded.value()) == false)
        throw std::runtime_error ("Cannot marshal substream's contents");

    return mContentEncoded.value();
}

bool PySubStream::isChecksumed () const {
    return this->mChecksumed;
}

PyDataType::PyType PySubStream::Type = PyTypeSubStream;