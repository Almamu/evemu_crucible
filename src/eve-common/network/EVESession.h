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

#ifndef __EVE_SESSION_H__INCL__
#define __EVE_SESSION_H__INCL__

#include "network/EVETCPConnection.h"

class EVEPacket;

class EVESecureClientHandshake;
class EVELowLevelVersionExchange;

/**
 * @brief Client session from server's side.
 *
 * Defines what server needs to do when dealing with client;
 * takes care of initial authentication, before actual
 * packets are sent.
 *
 * @author Bloody.Rabbit
 */
class EVEClientSession
{
public:
    /** Typedef for readability. */
    typedef TCPConnection::state_t state_t;

    /**
     * @param[in] n     Connection of this session.
     * @param[in] arena The python memory arena to use for reading data
     */
    EVEClientSession (EVETCPConnection** n, TrackedPythonArena* arena);
    /**
     * @brief Destroys contained connection.
     */
    virtual ~EVEClientSession();

    /** Wrapper of TCPConnection::GetState(). */
    state_t GetState() const { return mNet->GetState(); }
    /** Wrapper of TCPConnection::GetAddress(). */
    std::string GetAddress() const { return mNet->GetAddress(); }

    /**
     * @brief Resets session.
     *
     * Reset session so we act like client just connected.
     */
    void Reset();

    /**
     * @brief Queues new packet, retaking ownership.
     *
     * @param[in] p Packed to be queued.
     */
    void QueuePacket (EVEPacket* packet, PythonArena* arena);

    /**
     * @brief Pops new packet from queue.
     *
     * @param[in] arena The python arena to be used for allocations
     *
     * @return Popped packet.
     */
    EVEPacket* PopPacket(PythonArena* arena);

    /**
     * @brief Disconnects client from the server
     */
    void CloseClientConnection() { mNet->Disconnect(); }


protected:
    /**
     * @brief Obtains version.
     *
     * @param[in] version Object to be filled with version information.
     */
    virtual void _GetVersion (EVELowLevelVersionExchange& version) = 0;
    /** @return Current count of connected users. */
    //virtual uint32 GetUserCount() = 0;
    /** @return Current position in connection queue. */
    virtual uint32 _GetQueuePosition() = 0;

    /**
     * @brief Verifies version.
     *
     * @param[in] version Version sent by client.
     *
     * @retval true  Verification succeeded; proceeds to next state.
     * @retval false Verification failed; stays in current state.
     */
    virtual bool _VerifyVersion (EVELowLevelVersionExchange& version) = 0;
    /**
     * @brief Verifies VIP key.
     *
     * @param[in] vipKey VIP key sent by client.
     *
     * @retval true  Verification succeeded; proceeds to next state.
     * @retval false Verification failed; stays in current state.
     */
    virtual bool _VerifyVIPKey( const std::string& vipKey ) = 0;
    /**
     * @brief Verifies crypto.
     *
     * @param[in] keyVersion Key version used by the client
     * @param[in] keyParams  Extra parameters for the key used by the client
     *
     * @retval true  Verification succeeded; proceeds to next state.
     * @retval false Verification failed; stays in current state.
     */
    virtual bool _VerifyCrypto (const std::string& keyVersion, PyDict* keyParams) = 0;
    /**
     * @brief Verifies login.
     *
     * @param[in] ccp Login data sent by client.
     *
     * @retval true  Verification succeeded; proceeds to next state.
     * @retval false Verification failed; stays in current state.
     */
    virtual bool _VerifyLogin (EVESecureClientHandshake& ccp) = 0;
    /**
     * @brief Verifies function result.
     *
     * @param[in] challenge_responsehash Hash response
     * @param[in] func_output            Output of the function sent by the server
     *
     * @retval true  Verification succeeded; proceeds to next state.
     * @retval false Verification failed; stays in current state.
     */
    virtual bool _VerifyFuncResult (const std::string& challenge_responsehash, const std::string& func_output) = 0;

    /** Connection of this session. */
    EVETCPConnection* const mNet;

    /** The python arena to use for input data */
    TrackedPythonArena* mArena;

private:
    // State machine facility:
    EVEPacket* ( EVEClientSession::*mPacketHandler )( PyDataType* rep );

    EVEPacket* _HandleVersion( PyDataType* rep );
    EVEPacket* _HandleCommand( PyDataType* rep );
    EVEPacket* _HandleCrypto( PyDataType* rep );
    EVEPacket* _HandleAuthentication( PyDataType* rep );
    EVEPacket* _HandleFuncResult( PyDataType* rep );
    EVEPacket* _HandlePacket( PyDataType* rep );
};

#endif /* !__EVE_SESSION_H__INCL__ */
