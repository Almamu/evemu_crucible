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

#ifndef __BOUNDSERVICE_H__
#define __BOUNDSERVICE_H__

#include "services/Callable.h"
#include "services/Service.h"
#include "Client.h"

typedef uint32_t BoundID;

template<class T> class Service;
class EVEServiceManager;
class EVEResult;
class EVECallArgs;
class Client;

class BoundDispatcher {
public:
    /** @returns BoundID The id of the bound service */
    virtual BoundID GetBoundID() const = 0;
    /** @returns PyTuple* The OID of the bound object */
    virtual PyTuple* GetOID(PythonArena* arena = HeapPythonArena::instance) const = 0;
    /** @returns The string ID */
    virtual const std::string& GetIDString() const = 0;
    /**
     * @brief Handles dispatching a call to this service
     */
    virtual EVEResult Dispatch(const std::string& name, EVECallArgs& args) = 0;
    /**
     * @brief Builds a string with information about calling a method in this service
     */
    virtual std::string DebugDispatch (const std::string& name) = 0;
    /**
     * @brief Increases the number of references to this bound object
     */
    virtual void NewReference (Client* client) = 0;
    /**
     * @brief Releases this dispatcher and frees any kept resources
     * @returns Whether the bound object was destroyed or someone still has a reference to it
     */
    virtual bool Release (Client* client) = 0;
protected:
    /**
     * @brief Check performed before dispatching a call to this bound service
     */
    virtual bool CanClientCall(Client* client) = 0;
};
/**
 * Parent for any of the bound services
 *
 * @tparam Bound
 */
template <class Bound>
class BoundServiceParent {
public:
    /**
     * Called by the bound services when It's getting removed
     *
     * @param bound
     */
    virtual void BoundReleased (Bound* bound) = 0;
};

/**
 * @brief Service that exposes the mechanism for binding services.
 * 
 * Due to how EVE requests these bindings sometimes call info is included,
 * so this class provides all the required mechanisms to handle the whole proccess
 */
template<class Svc, class Bound>
class BindableService : public Service<Svc>, public BoundServiceParent<Bound> {
public:
    BindableService(const std::string& name, EVEServiceManager& mgr, AccessLevel level = eAccessLevel_None) :
        Service <Svc>(name, level),
        mManager(mgr)
    {
        this->Add("MachoResolveObject", &BindableService<Svc, Bound>::MachoResolveObject);
        this->Add("MachoBindObject", &BindableService<Svc, Bound>::MachoBindObject);
    }

    EVEResult MachoResolveObject(EVECallArgs& args, PyDataType* bindParameters, PyDataType* justQuery) {
        return args.arena.Int(this->GetServiceManager().GetNodeID());
    }

    EVEResult MachoBindObject(EVECallArgs& args, PyDataType* bindParameters, std::optional<PyTuple*> call) {
        // register the new instance in the service manager
        BoundDispatcher* bound = this->BindObject(args.client, bindParameters);

        // binding failed for whatever reason, just return none and get on with our lifes
        if (bound == nullptr) {
            return nullptr;
        }

        // add the client to the allowed list
        bound->NewReference (args.client);

        // build the bound service identifier
        PyDataType* response;
        // TODO: CHECK THIS CHECK ARENA OWNERSHIP TO ENSURE IT'S ACTUALLY A GOOD THING TO IGNORE OR IF WE'RE BETTER JUST
        // TODO: CLONING THE OID
        PyDict* byName = args.arena.Dict({
            {"OID+", bound->GetOID(&args.arena)}
        });

        bool checkArenaOwnership = true;

        if (call.has_value() == false) {
            response = args.arena.None();
        } else {
            auto tuple = call.value();

            if (tuple->size() != 3) {
                throw std::runtime_error ("MachoBindObject - tuple is the wrong size, expected 3");
            }

            auto method_name = tuple->at(0)->string();
            auto arguments = tuple->at<PyTuple>(1);
            auto dict_arguments = tuple->at<PyDict>(2);

            _log(SERVICE__MESSAGE, "%s Service: MachoBindObject also contains call to %s", this->GetName().c_str(), method_name.c_str());

            EVECallArgs subArgs(args.client, arguments, dict_arguments, args.arena);

            EVEResult result = bound->Dispatch(method_name, subArgs);

            // set the tuple data
            response = result.result.has_value() ? result.result.value() : args.arena.None();
            // TODO: merge the dicts to return the full response data?
            // Py types are lacking lots of helper methods that could be useful

            checkArenaOwnership = result.check_arena_owner;

            if (result.check_arena_owner && response->arena() != &args.arena) {
                throw std::runtime_error (
                    "The result data does not belong to the right arena. Make sure to use args.arena to create result objects"
                );
            }
        }

        // return the response
        return EVEResult (
            args.arena.Tuple ({
                args.arena.SubStruct(args.arena.SubStream(bound->GetOID(&args.arena))),
                response
            }, checkArenaOwnership),
            byName
        );
    }

protected:
    /**
     * @brief Handles the creation of the bound service
     */
    virtual BoundDispatcher* BindObject(Client* client, PyDataType* bindParameters) = 0;
    /** @returns The service manager this service is registered in */
    EVEServiceManager& GetServiceManager() const { return mManager; }
private:
    EVEServiceManager& mManager;
};

/**
 * @brief Implementation of EVE Online's bound objects
 * 
 * @see https://learn.microsoft.com/en-us/windows/win32/com/monikers
 */
template<class Bound>
class EVEBoundObject : public BoundDispatcher {
protected:
    EVEBoundObject(EVEServiceManager& mgr, BoundServiceParent<Bound>& parent) :
        mManager(mgr),
        mParent (parent)
    {
        mBoundId = this->GetServiceManager().RegisterBoundService(this);

        // build the id string
        std::stringstream strBuilder;
        strBuilder << "N=" << this->GetServiceManager().GetNodeID() << ":" << mBoundId;

        // store it
        mIdString = strBuilder.str();

        // build the OID
        mOID = mArena.Tuple ({
            mArena.String (mIdString),
            mArena.Int (GetFileTimeNow()) // this isn't really the datetime, should be an unique ID
        });
    }

    /**
     * Default destructor, override to free resources if needed
     */
    virtual ~EVEBoundObject () {
    }

    /**
     * @brief Registers a method handler
     */
    template <class H, class... Args>
    void Add(const std::string& name, EVEResult (H::*callHandler)(EVECallArgs&, Args...)) {
        mHandlers.push_back(std::make_pair(std::string(name), new CallHandler <H> (callHandler)));
    }

public:
    /**
     * @brief Handles dispatching a call to this service
     */
  EVEResult Dispatch(const std::string& name, EVECallArgs& args) override {
        if (this->CanClientCall(args.client) == false)
            throw CustomError("This client is not allowed to call this bound service");

        for (auto handler : mHandlers) {
            if (handler.first != name)
                continue;

            try {
                return (*handler.second)(reinterpret_cast <void*> (this), args);
            } catch (std::invalid_argument) {
                // ignored, this just means the function does not match the possible calls
            }
        }

        throw method_not_found ();
    }

    /**
     * @brief Builds a string with information about calling a method in this service
     */
    std::string DebugDispatch (const std::string& name) override {
        std::string result = name + " candidates: \n";

        for (auto handler : mHandlers) {
            if (handler.first != name)
                continue;

            result += "\t(" + handler.second->getSignature () + ")";
            result += "\n";
        }

        return result;
    }

    /**
     * @brief Increases the number of references to this bound object
     */
    void NewReference (Client* client) override {
        // ensure the client is not there yet
        auto it = mClients.find (client);

        if (it != mClients.end ())
            return;

        // the client didn't hold a reference to this service
        // so add it to the list and increase the RefCount
        mClients.insert_or_assign (client, true);
        // also add it to the bind list of the client
        client->AddBindID (this->GetBoundID ());
    }
    /**
     * @brief Signals this EVEBoundObject that one of the clients that held a reference, released it
     * @returns Whether the bound object was destroyed or someone still has a reference to it
     */
    bool Release(Client* client) override {
        auto it = mClients.find (client);

        // the client doesn't have access to this bound service, so nothing has to be done
        if (it == mClients.end ())
            return false;

        // remove the client for the list, and if that's the last one, free the service
        mClients.erase (it);

        if (mClients.size () == 0) {
            this->GetParent ().BoundReleased (reinterpret_cast <Bound*> (this));
            delete this; // we hate this
            return true;
        }

        return false;
    }

    bool CanClientCall(Client* client) override {
        return mClients.find (client) != mClients.end();
    }

    /** @returns BoundID The id of the bound service */
    BoundID GetBoundID() const override { return mBoundId; }
    /** @returns The normal service that created this service */
    BoundServiceParent<Bound>& GetParent () const { return mParent; }
    /** @returns PyTuple* The (cloned) OID of the bound object */
    PyTuple* GetOID(PythonArena* arena = HeapPythonArena::instance) const override { return mOID->clone (arena); }
    /** @returns The string ID */
    const std::string& GetIDString() const override { return mIdString; }
    /** @returns The service manager this service is registered in */
    EVEServiceManager& GetServiceManager() const { return mManager; }
    /** @returns The list of clients that requested access to this service */
    const std::map <Client*, bool>& GetBoundClients () const { return mClients; }
private:
    /** @var The service manager this bound service is registered in */
    EVEServiceManager& mManager;
    /** @var The OID of the service */
    PyTuple* mOID;
    /** @var The ID string that identifies this service for the clients */
    std::string mIdString;
    /** @var The normal service that created this bound object */
    BoundServiceParent<Bound>& mParent;
    /** @var The numeric ID of the bound service */
    BoundID mBoundId;
    /** @var The map of handlers for this service */
    std::vector <std::pair <std::string, CallHandlerBase*>> mHandlers;
    /** @var The clients that have access to this bound service */
    std::map <Client*, bool> mClients;
    /** @var The memory arena used to track bound-service specific Py types */
    TrackedPythonArena mArena;
};

#endif /* !__BOUNDSERVICE_H__ */