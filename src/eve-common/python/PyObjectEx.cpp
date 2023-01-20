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
#include "PyTuple.h"
#include "PyDict.h"
#include "PyString.h"
#include "PyObjectEx.h"

PyObjectEx::PyObjectEx (bool isType2, PyDataType* header) :
    PyDataType (Type),
    mIsType2 (isType2),
    mHeader (header),
    mList (),
    mDict () {
}

PyObjectEx::~PyObjectEx () {
    PySafeDecRef (this->mHeader);
}

bool PyObjectEx::isType2 () const {
    return this->mIsType2;
}

const PyDataType* PyObjectEx::header () const {
    return this->mHeader;
}

PyList& PyObjectEx::list () {
    return this->mList;
}

const PyList& PyObjectEx::list () const {
    return this->mList;
}

PyDict& PyObjectEx::dict () {
    return this->mDict;
}

const PyDict& PyObjectEx::dict () const {
    return this->mDict;
}

bool PyObjectEx::equals (const PyDataType* right) const {
    if (PyDataType::equals (right) == false)
        return false;

    PyObjectEx* other = right->As <PyObjectEx> ();

    return
        this->header ()->equals (other->header ()) &&
        this->list ().equals (&other->list ()) &&
        this->dict ().equals (&other->dict());
}

bool PyObjectEx::visit (PyVisitor& v) const {
    return v.VisitObjectEx (this);
}

PyDataType::PyType PyObjectEx::Type = PyTypeObject;

PyObjectEx_Type1::PyObjectEx_Type1 (PyToken* type, PyTuple* args, bool enclosed) :
    PyObjectEx (false, _CreateHeader (type, args, enclosed)) {

}

PyObjectEx_Type1::PyObjectEx_Type1 (PyObjectEx_Type1* args1, PyTuple* args2, bool enclosed) :
    PyObjectEx (false, _CreateHeader (args1, args2, enclosed)) {
}

PyObjectEx_Type1::PyObjectEx_Type1 (PyToken* type, PyTuple* args, PyDict* keywords, bool enclosed) :
    PyObjectEx (false, _CreateHeader (type, args, keywords, enclosed)) {
}

PyObjectEx_Type1::PyObjectEx_Type1 (PyToken* type, PyTuple* args, PyList* keywords, bool enclosed) :
    PyObjectEx (false, _CreateHeader (type, args, keywords, enclosed)) {
}

const PyTuple* PyObjectEx_Type1::args () const {
    return this->header ()->As <PyTuple> ()->at <PyTuple> (1);
}

PyTuple* PyObjectEx_Type1::_CreateHeader (PyToken* type, PyTuple* args, bool enclosed)
{
    if (args == nullptr)
        args = new PyTuple (0);

    return new PyTuple {
        enclosed ? new PyTuple {type} : (PyDataType*) type,
        args
    };
}

PyTuple* PyObjectEx_Type1::_CreateHeader (PyObjectEx_Type1* args1, PyTuple* args2, bool enclosed)
{
    if (args2 == nullptr)
        args2 = new PyTuple (0);

    return new PyTuple {
        enclosed ? new PyTuple {args1} : (PyDataType*) args1,
        args2
    };
}

PyTuple* PyObjectEx_Type1::_CreateHeader (PyToken* type, PyTuple* args, PyDict* keywords, bool enclosed)
{
    if (enclosed)
        throw std::runtime_error ("This constructor is not supported yet");

    if (args == nullptr)
        args = new PyTuple (0);

    if (keywords == nullptr)
        return new PyTuple {type, args};
    else
        return new PyTuple {type, args, keywords};
}

PyTuple* PyObjectEx_Type1::_CreateHeader (PyToken* type, PyTuple* args, PyList* keywords, bool enclosed)
{
    if (enclosed)
        throw std::runtime_error ("This constructor is not supported yet");

    if (args == nullptr)
        args = new PyTuple (0);

    if (keywords == nullptr)
        return new PyTuple {type, args};
    else
        return new PyTuple {type, args, keywords};
}

PyDataType* PyObjectEx_Type1::keyword (const char* keyword) const {
    auto* header = this->header ()->As <PyTuple> ();

    if (header->size () < 2)
        throw std::runtime_error ("PyObjectEx doesn't have keywords");

    for (const auto cur : *header->at <PyDict> (2))
    {
        if (cur.first->Is <PyString> () == false)
            continue;
        if (cur.first->As <PyString> ()->content () == keyword)
            return cur.second;
    }

    return nullptr;
}

PyObjectEx_Type2::PyObjectEx_Type2 (PyTuple* args, PyDict* keywords, bool enclosed) :
    PyObjectEx (true, _CreateHeader (args, keywords, enclosed)) {
}

PyObjectEx_Type2::PyObjectEx_Type2 (PyToken* args, PyDict* keywords, bool enclosed) :
    PyObjectEx (true, _CreateHeader (args, keywords, enclosed)) {
}

const PyTuple* PyObjectEx_Type2::args () const {
    return this->header ()->As <PyTuple> ()->at <PyTuple> (0);
}

PyTuple* PyObjectEx_Type2::_CreateHeader (PyTuple* args, PyDict* keywords, bool enclosed)
{
    if (args == nullptr)
        throw std::runtime_error ("PyObjectEx_Type2 args is null");
    if (keywords == nullptr)
        keywords = new PyDict;

    return new PyTuple {args, keywords};
}

PyTuple* PyObjectEx_Type2::_CreateHeader (PyToken* args, PyDict* keywords, bool enclosed)
{
    if (args == nullptr)
        throw std::runtime_error ("PyObjectEx_Type2 args is null");
    if (keywords == nullptr)
        keywords = new PyDict;
    if (enclosed)
        return new PyTuple {
            new PyTuple {args, keywords}
        };
    else
        return new PyTuple {args, keywords};
}
