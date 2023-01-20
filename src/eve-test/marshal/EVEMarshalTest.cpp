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
    Author:     Bloody.Rabbit
*/

#include "eve-test.h"

int marshal_EVEMarshalTest( int argc, char* argv[] )
{
    StaticPythonArena in(1 * 1024 * 1024);
    StaticPythonArena out(1 * 1024 * 1024);

    DBRowDescriptor *header = new(&in) DBRowDescriptor;
    // Fill header:
    header->add ("historyDate", DBTYPE_FILETIME);
    header->add ("lowPrice", DBTYPE_CY);
    header->add ("highPrice", DBTYPE_CY);
    header->add ("avgPrice", DBTYPE_CY);
    header->add ("volume", DBTYPE_I8);
    header->add ("orders", DBTYPE_I4);

    CRowset* rs = new(&in) CRowset (header);

    rs->insert({
        in.Int(Win32TimeNow()),
        in.Int(18000),
        in.Int(19000),
        in.Int(18400),
        in.Int(5463586),
        in.Int(254)
    });

    ::puts( "Marshaling..." );

    Buffer marshaled;
    bool res = MarshalDeflate( rs, marshaled );

    if( !res )
    {
        ::puts( "Failed to marshal Python object." );
        return EXIT_FAILURE;
    }

    ::puts( "Unmarshaling..." );

    PyDataType* rep = InflateUnmarshal (marshaled, &out);
    if( NULL == rep )
    {
        ::puts( "Failed to unmarshal Python object." );
        return EXIT_FAILURE;
    }

    ::puts( "Final:" );
    rep->dump( stdout, "    " );
    PyDecRef( rep );

    if (in.freeBytes() != out.freeBytes()) {
        return EXIT_FAILURE;
    }

    if (!rep->equals (rs)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
