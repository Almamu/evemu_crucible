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
    Author:        Bloody.Rabbit
*/

#include "eve-common.h"
#include "EVEVersion.h"

#include "GPSTransportClosed.h"

#include "python/PyString.h"
#include "python/PyTuple.h"
#include "python/PyToken.h"
#include "python/PyFloat.h"
#include "python/PyInt.h"

GPSTransportClosed::GPSTransportClosed (const char* reason) :
    PyObjectEx_Type1 (new PyToken ("exceptions.GPSTransportClosed"), _CreateArgs (reason), _CreateKeywords (reason)) {
}

GPSTransportClosed::GPSTransportClosed (std::string& reason) :
    PyObjectEx_Type1 (new PyToken ("exceptions.GPSTransportClosed"), _CreateArgs (reason.c_str ()), _CreateKeywords (reason.c_str())) {
}

void GPSTransportClosed::addKeyword (const char* name, PyDataType* value) {
    this->reasonArgs ()->set (new PyString (name), value);
}

PyDict* GPSTransportClosed::reasonArgs () const {
    return this->keyword ("reasonArgs")->As <PyDict> ();
}

PyTuple* GPSTransportClosed::_CreateArgs (const char* reason)
{
    return new PyTuple {
        new PyString (reason)
    };
}

PyDict* GPSTransportClosed::_CreateKeywords (const char* reason)
{
    return new PyDict {
        { new PyString ("reasonArgs"), new PyDict},
        { new PyString ("clock"), new PyInt (GetFileTimeNow ()) },
        // { new PyString ("loggedOnUserCount"), },
        { new PyString ("region"), new PyString (EVEProjectRegion) },
        { new PyString ("reason"), new PyString (reason) },
        { new PyString ("version"), new PyFloat (EVEVersionNumber) },
        { new PyString ("build"), new PyInt (EVEBuildVersion) },
        // { new PyString ("reasonCode"), },
        { new PyString ("codename"), new PyString (EVEProjectCodename) },
        { new PyString ("machoVersion"), new PyInt (MachoNetVersion) },
    };
}

