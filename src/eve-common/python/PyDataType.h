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
    Updates:    Allan
    Rewrite:    Almamu
*/

#ifndef __PYDATATYPE_H__
#define __PYDATATYPE_H__

#include <exception>

#include "eve-core.h"
#include "PyVisitor.h"
#include "memory/RefPtr.h"

/**
 * debug macros to ease the increase and decrease of references of a object
 * using this also increases the possibility of debugging it.
 */
#define PyIncRef(op) (op)->IncRef()
#define PyDecRef(op) (op)->DecRef()

/* Macros to use in case the object pointer may be NULL */
#define PySafeIncRef(op) if (op != nullptr) PyIncRef( op )
#define PySafeDecRef(op) if (op != nullptr) PyDecRef( op )

/**
 * Represents a data type coming from the client
 */
class PyDataType : public RefObject
{
public:
    enum PyType
    {
        PyTypeMin = 0,
        PyTypeNone = 0,
        PyTypeInt = 1,
        PyTypeFloat = 2,
        PyTypeString = 3,
        PyTypeToken = 4,
        PyTypeBuffer = 5,
        PyTypeBool = 6,
        PyTypeTuple = 7,
        PyTypeList = 8,
        PyTypeDict = 9,
        PyTypeObject = 10,
        PyTypePackedRow = 11,
        PyTypeSubStruct = 12,
        PyTypeSubStream = 13,
        PyTypeMax = 13,
    };

    using RefObject::IncRef;
    using RefObject::DecRef;

    /**
     * Checks if this object is of the given tyoe
     *
     * @tparam T Type to check against
     * @return
     */
    template <class T> inline bool Is () const { return this->mType == T::Type; }

    /**
     * Checks if this object is of the given type and casts it
     *
     * @tparam T Type to check against
     * @return
     */
    template <class T> inline T* As () const {
        if constexpr (std::is_same_v<T, PyDataType>)
            return (T*) this;

        if (Is <T> () == false)
            throw std::bad_cast ();

        return (T*) this;
    }

    const char* TypeString () const;

    /**
     * @brief Helper method, dumps object to file
     *
     * @param into[in]      File into which dump should be written
     * @param prefix[in]    Prefix which is put in front of each line
     */
    void dump (FILE* into, const char* prefix) const;

    /**
     * @brief Helper method, dumps object to console
     *
     * @param into[in]      Log type to use for dump
     * @param prefix[in]    Prefix which is put in front of each line
     */
    void dump (LogType type, const char* prefix) const;

    /**
     * virtual function to generate a hash value of a object to facilitate the various maps and checks
     *
     * @return a int32 containing the value representing this object
     */
    virtual int32 hash () const;

    /**
     * Visits object to do things with it
     *
     * @param v The visitor to be used
     *
     * @return
     */
    virtual bool visit (PyVisitor& v) const;

    /**
     * Compares the two data types given
     *
     * @param left
     *
     * @return Whether the two objects are equal or not
     */
    virtual bool equals (const PyDataType* right) const;

    /**
     * Performs a smart casting of the object and returns a string value
     * @return
     */
    std::string string () const;

    /**
     * Performs a smart casting of the object and returns an int64_t value
     * @return
     */
    int64_t i64 () const;

    /**
     * Performs a smart casting of the object and returns an uint32_t value
     * @return
     */
    uint32_t u32 () const;

    /**
     * Performs a smart casting of the object and returns a double value
     * @return
     */
    double decimal () const;

protected:
    /**
     * Default constructor
     * @param type
     */
    PyDataType (PyType type);

    /**
     * Default destructor
     */
    virtual ~PyDataType ();

private:
    /**
     * @return Indicates the type of this python data type
     */
    PyType GetType () const;

    /**
     * The type this python data type is
     */
    const PyType mType;
};

#endif // !__PYDATATYPE_H__
