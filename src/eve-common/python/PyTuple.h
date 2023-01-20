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

#ifndef __PYTUPLE_H__
#define __PYTUPLE_H__

#include <initializer_list>

#include "PyDataType.h"
#include "python/utils/VectorWrapper.h"

class DBRowDescriptor;

class PyTuple : public PyDataType
{
    // tuples should not be modified, but there's some minimal exceptions
    // that should be allowed, hence this friend here
    friend class DBRowDescriptor;
public:
    typedef std::vector <PyDataType*>      storage_type;
    typedef storage_type::iterator         iterator;
    typedef storage_type::const_iterator   const_iterator;

    template <class... T>
    PyTuple (T&&... args);
    PyTuple (storage_type data);
    PyTuple (size_t size);
    ~PyTuple ();

    const_iterator begin () const;
    const_iterator end () const;

    size_t size () const;
    bool empty () const;

    /**
     * Wraps this tuple to simplify access to it's elements
     *
     * @tparam T
     * @return
     */
    template<class T>
    VectorWrapper<T> wrap () const;

    template<class T = PyDataType>
    T* at(int i) const;

    bool equals (const PyDataType* right) const override;
    bool visit (PyVisitor& v) const override;
    int32 hash () const override;

    static PyType Type;

private:
    std::vector <PyDataType*> mItems;
};

#endif // !__PYTUPLE_H__
