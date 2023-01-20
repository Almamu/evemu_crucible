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

#ifndef __PYBUFFER_H__
#define __PYBUFFER_H__

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
    PyBuffer (Iter first, Iter second);
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
     * @return The buffer object
     */
    const Buffer& content () const;
    /**
     * @return The length of the buffer
     */
    size_t size () const;

    int32 hash () const override;
    bool visit (PyVisitor& v) const override;
    bool equals (const PyDataType* right) const override;

    static PyType Type;

private:
    virtual ~PyBuffer ();

    const Buffer* const mValue;
    mutable int32 mHashCache;
};


#endif // !__PYBUFFER_H__
