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

#pragma once

#include "PyDataType.h"
#include "PyDict.h"
#include "PyList.h"

class PyObjectEx : public PyDataType
{
    friend PythonArena;
public:
    PyObjectEx (bool isType2, PyDataType* header, bool checkArenaOwnership = true);
    ~PyObjectEx ();

    [[nodiscard]] bool isType2 () const;
    [[nodiscard]] const PyDataType* header () const;
    [[nodiscard]] PyList* list ();
    [[nodiscard]] PyDict* dict ();
    [[nodiscard]] const PyList* list () const;
    [[nodiscard]] const PyDict* dict () const;

    [[nodiscard]] bool equals(const PyDataType* right) const override;
    [[nodiscard]] bool visit (PyVisitor& v) const override;
    [[nodiscard]] PyObjectEx* clone () const override;
    [[nodiscard]] PyObjectEx* clone (PythonArena* arena) const override;

    static PyType Type;

protected:
    PyObjectEx (bool isType2, PyDataType* header, PyList* list, PyDict* dict, bool checkArenaOwnership = true);

    const PyDataType* mHeader;
    const bool mIsType2;

    PyList* mList;
    PyDict* mDict;
};

class PyObjectEx_Type1 : public PyObjectEx {
    friend PythonArena;
public:
    PyObjectEx_Type1 (PyToken* type, PyTuple* args, bool enclosed = false, bool checkArenaOwnership = true);
    PyObjectEx_Type1 (PyObjectEx_Type1* args1, PyTuple* args2, bool enclosed = false, bool checkArenaOwnership = true);
    PyObjectEx_Type1 (PyToken* type, PyTuple* args, PyDict* keywords, bool enclosed = false, bool checkArenaOwnership = true);
    PyObjectEx_Type1 (PyToken* type, PyTuple* args, PyList* keywords, bool enclosed = false, bool checkArenaOwnership = true);

    [[nodiscard]] PyTuple* args () const;

    /**
     * Searches for the specified keyword if possible
     *
     * @param keyword
     * @return
     */
    [[nodiscard]] PyDataType* keyword (const char* keyword) const;

    [[nodiscard]] PyObjectEx_Type1* clone () const override;
    [[nodiscard]] PyObjectEx_Type1* clone (PythonArena* arena) const override;

protected:
    PyObjectEx_Type1 (PyDataType* header, PyList* list, PyDict* dict, bool checkArenaOwnership = true);

private:
    PyTuple* _CreateHeader (PyToken* type, PyTuple* args, bool enclosed = false, bool checkArenaOwnership = true);
    PyTuple* _CreateHeader (PyObjectEx_Type1* args1, PyTuple* args2, bool enclosed = false, bool checkArenaOwnership = true);
    PyTuple* _CreateHeader (PyToken* type, PyTuple* args, PyDict* keywords, bool enclosed = false, bool checkArenaOwnership = true);
    PyTuple* _CreateHeader (PyToken* type, PyTuple* args, PyList* keywords, bool enclosed = false, bool checkArenaOwnership = true);
};

class PyObjectEx_Type2 : public PyObjectEx {
    friend PythonArena;
public:
    PyObjectEx_Type2 (PyTuple* args, PyDict* keywords, bool enclosed = false, bool checkArenaOwnership = true);
    PyObjectEx_Type2 (PyToken* args, PyDict* keywords, bool enclosed = false, bool checkArenaOwnership = true);

    [[nodiscard]] PyTuple* args () const;

    /**
     * Searches for the specified keyword if possible
     *
     * @param keyword
     * @return
     */
    [[nodiscard]] PyDataType* keyword (const char* keyword) const;

    [[nodiscard]] PyObjectEx_Type2* clone () const override;
    [[nodiscard]] PyObjectEx_Type2* clone (PythonArena* arena) const override;

protected:
    PyObjectEx_Type2 (PyDataType* header, PyList* list, PyDict* dict, bool checkArenaOwnership = true);

private:
    PyTuple* _CreateHeader (PyTuple* args, PyDict* keywords, bool enclosed = false, bool checkArenaOwnership = true);
    PyTuple* _CreateHeader (PyToken* args, PyDict* keywords, bool enclosed = false, bool checkArenaOwnership = true);
};
