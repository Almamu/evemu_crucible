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

#include "PyDataType.h"
#include "python/utils/VectorWrapper.h"

class DBRowDescriptor;

class PyTuple : public PyDataType {
    // tuples should not be modified, but there's some minimal exceptions
    // that should be allowed, hence this friend here
    friend class DBRowDescriptor;
public:
    using storage_type = std::vector<PyDataType*>;
    using iterator = storage_type::iterator;
    using const_iterator = storage_type::const_iterator;
    using value_type = storage_type::value_type;

    PyTuple();
    explicit PyTuple (std::initializer_list<value_type> init, bool checkArenaOwnership = true);
    explicit PyTuple (storage_type data, bool checkArenaOwnership = true);
    /**
     * @deprecated This is here to keep compatibility with XMLPKTGen, and will be removed at some point
     *
     * @param count
     */
    explicit PyTuple (size_t count);
    ~PyTuple ();

    [[nodiscard]] const_iterator begin () const;
    [[nodiscard]] const_iterator end () const;

    [[nodiscard]] size_t size () const;
    [[nodiscard]] bool empty () const;

    /**
     * Wraps this tuple to simplify access to it's elements
     *
     * @tparam T
     * @return
     */
    template<class T>
    [[nodiscard]] VectorWrapper<T, PyDataType> wrap () const {
        return VectorWrapper <T, PyDataType> (this->mItems);
    }

    [[nodiscard]] inline PyDataType* at(int i) const {
        return this->mItems.at(i);
    }

    template<class T>
    [[nodiscard]] inline T* at(int i) const {
        return this->mItems.at(i)->as<T> ();
    }

    [[nodiscard]] bool equals (const PyDataType* right) const override;
    [[nodiscard]] bool visit (PyVisitor& v) const override;
    [[nodiscard]] int32 hash () const override;
    [[nodiscard]] PyTuple* clone () const override;
    [[nodiscard]] PyTuple* clone (PythonArena* arena) const override;

    static PyType Type;

    /**
     * @deprecated Public access to mItems will be removed once XMLPKTGEN is erased
     */
    storage_type mItems;
};
