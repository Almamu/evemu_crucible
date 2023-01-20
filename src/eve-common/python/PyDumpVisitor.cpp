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

#include "PyDumpVisitor.h"

#include "utils/EVEUtils.h"

#include "python/PySubStruct.h"
#include "python/PySubStream.h"
#include "python/PyPackedRow.h"
#include "python/PyObjectEx.h"
#include "python/PyString.h"
#include "python/PyBuffer.h"
#include "python/PyObject.h"
#include "python/PyTuple.h"
#include "python/PyToken.h"
#include "python/PyFloat.h"
#include "python/PyBool.h"
#include "python/PyList.h"
#include "python/PyDict.h"
#include "python/PyInt.h"

PyDumpVisitor::PyDumpVisitor(const char *pfx, bool fullNested) :
    PyPfxVisitor (pfx),
    mIsFullNested (fullNested) {
}

bool PyDumpVisitor::VisitNone (const PyNone* rep) {
    _print ("%s[PyNone]", _pfx ());
    return true;
}

bool PyDumpVisitor::VisitInteger (const PyInt* rep) {
    _print ("%s[PyInt %lli]", _pfx (), rep->value ());
    return true;
}

bool PyDumpVisitor::VisitReal (const PyFloat* rep) {
    _print ("%s[PyFloat %d]", _pfx (), rep->value ());
    return true;
}

bool PyDumpVisitor::VisitString (const PyString* rep) {
    if (IsPrintable (rep))
        _print ("%s[PyString %s]", _pfx (), rep->content ().c_str ());
    else
        _print ("%s[PyString binary %lli]", _pfx (), rep->content ().length ());

    return true;
}

bool PyDumpVisitor::VisitToken (const PyToken* rep) {
    _print ("%s[PyToken %s]", _pfx (), rep->content ().c_str ());
    return true;
}

bool PyDumpVisitor::VisitBuffer (const PyBuffer* rep) {
    _print ("%s[PyBuffer (%llu bytes): ", _pfx (), rep->content ().size ());

    _pfxExtend ("\t");
    _dump (_pfx (), &rep->content ()[0], rep->content ().size ());
    _pfxWithdraw ();

    return true;
}

bool PyDumpVisitor::VisitBoolean (const PyBool* rep) {
    _print ("%s[PyBool %s]", _pfx (), rep->value () ? "true" : "false");
    return true;
}

bool PyDumpVisitor::VisitTuple (const PyTuple* rep) {
    bool res = true;

    _print ("%s[PyTuple %lli items]", _pfx (), rep->size ());

    _pfxExtend ("\t");

    int i = 0;

    for (auto cur : *rep) {
        if (++i > 100 && !this->isFullNested ()) {
            _print ("%s ... truncated ...", _pfx ());
            break;
        }

        res = cur->visit (*this);

        if (res == false)
            break;
    }

    _pfxWithdraw ();

    return res;
}

bool PyDumpVisitor::VisitList (const PyList* rep) {
    bool res = true;

    _print ("%s[PyList %lli items]", _pfx (), rep->size ());

    _pfxExtend ("\t");

    int i = 0;

    for (auto cur : *rep) {
        if (++i > 100 && !this->isFullNested ()) {
            _print ("%s ... truncated ...", _pfx ());
            break;
        }

        res = cur->visit (*this);

        if (res == false)
            break;
    }

    _pfxWithdraw ();

    return res;
}

bool PyDumpVisitor::VisitDict (const PyDict* rep) {
    bool res = true;

    _print ("%s[PyDict %lli pairs]", _pfx (), rep->size ());

    _pfxExtend ("\t");

    int i = 0;

    for (auto cur : *rep) {
        if (++i > 100 && !this->isFullNested ()) {
            _print ("%s ... truncated ...", _pfx ());
            break;
        }

        res = cur.first->visit (*this);

        if (res == false)
            break;

        res = cur.second->visit (*this);

        if (res == false)
            break;
    }

    _pfxWithdraw ();

    return res;
}

bool PyDumpVisitor::VisitObject (const PyObject* rep) {
    _print ("%s[PyObject %s]", _pfx (), rep->type ()->content ().c_str ());

    _pfxExtend ("\t");
    bool res = rep->arguments ()->visit (*this);
    _pfxWithdraw ();

    return res;
}

bool PyDumpVisitor::VisitObjectEx (const PyObjectEx* rep) {
    _print ("%s[PyObjectEx %s]", rep->isType2 () ? "Type2" : "Type1");

    _pfxExtend ("\t");
    bool res = rep->header ()->visit (*this) && rep->list ().visit (*this) && rep->dict ().visit (*this);
    _pfxWithdraw ();

    return res;
}

bool PyDumpVisitor::VisitPackedRow (const PyPackedRow* rep) {
    _print ("%s[PyPackedRow %lli elements]", _pfx ());

    _pfxExtend ("\t");
    for (uint32_t i = 0; i < rep->header ()->count (); i ++) {
        _print ("%s[PyPackedRowColumn '%s' (%s)]", _pfx (), rep->header ()->name (i), rep->header ()->type (i));

        if (rep->get (i)->visit (*this) == false)
            break;
    }
    _pfxWithdraw ();

    return true;
}

bool PyDumpVisitor::VisitSubStruct (const PySubStruct* rep) {
    _print ("%s[PySubStruct]", _pfx ());

    _pfxExtend ("\t");
    bool result = rep->content ()->visit (*this);
    _pfxWithdraw ();

    return result;
}

bool PyDumpVisitor::VisitSubStream (const PySubStream* rep) {
    _print ("%s[PySubStream %s]", _pfx (), rep->isChecksumed () ? "checksumed" : "not checksumed");

    _pfxExtend ("\t");
    bool result = rep->decoded ()->visit (*this);
    _pfxWithdraw ();

    return result;
}