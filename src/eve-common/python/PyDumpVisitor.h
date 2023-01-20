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
    Author:     Zhur, mmcs
*/

#ifndef __PYDUMPVISITOR_H__
#define __PYDUMPVISITOR_H__

#include "PyVisitor.h"
#include "PyPfxVisitor.h"

class PyDumpVisitor : public PyPfxVisitor
{
public:
    PyDumpVisitor (const char* pfx = "", bool fullNested = false);

    bool isFullNested () const;

protected:
    // Output functions
    virtual void _print( const char* fmt, ... ) = 0;
    virtual void _dump( const char* pfx, const uint8* data, size_t len ) = 0;

    virtual bool VisitNone (const PyNone* rep);
    virtual bool VisitInteger (const PyInt* rep);
    virtual bool VisitReal (const PyFloat* rep);
    virtual bool VisitString (const PyString* rep);
    virtual bool VisitToken (const PyToken* rep);
    virtual bool VisitBuffer (const PyBuffer* rep);
    virtual bool VisitBoolean (const PyBool* rep);

    virtual bool VisitTuple (const PyTuple* rep);
    virtual bool VisitList (const PyList* rep);
    virtual bool VisitDict (const PyDict* rep);

    virtual bool VisitObject (const PyObject* rep);
    virtual bool VisitObjectEx (const PyObjectEx* rep);

    virtual bool VisitPackedRow (const PyPackedRow* rep);

    virtual bool VisitSubStruct (const PySubStruct* rep);
    virtual bool VisitSubStream (const PySubStream* rep);
private:
    const bool mIsFullNested;
};


#endif /* !__PYDUMPVISITOR_H__ */
