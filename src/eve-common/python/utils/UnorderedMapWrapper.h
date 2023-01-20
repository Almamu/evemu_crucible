/*
    ------------------------------------------------------------------------------------
    LICENSE:
    ------------------------------------------------------------------------------------
    This file is part of EVEmu: EVE Online Server Emulator
    Copyright 2006 - 2025 The EVEmu Team
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
    Author:     Almamu
*/

#pragma once

#include <boost/iterator/transform_iterator.hpp>
#include <boost/functional.hpp>

#include "python/PyDataType.h"

template <class Key, class Value, class BaseKey, class BaseValue>
struct pair_casting {
    constexpr std::pair <Key*, Value*> operator ()(const std::pair<BaseKey*,BaseValue*>& value) const {
        return {value.first->as<Key>(), value.second->as<Value>()};
    }
};

/**
 * Wraper for std::unordered_map that casts elements inside of it to the desired type on access
 * This cast is performed using the "as" function inside the type
 *
 * @tparam Key The type the key will be casted to
 * @tparam Value The type the value will be casted to
 * @tparam BaseKey The type the key is in the unordered_map
 * @tparam BaseValue The type the value is in the unordered_map
 */
template<class Key, class Value, class BaseKey = PyDataType, class BaseValue = PyDataType>
struct UnorderedMapWrapper {
    friend class PyDict;
protected:
    UnorderedMapWrapper (const std::unordered_map <BaseKey*, BaseValue*>& container) : mContainer (container) {}
    ~UnorderedMapWrapper () = default;

public:
    using iterator = boost::transform_iterator <pair_casting <Key, Value, BaseKey, BaseValue>, typename std::unordered_map <BaseKey*, BaseValue*>::const_iterator>;

    iterator begin () const {
        return boost::make_transform_iterator (this->mContainer.begin (), pair_casting <Key, Value, BaseKey, BaseValue> ());
    }
    iterator end () const {
        return boost::make_transform_iterator (this->mContainer.end (), pair_casting <Key, Value, BaseKey, BaseValue> ());
    }

private:
    const std::unordered_map <BaseKey*, BaseValue*>& mContainer;
};
