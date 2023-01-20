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
    Rewrite:    Allan
*/

#include "../eve-common.h"

#include "python/PyVisitor.h"
//#include "python/PyRep.h"
#include "python/PyTuple.h"
#include "python/PyList.h"
#include "python/PyDict.h"
#include "python/PyObject.h"
#include "python/PyObjectEx.h"
#include "python/PyPackedRow.h"
#include "python/PySubStruct.h"
#include "python/PySubStream.h"

/************************************************************************/
/* PyVisitor                                                            */
/************************************************************************/
bool PyVisitor::VisitTuple(const PyTuple* rep)
{
    for (auto cur : *rep)
        if (!cur->visit(*this))
            return false;

    return true;
}

bool PyVisitor::VisitList(const PyList* rep)
{
    for (auto cur : *rep)
        if (!cur->visit(*this))
            return false;

    return true;
}

bool PyVisitor::VisitDict(const PyDict* rep)
{
    for (auto cur : *rep)
        if (!cur.first->visit (*this) || !cur.second->visit (*this))
            return false;

    return true;
}

bool PyVisitor::VisitObject(const PyObject* rep)
{
    return
        rep->type ()->visit (*this) &&
        rep->arguments ()->visit (*this);
}

bool PyVisitor::VisitObjectEx(const PyObjectEx* rep)
{
    return
        rep->header ()->visit (*this) &&
        rep->list ()->visit (*this) &&
        rep->dict ()->visit (*this);
}

bool PyVisitor::VisitPackedRow (const PyPackedRow* rep)
{
    if (!rep->header ()->visit (*this))
        return false;

    for (const auto* value : rep->fields ())
        if (!value->visit (*this))
            return false;

    return true;
}

bool PyVisitor::VisitSubStruct (const PySubStruct* rep)
{
    return rep->content ()->visit (*this);
}

bool PyVisitor::VisitSubStream (const PySubStream* rep)
{
    const PyDataType* data = rep->decoded ();

    return data != nullptr && data->visit (*this);
}
