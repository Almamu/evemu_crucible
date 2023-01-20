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
    Update:     Allan
*/

#include "ClientSession.h"
#include "EntityList.h"
#include "config/EVEServerConfig.h"

/**
 * Map of variables that should never be sent through the wire on the session data
 * These variables will be ignored. In reality we should have a whitelist
 *
 * TODO: COME UP WITH A REAL WHITELIST OF THINGS THAT HAVE TO BE SENT TO THE CLIENT
 */
static std::map<std::string, bool> NONPERSISTVARS = {
    {"clientID", true},
    {"sessionID", true},
    {"sid", true}
};


ClientSession::ClientSession() :
    mArena (),
    mDirty (false),
    mSessionID (0),
    mSession () {
    // default session values
    _initialize ("role", mArena.Int(Acct::Role::PLAYER | Acct::Role::NEWBIE));
    _initialize ("userid", mArena.Int(0));
    _initialize ("address", mArena.String("0.0.0.0"));

    // session id is unique for each connection, a good candidate is the clientID
    // but it can also be generated randomly or with the current time as this is doing
    // doesn't really matter as long as it's unique
    mSessionID = GetTimeUSeconds() *15;
    sEntityList.RegisterSID(mSessionID);
}

ClientSession::~ClientSession() {
    // do we clear session vars here, or let ~PyDict() do it?
    sEntityList.RemoveSID(mSessionID);
}

bool ClientSession::isDirty() const {
    return mDirty;
}

void ClientSession::clear (const char* name) {
    _set (name, mArena.None());
}

ClientSession::const_iterator ClientSession::begin() const {
    return mSession.begin();
}

ClientSession::iterator ClientSession::begin() {
    return mSession.begin();
}

ClientSession::const_iterator ClientSession::end() const {
    return mSession.end();
}

ClientSession::iterator ClientSession::end() {
    return mSession.end();
}

void ClientSession::set (const char* name, int64 value) {
    _set (name, mArena.Int (value));
}

void ClientSession::set (const char* name, int32 value) {
    _set (name, mArena.Int (value));
}

void ClientSession::set (const char* name, uint32 value) {
    _set (name, mArena.Int (value));
}

void ClientSession::set (const char* name, const char* value) {
    _set (name, mArena.String (value));
}

void ClientSession::set (const char* name, double value) {
    _set (name, mArena.Float (value));
}

int64 ClientSession::i64 (const char* name) const {
    auto it = _getValue (name);

    if (it == this->end()) {
        return 0;
    }

    return it->second.current->i64();
}

int64 ClientSession::i64previous (const char* name) const {
    auto it = _getValue (name);

    if (it == this->end()) {
        return 0;
    }

    return it->second.previous->i64();
}

std::string ClientSession::string (const char* name) const {
    auto it = _getValue (name);

    if (it == this->end()) {
        return "";
    }

    return it->second.current->string();
}

std::string ClientSession::stringPrevious (const char* name) const {
    auto it = _getValue (name);

    if (it == this->end()) {
        return "";
    }

    return it->second.previous->string();
}

double ClientSession::decimal (const char* name) const {
    auto it = _getValue (name);

    if (it == this->end()) {
        return 0;
    }

    return it->second.current->decimal();
}

double ClientSession::decimalPrevious (const char* name) const {
    auto it = _getValue (name);

    if (it == this->end()) {
        return 0;
    }

    return it->second.previous->decimal();
}

void ClientSession::encodeChanges(PyDict* into) {
    if (!mDirty)
        return;

    for (auto cur : *this) {
        if (cur.second.dirty == false)
            continue;

        cur.second.dirty = false;

        // ignore variables that should not be sent to the client
        if (NONPERSISTVARS.find (cur.first) != NONPERSISTVARS.end())
            continue;

        into->set (cur.first, into->arena()->Tuple ({cur.second.previous, cur.second.current}, false));
    }

    mDirty = false;
}

void ClientSession::encodeInitialState (PyDict* into) {
    for (auto cur : *this) {
        cur.second.dirty = false;

        if (NONPERSISTVARS.find (cur.first) != NONPERSISTVARS.end())
            continue;

        // TODO: DECIDE IF THIS SHOULD BE CHECKED AGAINST THE ARENA OR NOT
        into->set (cur.first, cur.second.current, false);
    }

    // mark the session as not dirty
    mDirty = false;
}

bool ClientSession::has (const char* name) const {
    return _getValue (name) != this->end();
}

int64 ClientSession::sessionID() const {
    return mSessionID;
}

ClientSession::const_iterator ClientSession::_getValue (const char* name) const {
    return mSession.find (name);
}

ClientSession::iterator ClientSession::_getValue (const char* name) {
    return mSession.find (name);
}

void ClientSession::_set (const char* name, PyDataType* value) {
    auto it = _getValue (name);

    if (it == this->end()) {
        it = _initialize (name, mArena.None());

        if (it == this->end()) {
            throw std::runtime_error ("Cannot initialize new key in the client session data");
        }
    }

    if (it->second.current->equals (value) == false) {
        // delete previous value as it's not needed anymore
        // TODO: REPLACE WITH A PROPER DELETE
        PySafeDecRef (it->second.previous);

        // now replace it with the right values
        it->second.previous = it->second.current;
        it->second.current = value->clone (&mArena);
        it->second.dirty = true;
        mDirty = true;
    }

    PyDecRef (value);
}

ClientSession::iterator ClientSession::_initialize (const char* name, PyDataType* value) {
    return mSession.insert ({name, {
        .previous = mArena.None(),
        .current = value,
        .dirty = false
    }}).first;
}