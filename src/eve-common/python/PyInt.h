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

#include "eve-core.h"
#include "PyDataType.h"

class PyInt : public PyDataType
{
public:
    explicit PyInt (int64 value);

    [[nodiscard]] int64 value () const;
    [[nodiscard]] bool visit (PyVisitor& v) const override;
    [[nodiscard]] int32 hash () const override;
    [[nodiscard]] bool equals (const PyDataType* right) const override;
    [[nodiscard]] PyInt* clone () const override;
    [[nodiscard]] PyInt* clone (PythonArena* arena) const override;

    explicit operator int64() const;
    // explicit operator uint64() const; // this one is unavailable, not sure why
    explicit operator int32() const;
    explicit operator uint32() const;
    explicit operator int16() const;
    explicit operator uint16() const;
    explicit operator int8() const;
    explicit operator uint8() const;

    static PyType Type;

protected:
    virtual ~PyInt();

private:
    int64 mValue;
};
