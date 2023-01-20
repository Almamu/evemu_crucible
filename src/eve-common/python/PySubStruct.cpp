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

#include "PySubStruct.h"

PySubStruct::PySubStruct (PyDataType* content) :
    PyDataType (Type),
    mContent (content) {
}

bool PySubStruct::visit (PyVisitor& v) const {
    return v.VisitSubStruct (this);
}
bool PySubStruct::equals (const PyDataType* right) const {
    if (PyDataType::equals (right) == false)
        return false;

    return right->As <PySubStruct> ()->content ()->equals (this->content ());
}

PyDataType* PySubStruct::content () const {
    return this->mContent;
}

PyDataType::PyType PySubStruct::Type = PyTypeSubStruct;