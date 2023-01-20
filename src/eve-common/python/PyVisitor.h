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

#ifndef __PY_VISITOR_H__INCL__
#define __PY_VISITOR_H__INCL__

#include <stack>
#include <queue>

class PyRep;
class PyInt;
class PyLong;
class PyFloat;
class PyBool;
class PyNone;
class PyBuffer;
class PyString;
class PyWString;
class PyToken;
class PyObject;
class PyObjectEx;
class PySubStruct;
class PySubStream;
class PyChecksumedStream;
class PyDict;
class PyList;
class PyTuple;
class PyPackedRow;

class PyVisitor
{
public:
    virtual ~PyVisitor() {}

    virtual bool VisitNone (const PyNone* rep) { return true; }
    virtual bool VisitInteger (const PyInt* rep) { return true; }
    virtual bool VisitReal (const PyFloat* rep) { return true; }
    virtual bool VisitString (const PyString* rep) { return true; }
    virtual bool VisitToken (const PyToken* rep) { return true; }
    virtual bool VisitBuffer (const PyBuffer* rep) { return true; }
    virtual bool VisitBoolean (const PyBool* rep) { return true; }

    virtual bool VisitTuple (const PyTuple* rep);
    virtual bool VisitList (const PyList* rep);
    virtual bool VisitDict (const PyDict* rep);

    virtual bool VisitObject (const PyObject* rep);
    virtual bool VisitObjectEx (const PyObjectEx* rep);

    virtual bool VisitPackedRow (const PyPackedRow* rep);

    virtual bool VisitSubStruct (const PySubStruct* rep);
    virtual bool VisitSubStream (const PySubStream* rep);
};

#endif /* !__PY_VISITOR_H__INCL__ */
