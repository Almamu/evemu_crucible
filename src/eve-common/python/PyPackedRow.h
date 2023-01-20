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

#ifndef __PYPACKEDROW_H__
#define __PYPACKEDROW_H__

#include "PyDataType.h"
#include "PyList.h"

#include "database/DBRowDescriptor.h"

class PyPackedRow : public PyDataType
{
public:
    PyPackedRow (DBRowDescriptor* header);
    PyPackedRow (DBRowDescriptor* header, std::initializer_list <PyDataType*> values);

    DBRowDescriptor* header () const;
    const PyList& fields () const;

    void set (uint32_t index, PyDataType* value);
    void set (const char* column, PyDataType* value);
    template <class T = PyDataType>
    T* get (uint32_t index) const;
    template <class T = PyDataType>
    T* get (const char* column) const;

    bool equals(const PyDataType* right) const override;
    bool visit (PyVisitor& v) const override;

protected:
    DBRowDescriptor* const mHeader;
    PyList mFields;

    static PyType Type;
};


#endif /* !__PYPACKEDROW_H__ */
