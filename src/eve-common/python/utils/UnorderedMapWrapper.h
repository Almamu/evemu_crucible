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
    Author:     Almamu
*/

#ifndef __UNORDEREDMAPWRAPPER_H__
#define __UNORDEREDMAPWRAPPER_H__

#include <boost/iterator/transform_iterator.hpp>
#include <boost/functional.hpp>

#include "python/PyDataType.h"

template <class Key, class Value>
struct pair_casting {
    constexpr std::pair <Key*, Value*> operator ()(const std::pair<PyDataType*,PyDataType*>& value) const {
        return {value.first->As <Key>(), value.second->As <Value> ()};
    }
};

/**
 * Simple wrapper that allows simpler access to dictionary data
 *
 * @tparam Key The type the key will be casted to
 * @tparam Value The type the value will be casted to
 */
template<class Key, class Value>
struct UnorderedMapWrapper
{
    friend class PyDict;
protected:
    UnorderedMapWrapper (const std::unordered_map <PyDataType*, PyDataType*>& container);
    ~UnorderedMapWrapper ();

public:
    typedef boost::transform_iterator <pair_casting <Key, Value>, std::unordered_map <PyDataType*, PyDataType*>::const_iterator> iterator;

    iterator begin () const;
    iterator end () const;

private:
    const std::unordered_map <PyDataType*, PyDataType*>& mContainer;
};

#endif // !__UNORDEREDMAPWRAPPER_H__
