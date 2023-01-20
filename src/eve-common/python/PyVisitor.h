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
    Author:     Zhur, mmcs, Bloody.Rabbit
*/

#pragma once

#include <stack>
#include <queue>

class PyInt;
class PyFloat;
class PyBool;
class PyNone;
class PyBuffer;
class PyString;
class PyToken;
class PyObject;
class PyObjectEx;
class PySubStruct;
class PySubStream;
class PyDict;
class PyList;
class PyTuple;
class PyPackedRow;

class PyVisitor
{
public:
    virtual ~PyVisitor() {}

    [[nodiscard]] virtual bool VisitNone (const PyNone* rep) { return true; }
    [[nodiscard]] virtual bool VisitInteger (const PyInt* rep) { return true; }
    [[nodiscard]] virtual bool VisitReal (const PyFloat* rep) { return true; }
    [[nodiscard]] virtual bool VisitString (const PyString* rep) { return true; }
    [[nodiscard]] virtual bool VisitToken (const PyToken* rep) { return true; }
    [[nodiscard]] virtual bool VisitBuffer (const PyBuffer* rep) { return true; }
    [[nodiscard]] virtual bool VisitBoolean (const PyBool* rep) { return true; }

    [[nodiscard]] virtual bool VisitTuple (const PyTuple* rep);
    [[nodiscard]] virtual bool VisitList (const PyList* rep);
    [[nodiscard]] virtual bool VisitDict (const PyDict* rep);

    [[nodiscard]] virtual bool VisitObject (const PyObject* rep);
    [[nodiscard]] virtual bool VisitObjectEx (const PyObjectEx* rep);

    [[nodiscard]] virtual bool VisitPackedRow (const PyPackedRow* rep);

    [[nodiscard]] virtual bool VisitSubStruct (const PySubStruct* rep);
    [[nodiscard]] virtual bool VisitSubStream (const PySubStream* rep);
};
