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

#ifndef __PYLIST_H__
#define __PYLIST_H__

#include <initializer_list>

#include "python/utils/VectorWrapper.h"
#include "PyDataType.h"

class PyList : public PyDataType
{
public:
    typedef std::vector <PyDataType*>      storage_type;
    typedef storage_type::iterator         iterator;
    typedef storage_type::const_iterator   const_iterator;

    PyList ();
    template <class... T>
    PyList (T&&... args);
    PyList (storage_type data);
    PyList (size_t size);
    ~PyList ();

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
    T* at (int i) const;

    void add(PyDataType* value);
    void set(int i, PyDataType* value);

    bool equals (const PyDataType* right) const override;
    bool visit (PyVisitor& v) const override;

    static PyType Type;

private:
    storage_type mItems;
};

#endif // !__PYLIST_H__
