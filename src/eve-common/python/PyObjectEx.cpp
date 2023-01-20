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

PyObjectEx::PyObjectEx (bool isType2, PyDataType* header, bool checkArenaOwnership) :
    PyDataType (Type),
    mIsType2 (isType2),
    mHeader (header),
    mList (new (this->arena()) PyList),
    mDict (new (this->arena()) PyDict) {
    if (checkArenaOwnership) {
        this->_checkArenaOwnership (header);
    }
}

PyObjectEx::PyObjectEx (bool isType2, PyDataType* header, PyList* list, PyDict* dict, bool checkArenaOwnership) :
    PyDataType (Type),
    mIsType2 (isType2),
    mHeader (header),
    mList (list),
    mDict (dict) {
    if (checkArenaOwnership) {
        this->_checkArenaOwnership (header);
        this->_checkArenaOwnership (list);
        this->_checkArenaOwnership (dict);
    }
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

PyList* PyObjectEx::list () {
    return this->mList;
}

const PyList* PyObjectEx::list () const {
    return this->mList;
}

PyDict* PyObjectEx::dict () {
    return this->mDict;
}

const PyDict* PyObjectEx::dict () const {
    return this->mDict;
}

bool PyObjectEx::equals (const PyDataType* right) const {
    if (PyDataType::equals (right) == false)
        return false;

    PyObjectEx* other = right->as<PyObjectEx>();

    return
        this->header ()->equals (other->header ()) &&
        this->list ()->equals (other->list ()) &&
        this->dict ()->equals (other->dict());
}

bool PyObjectEx::visit (PyVisitor& v) const {
    return v.VisitObjectEx (this);
}

PyObjectEx* PyObjectEx::clone () const {
    return this->clone (HeapPythonArena::instance);
}

PyObjectEx* PyObjectEx::clone (PythonArena* arena) const {
    return arena->ObjectEx (
        this->isType2(),
        this->header()->clone(arena),
        this->list()->clone(arena),
        this->dict()->clone(arena)
    );
}

PyDataType::PyType PyObjectEx::Type = PyTypeObjectEx;

PyObjectEx_Type1::PyObjectEx_Type1 (PyToken* type, PyTuple* args, bool enclosed, bool checkArenaOwnership) :
    PyObjectEx (false, _CreateHeader (type, args, enclosed, checkArenaOwnership)) {
}

PyObjectEx_Type1::PyObjectEx_Type1 (PyObjectEx_Type1* args1, PyTuple* args2, bool enclosed, bool checkArenaOwnership) :
    PyObjectEx (false, _CreateHeader (args1, args2, enclosed, checkArenaOwnership)) {
}

PyObjectEx_Type1::PyObjectEx_Type1 (PyToken* type, PyTuple* args, PyDict* keywords, bool enclosed, bool checkArenaOwnership) :
    PyObjectEx (false, _CreateHeader (type, args, keywords, enclosed, checkArenaOwnership)) {
}

PyObjectEx_Type1::PyObjectEx_Type1 (PyToken* type, PyTuple* args, PyList* keywords, bool enclosed, bool checkArenaOwnership) :
    PyObjectEx (false, _CreateHeader (type, args, keywords, enclosed, checkArenaOwnership)) {
}

PyObjectEx_Type1::PyObjectEx_Type1 (PyDataType* header, PyList* list, PyDict* dict, bool checkArenaOwnership) :
    PyObjectEx (false, header, list, dict, checkArenaOwnership) {
}

PyTuple* PyObjectEx_Type1::args () const {
    return this->header()->as<PyTuple>()->at <PyTuple> (1);
}

PyDataType* PyObjectEx_Type1::keyword (const char* keyword) const {
    auto* header = this->header()->as<PyTuple>();

    // TODO: ALLOW FOR KEYWORDS TO NOT BE SPECIFIED
    if (header->size () < 3)
        throw std::runtime_error ("PyObjectEx doesn't have keywords");

    for (const auto cur : *header->at <PyDict> (2))
    {
        if (cur.first->is<PyString>() == false)
            continue;
        if (cur.first->as<PyString>()->content () == keyword)
            return cur.second;
    }

    // TODO: THROW EXCEPTION HERE
    return nullptr;
}

PyObjectEx_Type1* PyObjectEx_Type1::clone () const {
    return this->clone (HeapPythonArena::instance);
}

PyObjectEx_Type1* PyObjectEx_Type1::clone (PythonArena* arena) const {
    return arena->ObjectEx_Type1 (
        this->header()->clone(arena),
        this->list()->clone(arena),
        this->dict()->clone(arena)
    );
}

PyTuple* PyObjectEx_Type1::_CreateHeader (PyToken* type, PyTuple* args, bool enclosed, bool checkArenaOwnership) {
    if (args == nullptr)
        args = this->arena()->Tuple ();

    if (checkArenaOwnership) {
        this->_checkArenaOwnership (type);
        this->_checkArenaOwnership (args);
    }

    return this->arena()->Tuple ({
        enclosed ? this->arena()->Tuple ({type}) : (PyDataType*) type,
        args
    });
}

PyTuple* PyObjectEx_Type1::_CreateHeader (PyObjectEx_Type1* args1, PyTuple* args2, bool enclosed, bool checkArenaOwnership) {
    if (args2 == nullptr)
        args2 = this->arena()->Tuple ();

    if (checkArenaOwnership) {
        this->_checkArenaOwnership (args1);
        this->_checkArenaOwnership (args2);
    }

    return this->arena()->Tuple ({
        enclosed ? this->arena()->Tuple ({args1}) : (PyDataType*) args1,
        args2
    });
}

PyTuple* PyObjectEx_Type1::_CreateHeader (PyToken* type, PyTuple* args, PyDict* keywords, bool enclosed, bool checkArenaOwnership) {
    if (enclosed)
        throw std::runtime_error ("This constructor is not supported yet");

    if (args == nullptr)
        args = this->arena()->Tuple ();

    if (checkArenaOwnership) {
        this->_checkArenaOwnership (type);
        this->_checkArenaOwnership (args);

        if (keywords != nullptr) {
            this->_checkArenaOwnership (keywords);
        }
    }

    if (keywords == nullptr)
        return this->arena()->Tuple ({type, args});
    else
        return this->arena()->Tuple ({type, args, keywords});
}

PyTuple* PyObjectEx_Type1::_CreateHeader (PyToken* type, PyTuple* args, PyList* keywords, bool enclosed, bool checkArenaOwnership) {
    if (enclosed)
        throw std::runtime_error ("This constructor is not supported yet");

    if (args == nullptr)
        args = this->arena()->Tuple ();

    if (checkArenaOwnership) {
        this->_checkArenaOwnership (type);
        this->_checkArenaOwnership (args);

        if (keywords != nullptr) {
            this->_checkArenaOwnership (keywords);
        }
    }

    if (keywords == nullptr)
        return this->arena()->Tuple ({type, args});
    else
        return this->arena()->Tuple ({type, args, keywords});
}

PyObjectEx_Type2::PyObjectEx_Type2 (PyTuple* args, PyDict* keywords, bool enclosed, bool checkArenaOwnership) :
    PyObjectEx (true, _CreateHeader (args, keywords, enclosed, checkArenaOwnership)) {
}

PyObjectEx_Type2::PyObjectEx_Type2 (PyToken* args, PyDict* keywords, bool enclosed, bool checkArenaOwnership) :
    PyObjectEx (true, _CreateHeader (args, keywords, enclosed, checkArenaOwnership)) {
}

PyObjectEx_Type2::PyObjectEx_Type2 (PyDataType* header, PyList* list, PyDict* dict, bool checkArenaOwnership) :
    PyObjectEx (true, header, list, dict, checkArenaOwnership) {
}

PyTuple* PyObjectEx_Type2::args () const {
    return this->header()->as<PyTuple>()->at <PyTuple> (0);
}

PyDataType* PyObjectEx_Type2::keyword (const char* keyword) const {
    auto* header = this->header()->as<PyTuple>();

    if (header->size () < 2)
        throw std::runtime_error ("PyObjectEx doesn't have keywords");

    for (const auto cur : *header->at <PyDict> (1))
    {
        if (cur.first->is<PyString>() == false)
            continue;
        if (cur.first->as<PyString>()->content () == keyword)
            return cur.second;
    }

    // TODO: THROW EXCEPTION HERE
    return nullptr;
}

PyObjectEx_Type2* PyObjectEx_Type2::clone () const {
    return this->clone (HeapPythonArena::instance);
}

PyObjectEx_Type2* PyObjectEx_Type2::clone (PythonArena* arena) const {
    return arena->ObjectEx_Type2 (
        this->header()->clone(arena),
        this->list()->clone(arena),
        this->dict()->clone(arena)
    );
}

PyTuple* PyObjectEx_Type2::_CreateHeader (PyTuple* args, PyDict* keywords, bool enclosed, bool checkArenaOwnership) {
    if (args == nullptr)
        throw std::runtime_error ("PyObjectEx_Type2 args is null");
    if (keywords == nullptr)
        keywords = this->arena()->Dict();

    if (checkArenaOwnership){
        this->_checkArenaOwnership (args);
        this->_checkArenaOwnership (keywords);
    }

    return this->arena()->Tuple ({args, keywords});
}

PyTuple* PyObjectEx_Type2::_CreateHeader (PyToken* args, PyDict* keywords, bool enclosed, bool checkArenaOwnership) {
    if (args == nullptr)
        throw std::runtime_error ("PyObjectEx_Type2 args is null");
    if (keywords == nullptr)
        keywords = this->arena()->Dict();

    if (checkArenaOwnership) {
        this->_checkArenaOwnership (args);
        this->_checkArenaOwnership (keywords);
    }

    if (enclosed)
        return this->arena()->Tuple ({
            this->arena()->Tuple ({args, keywords})
        });
    else
        return this->arena()->Tuple ({args, keywords});
}
