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

template<class Return, class BaseType>
struct element_casting {
    constexpr Return* operator ()(const BaseType* value) const {
        return value->template as<Return>();
    }
};

/**
 * Wrapper for std::vector that casts elements inside of it to the desired type on access.
 * This cast is performed using the "as" function inside the type
 *
 * @tparam CastTo The type all the vector elements are expected to be
 * @tparam BaseType The type stored in the vector
 */
template<class CastTo, class BaseType = PyDataType>
struct VectorWrapper {
    friend class PyTuple;
    friend class PyList;
protected:
    VectorWrapper (const std::vector<BaseType*>& container) : mContainer (container) {}

public:
    ~VectorWrapper () = default;
    using iterator = boost::transform_iterator <element_casting <CastTo, BaseType>, typename std::vector <BaseType*>::const_iterator>;

    iterator begin () const {
        return boost::make_transform_iterator(this->mContainer.begin(), element_casting <CastTo, BaseType>());
    }
    iterator end () const {
        return boost::make_transform_iterator(this->mContainer.end(), element_casting <CastTo, BaseType>());
    }

private:
    const std::vector<BaseType*>& mContainer;
};
