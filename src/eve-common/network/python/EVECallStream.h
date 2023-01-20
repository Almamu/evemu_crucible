/*
    ------------------------------------------------------------------------------------
    LICENSE:
    ------------------------------------------------------------------------------------
    This file is part of EVEmu: EVE Online Server Emulator
    Copyright 2006 - 2023 The EVEmu Team
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
    Author:        Almamu
*/

#ifndef __PYCALLSTREAM_H__
#define __PYCALLSTREAM_H__

#include <optional>

#include "python/PyDataType.h"
#include "python/PySubStream.h"
#include "python/PyTuple.h"
#include "python/PyDict.h"

class EVECallStream
{
public:
    EVECallStream ();
    EVECallStream (const std::string& type, PyTuple* payload);
    ~EVECallStream ();

    uint32_t remoteObject;
    std::string remoteObjectStr;

    std::string method;
    const PyTuple* arg_tuple;
    std::optional <const PyDict*> arg_dict;

    PyDataType* operator= (const EVECallStream& oth);
};


#endif /* !__PYCALLSTREAM_H__ */
