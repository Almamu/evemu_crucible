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

#include "PyToken.h"

template<typename Iter>
PyToken::PyToken (Iter first, Iter last) :
    PyDataType (Type),
    mHashCache (-1),
    mValue (first, last) {
}
PyToken::PyToken (std::string  str) :
    PyDataType (Type),
    mHashCache (-1),
    mValue (std::move(str))  {

}

const std::string& PyToken::content () const {
    return this->mValue;
}

size_t PyToken::size() const {
    return this->content ().size ();
}

bool PyToken::equals (const PyDataType* right) const {
    return PyDataType::equals (right) && this->content () == right->As <PyToken> ()->content ();
}

int32 PyToken::hash() const {
    if (this->mHashCache != -1)
        return this->mHashCache;

    if (this->mValue.length () > 0) {
        this->mHashCache = std::hash <std::string> {} (this->mValue);
    } else {
        this->mHashCache = 0;
    }

    return this->mHashCache;
}

bool PyToken::visit (PyVisitor& v) const {
    return v.VisitToken (this);
}

PyDataType::PyType PyToken::Type = PyTypeToken;