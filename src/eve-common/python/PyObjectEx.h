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

#ifndef __PYOBJECTEX_H__
#define __PYOBJECTEX_H__

#include "PyDataType.h"
#include "PyDict.h"
#include "PyList.h"

class PyObjectEx : public PyDataType
{
public:
    PyObjectEx (bool isType2, PyDataType* header);
    ~PyObjectEx ();

    bool isType2 () const;
    const PyDataType* header () const;
    PyList& list ();
    PyDict& dict ();
    const PyList& list () const;
    const PyDict& dict () const;

    bool equals(const PyDataType* right) const override;
    bool visit (PyVisitor& v) const override;

    virtual PyDataType* keyword (const char* keyword) const;

    static PyType Type;

protected:
    const PyDataType* mHeader;
    const bool mIsType2;

    PyList mList;
    PyDict mDict;
};

class PyObjectEx_Type1 : public PyObjectEx
{
public:
    PyObjectEx_Type1 (PyToken* type, PyTuple* args, bool enclosed = false);
    PyObjectEx_Type1 (PyObjectEx_Type1* args1, PyTuple* args2, bool enclosed = false);
    PyObjectEx_Type1 (PyToken* type, PyTuple* args, PyDict* keywords, bool enclosed = false);
    PyObjectEx_Type1 (PyToken* type, PyTuple* args, PyList* keywords, bool enclosed = false);

    const PyTuple* args () const;

    /**
     * Searches for the specified keyword if possible
     *
     * @param keyword
     * @return
     */
    PyDataType* keyword (const char* keyword) const;

private:
    static PyTuple* _CreateHeader (PyToken* type, PyTuple* args, bool enclosed = false);
    static PyTuple* _CreateHeader (PyObjectEx_Type1* args1, PyTuple* args2, bool enclosed = false);
    static PyTuple* _CreateHeader (PyToken* type, PyTuple* args, PyDict* keywords, bool enclosed = false);
    static PyTuple* _CreateHeader (PyToken* type, PyTuple* args, PyList* keywords, bool enclosed = false);
};

class PyObjectEx_Type2 : public PyObjectEx
{
public:
    PyObjectEx_Type2 (PyTuple* args, PyDict* keywords, bool enclosed = false);
    PyObjectEx_Type2 (PyToken* args, PyDict* keywords, bool enclosed = false);

    const PyTuple* args () const;

private:
    static PyTuple* _CreateHeader (PyTuple* args, PyDict* keywords, bool enclosed = false);
    static PyTuple* _CreateHeader (PyToken* args, PyDict* keywords, bool enclosed = false);
};

#endif // !__PYOBJECTEX_H__
