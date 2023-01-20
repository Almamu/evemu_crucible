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

#ifndef __PYDICT_H__
#define __PYDICT_H__

#include "python/PyDataType.h"
#include "python/PyNone.h"
#include "python/utils/UnorderedMapWrapper.h"

class PyDict : public PyDataType
{
protected:
    // provide implementations for the hash and comparison of the storage
    class _hash : public std::function <PyDataType* (size_t)> {
    public:
        size_t operator() (const PyDataType* keyval) const {
            return (keyval == nullptr) ? PyNone::HASH_VALUE : keyval->hash ();
        }
    };

    class _comp : public std::function <PyDataType*(PyDataType*, bool)> {
    public:
        bool operator() (const PyDataType* arg1, const PyDataType* arg2) const {
            return arg1->equals (arg2);
        }
    };

public:
    typedef std::unordered_map <PyDataType*, PyDataType*, _hash, _comp> storage_type;
    typedef storage_type::iterator                                      iterator;
    typedef storage_type::const_iterator                                const_iterator;

    PyDict ();
    PyDict (std::initializer_list <storage_type::value_type> init);
    ~PyDict ();

    const_iterator begin () const;
    const_iterator end () const;

    size_t size () const;
    bool empty () const;

    /**
     * Wraps this dictionary to simplify access to it's elements
     *
     * @tparam T
     * @return
     */
    template<class Key, class Value>
    UnorderedMapWrapper<Key, Value> wrap () const;

    bool equals (const PyDataType* right) const override;
    bool visit (PyVisitor& v) const override;

    template <class T = PyDataType>
    T* get (PyDataType* key) const;
    void set (PyDataType* key, PyDataType* value);

    static PyType Type;

private:
    storage_type mItems;
};


#endif // !__PYDICT_H__
