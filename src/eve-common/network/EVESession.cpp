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

#include "marshal/EVEMarshal.h"
#include "marshal/EVEUnmarshal.h"
#include "network/EVESession.h"
#include "network/python/EVESecureClientHandshake.h"
#include "network/python/EVELowLevelVersionExchange.h"
#include "network/python/EVEPacket.h"
#include "python/Types.h"
#include "python/visitors/PyDumpVisitor.h"
#include "EVEVersion.h"

EVEClientSession::EVEClientSession(EVETCPConnection** n, TrackedPythonArena* arena)
: mNet(*n), mArena(arena), mPacketHandler(nullptr) {
    *n = nullptr;
}

EVEClientSession::~EVEClientSession() {
    // Destroy connection we used
    delete mNet;
}

void EVEClientSession::Reset() {
    mPacketHandler = nullptr;

    if (GetState() != TCPConnection::STATE_CONNECTED)
        // Connection has been lost, there's no point in reset
        return;

    EVELowLevelVersionExchange version;
    _GetVersion(version);

    mNet->QueueRep(version);

    mPacketHandler = &EVEClientSession::_HandleVersion;
}

void EVEClientSession::QueuePacket (EVEPacket* packet, PythonArena* arena) {
    if (packet == nullptr)
        return;

    mNet->QueueRep(packet->encode(arena));
}

EVEPacket* EVEClientSession::PopPacket(PythonArena* arena) {
    auto rep = mNet->PopRep(arena);

    if (rep == nullptr)
        return nullptr;

    if (is_log_enabled(NET__PRES_REP)) {
        _log(NET__PRES_REP, "%s: Raw Rep Dump:", GetAddress().c_str());
        rep->dump(NET__PRES_REP, "    ");
    }

    assert(mPacketHandler);
    return (this->*mPacketHandler)(rep);
}

EVEPacket* EVEClientSession::_HandleVersion(PyDataType* rep) {
    //we are waiting for their version information...
    EVELowLevelVersionExchange ve(rep->as<PyTuple>());

    if (_VerifyVersion(ve)) {
        mPacketHandler = &EVEClientSession::_HandleCommand;
    }

    //PySafeDecRef(rep);
    // recurse
    return PopPacket(mArena);
}

EVEPacket* EVEClientSession::_HandleCommand(PyDataType* rep) {
    // TODO: THROW EXCEPTIONS INSTEAD
    auto tup = rep->as<PyTuple>();

    if (tup->size() > 3) {
        sLog.Error("_HandleCommand", "%s: Invalid packet during waiting for command (tuple expected).", GetAddress().c_str());

        if (is_log_enabled(NET__PRES_ERROR)) {
            _log(NET__PRES_ERROR, "%s: Received invalid command packet:", GetAddress().c_str());
            rep->dump(NET__PRES_ERROR, "  ");
        }

        return PopPacket(mArena);
    }

    if (tup->size() == 2) {
        if (tup->at(0)->is<PyNone>() && tup->at(1)->string() == "QC") {
            sLog.Debug("_HandleCommand", "%s: Got Queue Check command.", GetAddress().c_str());

            //they return position in queue
            mNet->QueueRep(new PyInt(_GetQueuePosition()));

            //now reset connection
            Reset();
        } else {
            sLog.Error("_HandleCommand", "%s: Failed to decode 2-arg command.", GetAddress().c_str());
        }
    } else if (tup->size() == 3) {
        if (tup->at(0)->is<PyNone>() && tup->at(1)->string() == "VK") {
            auto vipKey = tup->at(2)->string();

            sLog.Debug("_HandleCommand", "%s: Got VK command, vipKey=%s.", GetAddress().c_str(), vipKey.c_str());

            if (_VerifyVIPKey(vipKey))
                mPacketHandler = &EVEClientSession::_HandleCrypto;
        } else {
            sLog.Error("_HandleCommand", "%s: Failed to decode 3-arg command.", GetAddress().c_str());
        }
    }

    return PopPacket(mArena);
}

EVEPacket* EVEClientSession::_HandleCrypto(PyDataType* rep) {
    // TODO: THROW EXCEPTIONS INSTEAD
    auto tup = rep->as<PyTuple>();

    if (tup->size() != 2) {
        sLog.Error("_HandleCrypto", "%s: Received invalid crypto request!", GetAddress().c_str());
    } else {
        std::string keyVersion = tup->at(0)->string();
        PyDict* keyParams = tup->at(1)->as<PyDict>();

        if (_VerifyCrypto(keyVersion, keyParams)) {
          mPacketHandler = &EVEClientSession::_HandleAuthentication;
        }
    }

    return PopPacket(mArena);
}

EVEPacket* EVEClientSession::_HandleAuthentication(PyDataType* rep) {
    // TODO: THROW EXCEPTIONS INSTEAD
    //just to be sure
    EVESecureClientHandshake ccp(rep->as<PyTuple>());
    if (_VerifyLogin(ccp)) {
        mPacketHandler = &EVEClientSession::_HandleFuncResult;
    }

    //PySafeDecRef(rep);
    return PopPacket(mArena);
}

EVEPacket* EVEClientSession::_HandleFuncResult(PyDataType* rep) {
    // TODO: THROW EXCEPTIONS INSTEAD
    auto tup = rep->as<PyTuple>();

    if (tup->size() != 3) {
        sLog.Error("_HandleFuncResult", "%s: Received invalid crypto handshake result!", GetAddress().c_str());
    } else {
        auto challenge_responsehash = tup->at(0)->string();
        auto func_output = tup->at(1)->string();

        if (_VerifyFuncResult(challenge_responsehash, func_output)) {
          mPacketHandler = &EVEClientSession::_HandlePacket;
        }
    }

    return PopPacket(mArena);
}

EVEPacket* EVEClientSession::_HandlePacket(PyDataType* rep) {
    return EVEPacket::from (rep);
}
