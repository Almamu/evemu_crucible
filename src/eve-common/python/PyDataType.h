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

#pragma once

#include <exception>
#include <source_location>

#include "eve-core.h"
#include "PyVisitor.h"
#include "memory/RefPtr.h"
#include "memory/ArenaAware.h"

// TODO: REMOVE THESE ONCE WE DECIDE EXACTLY WHAT TO DO AND HOW T.T
/**
 * debug macros to ease the increase and decrease of references of a object
 * using this also increases the possibility of debugging it.
 */
#define PyIncRef(op) (op)->IncRef()
#define PyDecRef(op) (op)->DecRef()

/* Macros to use in case the object pointer may be NULL */
#define PySafeIncRef(op) if (op != nullptr) PyIncRef( op )
#define PySafeDecRef(op) if (op != nullptr) PyDecRef( op )

#define PyOptionalIncRef(op) if (op.has_value()) PyIncRef (op.value())
#define PyOptionalDecRef(op) if (op.has_value()) PyDecRef (op.value())

#define PyOptionalSafeIncRef(op) if (op.has_value()) PySafeIncRef (op.value())
#define PyOptionalSafeDecRef(op) if (op.has_value()) PySafeDecRef (op.value())

class PythonArena;
class HeapPythonArena;
class PyTraceLog;

/**
 * Represents a data type coming from the client
 */
class PyDataType : public ArenaAware<HeapPythonArena, PythonArena>, public RefObject {
    friend PyTraceLog;
public:
    enum PyType {
        PyTypeMin = 0,
        PyTypeNone = 1,
        PyTypeInt = 2,
        PyTypeFloat = 3,
        PyTypeString = 4,
        PyTypeToken = 5,
        PyTypeBuffer = 6,
        PyTypeBool = 7,
        PyTypeTuple = 8,
        PyTypeList = 9,
        PyTypeDict = 10,
        PyTypeObject = 11,
        PyTypeObjectEx = 12,
        PyTypePackedRow = 13,
        PyTypeSubStruct = 14,
        PyTypeSubStream = 15,
        PyTypeMax = 16,
    };

    using RefObject::IncRef;
    using RefObject::DecRef;

    /**
     * Checks if this object is of the given tyoe
     *
     * @tparam T Type to check against
     * @return
     */
    template <class T> inline bool is() const {
        return this->mType == T::Type;
    }

    /**
     * Checks if this object is of the given type and casts it
     *
     * @tparam T Type to check against
     * @return
     */
    template <class T> inline T* as() const {
        if (is<T>() == false)
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
    [[nodiscard]] virtual int32 hash () const;

    /**
     * Visits object to do things with it
     *
     * @param v The visitor to be used
     *
     * @return
     */
    [[nodiscard]] virtual bool visit (PyVisitor& v) const;

    /**
     * Compares the two data types given
     *
     * @param left
     *
     * @return Whether the two objects are equal or not
     */
    [[nodiscard]] virtual bool equals (const PyDataType* right) const;

    /**
     * Clones this data type into the given arena
     *
     * @param arena
     *
     * @return
     */
    [[nodiscard]] virtual PyDataType* clone() const;

    /**
     * Clones this data type into the given arena
     *
     * @param arena
     *
     * @return
     */
    [[nodiscard]] virtual PyDataType* clone(PythonArena* arena) const;

    /**
     * Performs a smart casting of the object and returns a string value
     * @return
     */
    [[nodiscard]] std::string string () const;

    /**
     * Performs a smart casting of the object and returns an int64_t value
     * @return
     */
    [[nodiscard]] int64_t i64 () const;

    /**
     * Performs a smart casting of the object and returns an uint32_t value
     * @return
     */
    [[nodiscard]] uint32_t u32 () const;

    /**
     * Performs a smart casting of the object and returns an int32_t value
     * @return
     */
    [[nodiscard]] int32_t i32 () const;

    /**
     * Performs a smart casting of the object and returns a double value
     * @return
     */
    [[nodiscard]] double decimal () const;

protected:
    /**
     * Default constructor
     * @param type
     */
    explicit PyDataType (PyType type);

    /**
     * Default destructor
     */
    virtual ~PyDataType ();

    /**
     * Checks that the arena owning this object is the same as the specified object
     * and throws an exception if that's not the case
     *
     * @param obj
     * @throws std::runtime_error
     */
    void _checkArenaOwnership (PyDataType* obj);

private:
    /**
     * @return Indicates the type of this python data type
     */
    [[nodiscard]] PyType GetType () const;

    /**
     * The type this python data type is
     */
    const PyType mType;
};
