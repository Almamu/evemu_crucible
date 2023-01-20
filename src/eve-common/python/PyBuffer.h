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

class PyBuffer : public PyDataType
{
public:
    /**
     * Creates a new PyBuffer of len filled with the specified byte value
     *
     * @param fill
     * @param len
     */
    PyBuffer (const uint8& fill, size_t len);
    template <typename Iter>
    PyBuffer (Iter first, Iter second) :
        PyDataType (Type),
        mValue (new Buffer (first, second)),
        mHashCache (-1) {
    }
    /**
     * Creates a new PyBuffer using the specified buffer as basis
     *
     * @param buffer The buffer to copy
     */
    PyBuffer (const Buffer& buffer);
    /**
     * Creates a new PyBuffer using the specified buffer as basis
     *
     * @param buffer The buffer to use, this class takes ownership of it
     */
    PyBuffer (Buffer** buffer);
    /**
     * Creates a new PyBuffer using the specified PyString as basis
     *
     * @param str
     */
    PyBuffer (const PyString& str);

    /**
     * @return The buffer object
     */
    [[nodiscard]] const Buffer& content () const;
    /**
     * @return The length of the buffer
     */
    [[nodiscard]] size_t size () const;

    [[nodiscard]] int32 hash () const override;
    [[nodiscard]] bool visit (PyVisitor& v) const override;
    [[nodiscard]] bool equals (const PyDataType* right) const override;
    [[nodiscard]] PyBuffer* clone () const override;
    [[nodiscard]] PyBuffer* clone (PythonArena* arena) const override;

    static PyType Type;

private:
    virtual ~PyBuffer ();

    const Buffer* const mValue;
    mutable int32 mHashCache;
};
