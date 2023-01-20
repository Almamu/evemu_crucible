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

#include "VectorWrapper.h"

template<class T>
VectorWrapper<T>::VectorWrapper (const std::vector<PyDataType*>& container) :
    mContainer (container) {
}

template<class T>
VectorWrapper<T>::~VectorWrapper () {}

template<class T>
typename VectorWrapper<T>::iterator VectorWrapper <T>::begin () const {
    return boost::make_transform_iterator(this->mContainer.begin(), element_casting <T>());
}

template<class T>
typename VectorWrapper<T>::iterator VectorWrapper <T>::end () const {
    return boost::make_transform_iterator(this->mContainer.end(), element_casting <T>());
}