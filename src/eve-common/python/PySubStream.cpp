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

PySubStream::PySubStream (PyDataType* content, bool checksumed) :
    PyDataType (Type),
    mContentDecoded (content),
    mHaveEncoded (false),
    mChecksumed (checksumed) {
}

PySubStream::PySubStream (const Buffer& content, bool checksumed) :
    PyDataType (Type),
    mContentEncoded (content),
    mHaveEncoded (true),
    mChecksumed (checksumed) {
}

bool PySubStream::visit (PyVisitor& v) const {
    return v.VisitSubStream (this);
}
bool PySubStream::equals (const PyDataType* right) const {
    if (PyDataType::equals (right) == false)
        return false;

    PySubStream* other = right->As <PySubStream> ();

    return
        other->isChecksumed () == this->isChecksumed () &&
        other->decoded ()->equals (this->decoded ());
}

const PyDataType* PySubStream::decoded () const {
    if (this->mContentDecoded != nullptr)
        return this->mContentDecoded;

    return this->mContentDecoded = Unmarshal (this->mContentEncoded);
}

const Buffer& PySubStream::encoded () const {
    if (this->mHaveEncoded == true)
        return this->mContentEncoded;

    if (Marshal (this->mContentDecoded, this->mContentEncoded) == false)
        throw std::runtime_error ("Cannot marshal substream's contents");

    this->mHaveEncoded = true;

    return this->mContentEncoded;
}

bool PySubStream::isChecksumed () const {
    return this->mChecksumed;
}

PyDataType::PyType PySubStream::Type = PyTypeSubStream;