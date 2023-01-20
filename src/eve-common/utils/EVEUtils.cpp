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
*/

#include "eve-common.h"

#include "utils/EVEUtils.h"

bool IsPrintable( const PyString* str )
{
    return IsPrintable( str->content() );
}

bool DBTYPE_IsCompatible( DBTYPE type, const PyDataType* rep )
{
// Helper macro, checks type and range
#define CheckTypeRangeUnsigned( type, lower_bound, upper_bound ) \
    ( rep->is<Py##type> () && (int64)rep->as<Py##type>()->value() >= lower_bound && (int64)rep->as<Py##type>()->value() <= upper_bound )
#define CheckTypeRange( type, lower_bound, upper_bound ) \
    ( rep->is<Py##type>() && rep->as<Py##type>()->value() >= lower_bound && rep->as<Py##type>()->value() <= upper_bound )

    if(rep->is<PyNone>() )
        // represents NULL
        return true;

    // TODO: REMOVE THIS RETURN, THE ACTUAL DATA TYPES SHOULD BE CHECKED
    return true;

    switch( type )
    {
        case DBTYPE_UI8:
        case DBTYPE_CY:
        case DBTYPE_FILETIME:
            return (CheckTypeRangeUnsigned(Int, 0LL, 0xFFFFFFFFFFFFFFFFLL) ||
                  CheckTypeRangeUnsigned(Float, 0LL, 0xFFFFFFFFFFFFFFFFLL));
        case DBTYPE_UI4:
            return (CheckTypeRangeUnsigned(Int, 0L, 0xFFFFFFFFL) ||
                  CheckTypeRangeUnsigned(Float, 0L, 0xFFFFFFFFL));
        case DBTYPE_UI2:
            return (CheckTypeRangeUnsigned(Int, 0, 0xFFFF) ||
                  CheckTypeRangeUnsigned(Float, 0, 0xFFFF));
        case DBTYPE_UI1:
            return (CheckTypeRangeUnsigned(Int, 0, 0xFF) ||
                  CheckTypeRangeUnsigned(Float, 0, 0xFF));

        case DBTYPE_I8:
            return (CheckTypeRange(Int, -0x7FFFFFFFFFFFFFFFLL,
                                 0x7FFFFFFFFFFFFFFFLL) ||
                  CheckTypeRange(Float, -0x7FFFFFFFFFFFFFFFLL,
                                 0x7FFFFFFFFFFFFFFFLL));
        case DBTYPE_I4:
            return (CheckTypeRange(Int, -0x7FFFFFFFL, 0x7FFFFFFFL) ||
                  CheckTypeRange(Float, -0x7FFFFFFFL, 0x7FFFFFFFL));
        case DBTYPE_I2:
            return (CheckTypeRange(Int, -0x7FFF, 0x7FFF) ||
                  CheckTypeRange(Float, -0x7FFF, 0x7FFF));
        case DBTYPE_I1:
            return (CheckTypeRange(Int, -0x7F, 0x7F) ||
                  CheckTypeRange(Float, -0x7F, 0x7F));

        case DBTYPE_R8:
            return (CheckTypeRange(Int, -DBL_MAX, DBL_MAX) ||
                  CheckTypeRange(Float, -DBL_MAX, DBL_MAX));
        case DBTYPE_R4:
            return (CheckTypeRange(Int, -FLT_MAX, FLT_MAX) ||
                  CheckTypeRange(Float, -FLT_MAX, FLT_MAX));

        case DBTYPE_BOOL:
            return rep->is<PyBool>();

        case DBTYPE_BYTES:
            return rep->is<PyBuffer>();

            // this looks like a horrible hack, and it is - but one that is used on live!
            // this works because STR type stuff is just tacked on to the marshal object
        case DBTYPE_STR:
            return true;
            //return rep->is<PyString>();

        case DBTYPE_WSTR:
            return rep->is<PyString>() && rep->as<PyString>()->utf8();

        case DBTYPE_EMPTY:
        case DBTYPE_ERROR:
            return false;
    }

    return false;

#undef CheckTypeRange
#undef CheckTypeRangeUnsigned
}
