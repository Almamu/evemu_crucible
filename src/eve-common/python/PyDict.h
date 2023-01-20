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

#include "PyDataType.h"
#include "PyNone.h"
#include "PyString.h"
#include "utils/UnorderedMapWrapper.h"

class PyDict : public PyDataType {
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
    using storage_type = std::unordered_map <PyDataType*, PyDataType*, _hash, _comp>;
    using iterator = storage_type::iterator;
    using const_iterator = storage_type::const_iterator;
    using value_type = storage_type::value_type;

    PyDict ();
    explicit PyDict (std::initializer_list <storage_type::value_type> init, bool checkArenaOwnership = true);
    explicit PyDict (std::initializer_list <std::map<std::string,PyDataType*>::value_type> init, bool checkArenaOwnership = true);
    explicit PyDict (storage_type storage, bool checkArenaOwnership = true);
    ~PyDict ();

    [[nodiscard]] const_iterator begin () const;
    [[nodiscard]] const_iterator end () const;

    [[nodiscard]] size_t size () const;
    [[nodiscard]] bool empty () const;

    /**
     * Wraps this dictionary to simplify access to it's elements
     *
     * @tparam T
     * @return
     */
    template<class Key, class Value>
    [[nodiscard]] UnorderedMapWrapper<Key, Value> wrap () const {
        return UnorderedMapWrapper <Key, Value> (mItems);
    }

    [[nodiscard]] bool equals (const PyDataType* right) const override;
    [[nodiscard]] bool visit (PyVisitor& v) const override;
    [[nodiscard]] PyDict* clone () const override;
    [[nodiscard]] PyDict* clone (PythonArena* arena) const override;

    template <class T>
    [[nodiscard]] T* get (PyDataType* key) const {
        auto it = this->mItems.find (key);

        if (it == this->mItems.end ())
            throw std::out_of_range ("Cannot find the given key in the dictionary");

        PySafeDecRef (key);

        return it->second->as<T>();
    }

    [[nodiscard]] PyDataType* get (PyDataType* key) const {
        auto it = this->mItems.find (key);

        if (it == this->mItems.end ())
            throw std::out_of_range ("Cannot find the given key in the dictionary");

        return it->second;
    }

    template <class T>
    [[nodiscard]] T* get (const std::string& key) const {
        auto keyRep = new(this->arena()) PyString (key);
        auto it = this->mItems.find (keyRep);

        if (it == this->mItems.end ()) {
            PyDecRef (keyRep);
            throw std::out_of_range ("Cannot find the given key in the dictionary");
        }

        PyDecRef (keyRep);

        return it->second->as<T>();
    }

    [[nodiscard]] PyDataType* get (const std::string& key) const {
        auto keyRep = new(this->arena()) PyString (key);
        auto it = this->mItems.find (keyRep);

        if (it == this->mItems.end ()) {
            PyDecRef (keyRep);
            throw std::out_of_range ("Cannot find the given key in the dictionary");
        }

        PySafeDecRef (keyRep);

        return it->second;
    }

    void set (PyDataType* key, PyDataType* value, bool checkArenaOwnership = true);
    void set (const std::string& key, PyDataType* value, bool checkArenaOwnership = true);

    static PyType Type;

private:
    storage_type mItems;
};
