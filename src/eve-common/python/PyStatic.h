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
    Author:     Allan
*/

#ifndef __PYSTATIC_H__
#define __PYSTATIC_H__

/**
 * @name PyStatic.h
 *   static memory object caching/tracking system for oft-used Python objects
 *
 * @Author:         Allan
 * @date:          13 December 17
 * @update:     15 February 21 (added mt objects)
 *
 */

#include "utils/Singleton.h"

#include "PyDataType.h"
#include "PyInt.h"
#include "PyNone.h"
#include "PyBool.h"
#include "PyDict.h"
#include "PyList.h"
#include "PyTuple.h"

class pyStatic : public Singleton <pyStatic>
{
public:
    pyStatic()
    {
        m_none = new PyNone();
        m_zero = new PyInt(0);
        m_one = new PyInt(1);
        m_negone = new PyInt(-1);
        m_true = new PyBool(true);
        m_false = new PyBool(false);
        m_dict = new PyDict();
        m_list = new PyList();
        m_tuple = new PyTuple();
    }

   ~pyStatic()
   {
       PyDecRef(m_none);
       PyDecRef(m_zero);
       PyDecRef(m_one);
       PyDecRef(m_negone);
       PyDecRef(m_true);
       PyDecRef(m_false);
       PyDecRef(m_dict);
       PyDecRef(m_list);
       PyDecRef(m_tuple);
    }

    PyDataType* NewNone()            { PyIncRef(m_none); return m_none; }
    PyDataType* NewZero()            { PyIncRef(m_zero); return m_zero; }
    PyDataType* NewOne()             { PyIncRef(m_one); return m_one; }
    PyDataType* NewNegOne()          { PyIncRef(m_negone); return m_negone; }
    PyDataType* NewTrue()            { PyIncRef(m_true); return m_true; }
    PyDataType* NewFalse()           { PyIncRef(m_false); return m_false; }

    PyDict* mtDict()            { PyIncRef(m_dict); return m_dict; }
    PyList* mtList()            { PyIncRef(m_list); return m_list; }
    PyTuple* mtTuple()          { PyIncRef(m_tuple); return m_tuple; }

private:
    PyDataType* m_none;
    PyDataType* m_zero;
    PyDataType* m_one;
    PyDataType* m_negone;
    PyDataType* m_true;
    PyDataType* m_false;

    PyDict* m_dict;
    PyList* m_list;
    PyTuple* m_tuple;
};

#define PyStatic \
    ( pyStatic::get() )


#endif /* !__PYSTATIC_H__ */
