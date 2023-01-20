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

#include "PyNone.h"

PyNone::PyNone () : PyDataType (Type) {}
PyNone::~PyNone () { }

bool PyNone::visit(PyVisitor& v) const
{
    return v.VisitNone( this );
}

int32 PyNone::hash () const {
    // HACK: this is hackish AF but should get us by
    return HASH_VALUE;
}

bool PyNone::equals (const PyDataType* right) const {
    return this == right || right == nullptr || right->Is <PyNone> ();
}

PyDataType::PyType PyNone::Type = PyTypeNone;