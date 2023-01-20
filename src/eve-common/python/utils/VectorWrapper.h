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

#ifndef __VECTORWRAPPER_H__
#define __VECTORWRAPPER_H__

#include <boost/iterator/transform_iterator.hpp>
#include <boost/functional.hpp>

#include "python/PyDataType.h"

template<class Return>
struct element_casting {
    constexpr Return* operator ()(const PyDataType* value) const {
        return value->As <Return> ();
    }
};

/**
 * Simple wrapper that allows simpler access to tuple data
 *
 * @tparam T The type all the tuple's elements are expected to be
 */
template<class T>
struct VectorWrapper
{
    friend class PyTuple;
protected:
    VectorWrapper (const std::vector<PyDataType*>& container);
    ~VectorWrapper ();

public:
    typedef boost::transform_iterator <element_casting <T>, std::vector <PyDataType*>::const_iterator> iterator;

    iterator begin () const;
    iterator end () const;

private:
    const std::vector<PyDataType*>& mContainer;
};

#endif // !__VECTORWRAPPER_H__
