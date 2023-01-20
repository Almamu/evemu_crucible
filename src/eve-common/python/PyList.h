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

#include <initializer_list>

#include "python/utils/VectorWrapper.h"
#include "PyDataType.h"

class PyList : public PyDataType {
public:
    using storage_type = std::vector <PyDataType*>;
    using iterator = storage_type::iterator;
    using const_iterator = storage_type::const_iterator;
    using value_type = storage_type::value_type;

    explicit PyList ();
    explicit PyList (std::initializer_list<value_type> init, bool checkArenaOwnership = true);
    explicit PyList (storage_type data, bool checkArenaOwnership = true);
    explicit PyList (size_t size);
    ~PyList ();

    [[nodiscard]] const_iterator begin () const;
    [[nodiscard]] const_iterator end () const;

    [[nodiscard]] size_t size () const;
    [[nodiscard]] bool empty () const;

    /**
     * @return The first element in the list
     */
    [[nodiscard]] PyDataType* front() const;

    /**
     * @return The last element in the list
     */
    [[nodiscard]] PyDataType* back() const;

    /**
     * Wraps this tuple to simplify access to it's elements
     *
     * @tparam T
     * @return
     */
    template<class T>
    [[nodiscard]] VectorWrapper<T> wrap () const {
        return VectorWrapper <T> (this->mItems);
    }

    [[nodiscard]] PyDataType* at (int i) const  {
        return this->mItems.at(i);
    }

    template<class T>
    [[nodiscard]] T* at (int i) const  {
        return this->mItems.at(i)->as<T>();
    }

    void add(PyDataType* value, bool checkArenaOwnership = true);
    void set(int i, PyDataType* value, bool checkArenaOwnership = true);
    void clear();

    [[nodiscard]] bool equals (const PyDataType* right) const override;
    [[nodiscard]] bool visit (PyVisitor& v) const override;
    [[nodiscard]] PyList* clone () const override;
    [[nodiscard]] PyList* clone (PythonArena* arena) const override;

    static PyType Type;

    /**
     * @deprecated Public access to mItems will be removed once XMLPKTGEN is erased
     */
    storage_type mItems;
};
