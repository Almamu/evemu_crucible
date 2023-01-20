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

#include "python/classes/PyExceptions.h"
#include "EVEVersion.h"

CustomError::CustomError(const char *fmt, ...)
: UserError (EXCEPTION_NAME)
{
    va_list va;
    va_start (va, fmt);

    char* str = nullptr;
    vasprintf (&str, fmt, va);
    assert (str);

    va_end (va);

    this->AddFormatValue ("error", new PyString (str));

    SafeFree (str);
}

const char* CustomError::EXCEPTION_NAME = "CustomError";
const char* UserError::EXCEPTION_NAME = "ccp_exceptions.UserError";