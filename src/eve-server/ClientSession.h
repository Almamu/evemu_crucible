/*
    ------------------------------------------------------------------------------------
    LICENSE:
    ------------------------------------------------------------------------------------
    This file is part of EVEmu: EVE Online Server Emulator
    Copyright 2006 - 2025 The EVEmu Team
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
    Author: Bloody.Rabbit
    Updates: Allan, Almamu
*/

#pragma once

#include "eve-server.h"

/**
 * @brief Value keeper for single EVE session.
 *
 * This object keeps track of string-value pairs
 * and encodes their changes as session changes.
 */
class ClientSession {
public:
    struct SessionValue {
        PyDataType* previous;
        PyDataType* current;
        bool dirty;
    };

    using storage_type = std::map<std::string, SessionValue>;
    using iterator = storage_type::iterator;
    using const_iterator = storage_type::const_iterator;

    ClientSession();
    ~ClientSession();

    bool isDirty() const;

    /**
     * Clears the value of the given session key
     *
     * @param name
     */
    void clear (const char* name);

    const_iterator begin() const;
    const_iterator end() const;
    iterator begin();
    iterator end();

    void set (const char* name, int64 value);
    void set (const char* name, int32 value);
    void set (const char* name, uint32 value);
    void set (const char* name, const char* value);
    void set (const char* name, double value);

    int64 i64 (const char* name) const;
    int64 i64previous (const char* name) const;
    std::string string (const char* name) const;
    std::string stringPrevious (const char* name) const;
    double decimal (const char* name) const;
    double decimalPrevious (const char* name) const;

    /**
     * Buils a PyDictionary with the list of changes the session has gone through
     * Every item is a tuple that includes the old and new value so the client can compare against what it has
     *
     * @param into
     */
    void encodeChanges (PyDict* into);

    /**
     * Builds a PyDictionary with the current state of the session. Useful for macho.SessionInitialStateNotification
     * The side-effects include overwriting of the old value on the session data with the new one
     *
     * @param into
     */
    void encodeInitialState (PyDict* into);

    /**
     * Checks if a value is present in the session
     *
     * @param name
     * @return
     */
    bool has (const char* name) const;

    /**
     * @return The current session id
     */
    int64 sessionID() const;

protected:
    /**
     * Searches for the given session value by name and returns the tuple used to store the value
     *
     * @param name
     *
     * @return
     */
    const_iterator _getValue (const char* name) const;
    /**
     * Searches for the given session value by name and returns the tuple used to store the value
     *
     * @param name
     *
     * @return
     */
    iterator _getValue (const char* name);

    /**
     * Searches for the given session value by name and sets the current value
     *
     * @param name
     * @param value
     */
    void _set (const char* name, PyDataType* value);

    /**
     * Initializes the given session value with the specified data
     *
     * @param name
     * @param value
     */
    iterator _initialize (const char* name, PyDataType* value);

private:
    storage_type mSession;

    TrackedPythonArena mArena;

    bool mDirty;

    int64 mSessionID;
};
