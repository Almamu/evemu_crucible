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

class PyToken : public PyDataType {
public:
    template <typename Iter>
    PyToken (Iter first, Iter last) :
        PyDataType (Type),
        mHashCache (-1),
        mValue (first, last, this->allocator<std::string>()) {
    }
    explicit PyToken (std::string str);
    // TODO: WRITE PROPER DESTRUCTOR HERE
    ~PyToken() = default;

    [[nodiscard]] const std::string& content () const;
    [[nodiscard]] size_t size() const;

    [[nodiscard]] bool equals (const PyDataType* right) const override;
    [[nodiscard]] int32 hash() const override;
    [[nodiscard]] bool visit (PyVisitor& v) const override;
    [[nodiscard]] PyToken* clone () const override;
    [[nodiscard]] PyToken* clone (PythonArena* arena) const override;

    static PyType Type;

private:
    const std::string mValue;
    mutable int32 mHashCache;
};
