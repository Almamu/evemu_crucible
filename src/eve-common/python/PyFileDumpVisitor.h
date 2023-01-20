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
    Author:     Zhur, mmcs
*/

#ifndef __PYFILEDUMPVISITOR_H__
#define __PYFILEDUMPVISITOR_H__

#include "eve-common.h"
#include "PyDumpVisitor.h"
#include "PyVisitor.h"

class PyFileDumpVisitor : public PyDumpVisitor
{
public:
    PyFileDumpVisitor (FILE* _file, const char* pfx = "", bool full_nested = false, bool full_hex = false);

    bool fullHex () const { return mFullHex; }

    FILE* file () const { return mFile; }

protected:
    void _print (const char* fmt, ...);
    void _dump (const char* pfx, const uint8* data, size_t len);

private:
    const bool mFullHex;

    FILE* const mFile;
};

#endif /* !__PYFILEDUMPVISITOR_H__ */
