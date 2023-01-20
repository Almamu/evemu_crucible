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
    Author:        Zhur
*/

#ifndef EVE_UNMARSHAL_H
#define EVE_UNMARSHAL_H

#include "python/Types.h"

/**
 * @brief Turns marshal stream into Python object.
 *
 * @param[in] data Marshal stream.
 *
 * @return Ownership of Python object.
 */
extern PyDataType* Unmarshal (const Buffer& data, PythonArena* arena);
/**
 * @brief Turns possibly inflated marshal stream into Python object.
 *
 * @param[in] data Possibly inflated marshal stream.
 *
 * @return Ownership of Python object.
*/
extern PyDataType* InflateUnmarshal (const Buffer& data, PythonArena* arena);

/**
 * @brief Class which turns marshal bytecode into Python object.
 *
 * @author Zhur, Bloody.Rabbit
 */
class UnmarshalStream
{
public:
    UnmarshalStream (PythonArena* arena) :
        mStoredObjects(), mArena (arena)
    {
    }

    ~UnmarshalStream ();

    /**
     * @brief Loads Python object from given bytecode.
     *
     * @param[in] data Buffer containing marshal bytecode.
     *
     * @return Loaded Python object.
     */
    PyDataType* Load (const Buffer& data);

protected:
    /** Peeks element from stream. */
    template <typename T>
    const T& Peek () const { return *Peek <T> (1); }
    /** Peeks elements from stream. */
    template <typename T>
    Buffer::const_iterator <T> Peek (size_t count) const { return mInItr.As <T> (); }

    /** Reads element from stream. */
    template <typename T>
    const T& Read () { return *Read <T> (1); }
    /** Reads elements from stream. */
    template <typename T>
    Buffer::const_iterator <T> Read (size_t count)
    {
        Buffer::const_iterator <T> res = Peek <T> (count);
        mInItr = (res + count).template As <uint8>();
        return res;
    }

    /** Reads extended size from stream. */
    uint32 ReadSizeEx ()
    {
        uint32 size = Read <uint8> ();
        if (0xFF == size)
            size = Read <uint32> ();

        return size;
    }

    /** Initializes loading and loads rep from stream. */
    PyDataType* LoadStream (size_t streamLength);

    /** Loads rep from stream. */
    PyDataType*LoadDataType();

    /**
     * @brief Initializes object store.
     *
     * @param[in] streamLength Length of stream.
     * @param[in] saveCount    Number of saved objects within the stream.
     */
    void CreateObjectStore (size_t streamLength, uint32 saveCount);
    /**
     * @brief Destroys object store.
     */
    void DestroyObjectStore ();

    /**
     * @brief Obtains storage index for StoreObject.
     *
     * @return Storage index.
     */
    uint32 GetStorageIndex () { return *mStoreIndexItr++; }
    /**
     * @brief Obtains previously stored object.
     *
     * @param[in] index Index of stored object.
     *
     * @return The stored object.
     */
    PyDataType* GetStoredObject (uint32 index);
    /**
     * @brief Stores object.
     *
     * @param[in] index  Index of object.
     * @param[in] object The object to be stored.
     */
    void StoreObject (uint32 index, PyDataType* object);

private:
    /** Loads none from stream. */
    PyDataType* LoadNone () { return new(mArena) PyNone(); }

    /** Loads true boolean from stream. */
    PyDataType* LoadBoolTrue () { return new(mArena) PyBool(true); }
    /** Loads false boolean from stream. */
    PyDataType* LoadBoolFalse () { return new(mArena) PyBool(false); }

    /** Loads long long integer from stream. */
    PyDataType* LoadIntegerLongLong () { return new(mArena) PyInt (Read <int64> ()); }
    /** Loads long integer from stream. */
    PyDataType* LoadIntegerLong () { return new(mArena) PyInt (Read <int32> ()); }
    /** Loads signed short from stream. */
    PyDataType* LoadIntegerSignedShort () { return new(mArena) PyInt (Read <int16> ()); }
    /** Loads byte integer from stream. */
    PyDataType* LoadIntegerByte () { return new(mArena) PyInt (Read <int8> ()); }
    /** Loads variable length integer from stream. */
    PyDataType* LoadIntegerVar ();
    /** Loads minus one integer from stream. */
    PyDataType* LoadIntegerMinusOne () { return new(mArena) PyInt (-1); }
    /** Loads zero integer from stream. */
    PyDataType* LoadIntegerZero () { return new(mArena) PyInt (0); }
    /** Loads one integer from stream. */
    PyDataType* LoadIntegerOne () { return new(mArena) PyInt (1); }

    /** Loads real from stream. */
    PyDataType* LoadReal () { return new(mArena) PyFloat (Read <double> ()); }
    /** Loads zero real from stream. */
    PyDataType* LoadRealZero () { return new(mArena) PyFloat (0.0); }

    /** Loads empty string from stream. */
    PyDataType* LoadStringEmpty () { return new(mArena) PyString (""); }
    /** Loads single character string from stream. */
    PyDataType* LoadStringChar ();
    /** Loads short (up to 255 chars) string from stream. */
    PyDataType* LoadStringShort ();
    /** Loads long (no limit) string from stream. */
    PyDataType* LoadStringLong ();
    /** Loads table string from stream. */
    PyDataType* LoadStringTable ();

    /** Loads empty wide string from stream. */
    PyDataType* LoadWStringEmpty () { return new(mArena) PyString (std::string (""), true); }
    /** Loads single UCS-2 character string from stream. */
    PyDataType* LoadWStringUCS2Char ();
    /** Loads UCS-2 string from stream. */
    PyDataType* LoadWStringUCS2 ();
    /** Loads UTF-8 string from stream. */
    PyDataType* LoadWStringUTF8 ();

    /** Loads token from stream. */
    PyDataType* LoadToken ();

    /** Loads buffer from stream. */
    PyDataType* LoadBuffer ();

    /** Loads empty tuple from stream. */
    PyDataType* LoadTupleEmpty () { return new(mArena) PyTuple (); }
    /** Loads tuple from stream. */
    PyDataType* LoadTuple ();
    /** Loads one-element tuple from stream. */
    PyDataType* LoadTupleOne ();
    /** Loads two-element tuple from stream. */
    PyDataType* LoadTupleTwo ();

    /** Loads empty list from stream. */
    PyDataType* LoadListEmpty () { return new(mArena) PyList (0); }
    /** Loads list from stream. */
    PyDataType* LoadList ();
    /** Loads one-element list from stream. */
    PyDataType* LoadListOne ();

    /** Loads dict from stream. */
    PyDataType* LoadDict ();

    /** Loads object from stream. */
    PyDataType* LoadObject ();
    /** Loads extended object of type 1 from stream. */
    PyDataType* LoadObjectEx1 ();
    /** Loads extended object of type 2 from stream. */
    PyDataType* LoadObjectEx2 ();

    /** Loads sub stream from stream. */
    PyDataType* LoadSubStream ();
    /** Loads sub struct from stream. */
    PyDataType* LoadSubStruct ();
    /** Loads checksumed stream from stream. */
    PyDataType* LoadChecksumedStream ();

    /** Loads packed row from stream. */
    PyDataType* LoadPackedRow ();

    /** Prints error and returns NULL. */
    PyDataType* LoadError ();
    /** Loads saved stream element from stream. */
    PyDataType* LoadSavedStreamElement ();

    /** Helper; loads extended object from stream. */
    PyObjectEx* LoadObjectEx (bool is_type_2);
    /** Helper; loads zero-compressed buffer from stream. */
    bool LoadRLE (Buffer& out);

    /** Buffer iterator we are processing. */
    Buffer::const_iterator <uint8> mInItr;

    /** Next store index for referencing in the buffer. */
    Buffer::const_iterator <uint32> mStoreIndexItr;
    /** Referenced objects within the buffer. */
    std::vector<PyDataType*> mStoredObjects;
    /** Python arena used to allocate objets into */
    PythonArena* mArena;

    /** Load function map. */
    static PyDataType* (UnmarshalStream::* const s_mLoadMap [])();
};

#endif

