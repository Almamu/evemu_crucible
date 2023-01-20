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
    Author:        Almamu
*/

#include "UserError.h"

#include "python/PyObjectEx.h"
#include "python/PyString.h"
#include "python/PyToken.h"
#include "python/PyFloat.h"
#include "python/PyInt.h"

const char* UserError::EXCEPTION_NAME = "ccp_exceptions.UserError";

UserError::UserError (const char* exceptionType) :
    PyException (new PyObjectEx_Type1 (new PyToken (EXCEPTION_NAME), _CreateArgs (exceptionType), _CreateKeywords (exceptionType))) {
}

UserError& UserError::AddFormatValue (const char* name, PyDataType* value) {
    this->AddKeyword (name, value);

    return *this;
}

UserError& UserError::AddParameterKeyword (const char* name, UserError_ParameterIDs type, PyDataType* value, PyDataType* value2)
{
    PyTuple* param;

    if (value2 == nullptr)
        param = new PyTuple {new PyInt (type), value};
    else
        param = new PyTuple {new PyInt (type), value, value2};

    return this->AddFormatValue (name, param);
}

UserError& UserError::AddDateTime (const char* name, time_t date)
{
    return this->AddParameterKeyword (name, UserError_Parameter_DateTime, new PyInt (date));
}

UserError& UserError::AddDate (const char* name, time_t date)
{
    return this->AddParameterKeyword (name, UserError_Parameter_Date, new PyInt (date));
}

UserError& UserError::AddTime (const char* name, time_t time)
{
    return this->AddParameterKeyword (name, UserError_Parameter_Time, new PyInt (time));
}

UserError& UserError::AddTimeShort (const char* name, time_t time)
{
    return this->AddParameterKeyword (name, UserError_Parameter_TimeShort, new PyInt (time));
}
UserError& UserError::AddA (const char* name, const char* value)
{
    return this->AddParameterKeyword (name, UserError_Parameter_A, new PyString (value));
}

UserError& UserError::AddThe (const char* name, const char* value)
{
    return this->AddParameterKeyword (name, UserError_Parameter_The, new PyString (value));
}

UserError& UserError::AddUELocalization (const char* name, const char* strKey, PyDict* args)
{
    return this->AddParameterKeyword (name, UserError_Parameter_Localization, new PyString (strKey), args);
}

UserError& UserError::AddList (const char* name, PyList* listEntries, const char* separator)
{
    if (separator == nullptr)
        return this->AddParameterKeyword (name, UserError_Parameter_List, listEntries);
    else
        return this->AddParameterKeyword (name, UserError_Parameter_List, listEntries, new PyString (separator));
}

UserError& UserError::AddOwnerName (const char* name, uint32 ownerID)
{
    return this->AddParameterKeyword (name, UserError_Parameter_OwnerName, new PyInt (ownerID));
}

UserError& UserError::AddOwnerNick (const char* name, uint32 ownerID)
{
    return this->AddParameterKeyword (name, UserError_Parameter_OwnerNick, new PyInt (ownerID));
}

UserError& UserError::AddLocationName (const char* name, uint32 locationID)
{
    return this->AddParameterKeyword (name, UserError_Parameter_LocationName, new PyInt (locationID));
}

UserError& UserError::AddTypeName (const char* name, uint32 typeID)
{
    return this->AddParameterKeyword (name, UserError_Parameter_TypeName, new PyInt (typeID));
}

UserError& UserError::AddTypeDescription (const char* name, uint32 typeID)
{
    return this->AddParameterKeyword (name, UserError_Parameter_TypeDescription, new PyInt (typeID));
}

UserError& UserError::AddTypeList (const char* name, PyList* typeIDs)
{
    return this->AddParameterKeyword (name, UserError_Parameter_TypeIDList, typeIDs);
}

UserError& UserError::AddBlueprintTypeName (const char* name, uint32 bpTypeID)
{
    return this->AddParameterKeyword (name, UserError_Parameter_BluePrintTypeName, new PyInt (bpTypeID));
}

UserError& UserError::AddGroupName (const char* name, uint32 groupID)
{
    return this->AddParameterKeyword (name, UserError_Parameter_GroupName, new PyInt (groupID));
}

UserError& UserError::AddGroupDescription (const char* name, uint32 groupID)
{
    return this->AddParameterKeyword (name, UserError_Parameter_GroupDescription, new PyInt (groupID));
}

UserError& UserError::AddCategoryName (const char* name, uint32 categoryID)
{
    return this->AddParameterKeyword (name, UserError_Parameter_CategoryName, new PyInt (categoryID));
}

UserError& UserError::AddCategoryDescription (const char* name, uint32 categoryID)
{
    return this->AddParameterKeyword (name, UserError_Parameter_CategoryDescription, new PyInt (categoryID));
}

UserError& UserError::AddAmount (const char* name, int quantity)
{
    return this->AddParameterKeyword (name, UserError_Parameter_Amount, new PyInt (quantity));
}

UserError& UserError::AddAmount (const char* name, uint quantity)
{
    return this->AddParameterKeyword (name, UserError_Parameter_Amount, new PyInt (quantity));
}

UserError& UserError::AddAmount (const char* name, double quantity)
{
    return this->AddParameterKeyword (name, UserError_Parameter_Amount, new PyFloat (quantity));
}

UserError& UserError::AddISK (const char* name, double isk)
{
    return this->AddParameterKeyword (name, UserError_Parameter_ISK, new PyFloat (isk));
}

UserError& UserError::AddAUR (const char* name, double aur)
{
    return this->AddParameterKeyword (name, UserError_Parameter_AUR, new PyFloat (aur));
}

UserError& UserError::AddDistance (const char* name, double distance)
{
    return this->AddParameterKeyword (name, UserError_Parameter_Distance, new PyFloat (distance));
}

UserError& UserError::AddTypeIDAndQuantity (const char* name, uint32 typeID, int quantity)
{
    return this->AddParameterKeyword (name, UserError_Parameter_TypeIDAndQuantity, new PyInt (typeID), new PyInt (quantity));
}

void UserError::AddKeyword (const char* name, PyDataType* value)
{
    // We need 2 refs ... the first one is given to us,
    // but we must create the second one ...
    PyIncRef (value);

    this->_GetTupleKeywords ()->set (new PyString (name), value);
    this->_GetDictKeywords ()->set (new PyString (name), value);
}

PyDict* UserError::_GetTupleKeywords() const
{
    return this->m_args->at <PyDict> (1);
}

PyDict* UserError::_GetDictKeywords() const
{
    return this->m_keywords->get(new PyString("dict"))->as<PyDict>();
}

PyTuple* UserError::_CreateArgs (const char* msg)
{
    return this->m_args = new PyTuple {
        new PyString (msg),
        new PyDict
    };
}

PyDict* UserError::_CreateKeywords (const char* msg)
{
    return this->m_keywords = new PyDict {
        {new PyString ("msg"), new PyString (msg)},
        {new PyString ("dict"), new PyDict}
    };
}
