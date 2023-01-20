//
// Created by games on 15/01/2023.
//

#ifndef EVEMU_CALLABLE_H
#define EVEMU_CALLABLE_H

#include <map>
#include <optional>

#include "eve-server.h"

class Client;

enum AccessLevel {
    eAccessLevel_None = 0,
    eAccessLevel_Location = 1,
    eAccessLevel_LocationPreferred = 2,
    eAccessLevel_SolarSystem = 3,
    eAccessLevel_SolarSystem2 = 4,
    eAccessLevel_Station = 5,
    eAccessLevel_Character = 6,
    eAccessLevel_Corporation = 7,
    eAccessLevel_User = 8
};

class method_not_found {
public:
    method_not_found() {}
};

/**
 * Value class that contains parameters related to a RPC call from the client
 */
class EVECallArgs {
public:
    EVECallArgs (Client* c, PyTuple* tup, PyDict* dict, PythonArena& arena);
    ~EVECallArgs () = default;

    void dump(LogType type) const;

    /** The python arena to use while allocating data for responses */
    PythonArena& arena;
    /** The client that performed the service call */
    Client* const client;    //we do not own this
    /** The parameters tuple */
    PyTuple* tuple;        //we own this, but it may be taken
    /** The named payload of the request */
    std::map<std::string, PyDataType*> byname;    //we own this, but elements may be taken.
};

class EVEResult {
public:
    EVEResult (bool checkArenaOwner = true);
    EVEResult (PyDataType* result, bool checkArenaOwner = true);
    EVEResult (PyDataType* result, PyDict* namedResult, bool checkArenaOwner = true);
    ~EVEResult () = default;

    std::optional<PyDataType*> result;
    std::optional<PyDict*> named_result;
    bool check_arena_owner;
};


template <class T> struct is_optional : std::false_type {};
template <class T> struct is_optional<std::optional<T>> : std::true_type {};

struct CallHandlerBase {
public:
    virtual EVEResult operator() (void* service, EVECallArgs& args) const = 0;
    virtual const std::string& getSignature () = 0;
};

template <class S>
struct CallHandler : public CallHandlerBase {
    template<class... Args> CallHandler(EVEResult (S::*callHandler)(EVECallArgs&, Args...)) :
            erasedHandler(reinterpret_cast <EVEResult (S::*)()> (callHandler)),
            handlerImpl {
                [this](S* service, EVEResult (S::*erasedHandler)(), EVECallArgs& args) -> EVEResult {
                    auto handler = reinterpret_cast <EVEResult (S::*)(EVECallArgs&, Args...)> (erasedHandler);

                    if constexpr (sizeof...(Args) == 0) {
                        // ensure there's no arguments in the data
                        if (args.tuple->size() != 0)
                            throw std::invalid_argument("expected 0 arguments");

                        // all the functions must have the PyCallArgs to know important info about the call
                        // like the client that sent it
                        return (service->*handler) (args);
                    } else {
                        if (this->validateArgs <std::decay_t<Args>...>(args) == false)
                            throw std::invalid_argument("arguments do not match");

                        return this->apply(service, handler, args);
                    }
                }
            }
    {
        this->generateSignature <std::decay_t <Args>...> ();
    }

    EVEResult operator() (void* service, EVECallArgs& args) const override {
        return handlerImpl(reinterpret_cast <S*> (service), erasedHandler, args);
    }

    const std::string& getSignature () {
        return this->signature;
    }

private:
    template <typename T> bool validateArg(size_t index, EVECallArgs& args) {
        // handle optional values
        if constexpr (is_optional <T>::value) {
            // treat Nones on optional values as valid
            if (index >= args.tuple->size() || args.tuple->at (index)->is<PyNone>()) {
                return true;
            } else {
                return validateArg <typename T::value_type>(index, args);
            }
        }

        if (index >= args.tuple->size()) {
            return false;
        }

        if constexpr (std::is_same_v <T, PyDataType*>)
            return true;

        PyDataType* rep = args.tuple->at (index);

        // validate type with their parameter equivalent
        if constexpr (std::is_same_v <T, PyBool*>)
            return rep->is<PyBool>();
        if constexpr (std::is_same_v <T, PyInt*>)
            return rep->is<PyInt>();
        if constexpr (std::is_same_v <T, PyFloat*>)
            return rep->is<PyFloat>();
        if constexpr (std::is_same_v <T, PyBuffer*>)
            return rep->is<PyBuffer>();
        if constexpr (std::is_same_v <T, PyString*>)
            return rep->is<PyString>();
        if constexpr (std::is_same_v <T, PyToken*>)
            return rep->is<PyToken>();
        if constexpr (std::is_same_v <T, PyTuple*>)
            return rep->is<PyTuple>();
        if constexpr (std::is_same_v <T, PyList*>)
            return rep->is<PyList>();
        if constexpr (std::is_same_v <T, PyDict*>)
            return rep->is<PyDict>();
        if constexpr (std::is_same_v <T, PyNone*>)
            return rep->is<PyNone>();
        if constexpr (std::is_same_v <T, PySubStruct*>)
            return rep->is<PySubStruct>();
        if constexpr (std::is_same_v <T, PySubStream*>)
            return rep->is<PySubStream>();
        if constexpr (std::is_same_v <T, PyObject*>)
            return rep->is<PyObject>();
        if constexpr (std::is_same_v <T, PyObjectEx*>)
            return rep->is<PyObjectEx>();
        if constexpr (std::is_same_v <T, PyPackedRow*>)
            return rep->is<PyPackedRow>();

        return false;
    }

    template <typename T> decltype(auto) getAs(size_t index, PyTuple* tup) {
        if constexpr (is_optional <T>::value) {
            if (index >= tup->size() || tup->at (index)->is<PyNone>()) {
                return T{};
            } else {
                return std::make_optional(getAs <typename T::value_type>(index, tup));
            }
        }

        if (index >= tup->size()) {
            throw std::runtime_error("This should not happen. Trying to get parameter out of bounds. What happened to the validation?!");
        }

        PyDataType* rep = tup->at (index);

        if constexpr (std::is_same_v <T, PyDataType*>)
            return rep;
        if constexpr (std::is_same_v <T, PyBool*>)
            return rep->as<PyBool>();
        if constexpr (std::is_same_v <T, PyInt*>)
            return rep->as<PyInt>();
        if constexpr (std::is_same_v <T, PyFloat*>)
            return rep->as<PyFloat>();
        if constexpr (std::is_same_v <T, PyBuffer*>)
            return rep->as<PyBuffer>();
        if constexpr (std::is_same_v <T, PyString*>)
            return rep->as<PyString>();
        if constexpr (std::is_same_v <T, PyToken*>)
            return rep->as<PyToken>();
        if constexpr (std::is_same_v <T, PyTuple*>)
            return rep->as<PyTuple>();
        if constexpr (std::is_same_v <T, PyList*>)
            return rep->as<PyList>();
        if constexpr (std::is_same_v <T, PyDict*>)
            return rep->as<PyDict>();
        if constexpr (std::is_same_v <T, PyNone*>)
            return rep->as<PyNone>();
        if constexpr (std::is_same_v <T, PySubStruct*>)
            return rep->as<PySubStruct>();
        if constexpr (std::is_same_v <T, PySubStream*>)
            return rep->as<PySubStream>();
        if constexpr (std::is_same_v <T, PyObject*>)
            return rep->as<PyObject>();
        if constexpr (std::is_same_v <T, PyObjectEx*>)
            return rep->as<PyObjectEx>();
        if constexpr (std::is_same_v <T, PyPackedRow*>)
            return rep->as<PyPackedRow>();
    }

    template<class... Args>
    bool validateArgs(EVECallArgs& args) {
        constexpr size_t ArgCount = sizeof... (Args);
        const bool argCountIsValid = args.tuple->size() <= ArgCount;

        if (argCountIsValid == false) {
            return false;
        }

        bool isValid = true;
        size_t index = 0;

        ((
            isValid = validateArg<Args>(index, args) && isValid, ++index
        ), ...);

        return isValid;
    }

    template <typename T> std::string translateParameter() {
        // handle optional values
        if constexpr (is_optional <T>::value) {
            // treat Nones on optional values as valid
            return "std::optional<" + translateParameter <typename T::value_type>() + ">";
        }

        if constexpr (std::is_same_v <T, PyDataType*>)
            return "PyDataType*";

        // validate type with their parameter equivalent
        if constexpr (std::is_same_v <T, PyBool*>)
            return "PyBool*";
        if constexpr (std::is_same_v <T, PyInt*>)
            return "PyInt*";
        if constexpr (std::is_same_v <T, PyInt*>)
            return "PyInt*";
        if constexpr (std::is_same_v <T, PyFloat*>)
            return "PyFloat*";
        if constexpr (std::is_same_v <T, PyBuffer*>)
            return "PyBuffer*";
        if constexpr (std::is_same_v <T, PyString*>)
            return "PyString*";
        if constexpr (std::is_same_v <T, PyToken*>)
            return "PyToken*";
        if constexpr (std::is_same_v <T, PyTuple*>)
            return "PyTuple*";
        if constexpr (std::is_same_v <T, PyList*>)
            return "PyList*";
        if constexpr (std::is_same_v <T, PyDict*>)
            return "PyDict*";
        if constexpr (std::is_same_v <T, PyNone*>)
            return "PyNone*";
        if constexpr (std::is_same_v <T, PySubStruct*>)
            return "PySubStruct*";
        if constexpr (std::is_same_v <T, PySubStream*>)
            return "PySubStream*";
        if constexpr (std::is_same_v <T, PyObject*>)
            return "PyObject*";
        if constexpr (std::is_same_v <T, PyObjectEx*>)
            return "PyObjectEx*";
        if constexpr (std::is_same_v <T, PyPackedRow*>)
            return "PyPackedRow*";

        return "Unknown";
    }

    template<class... Args>
    void generateSignature () {
        constexpr size_t length = sizeof... (Args);
        size_t index = 0;

        ((
            signature += translateParameter <Args> (), signature += (index < (length - 1) ? "," : ""), ++ index
        ), ...);
    }

    template<class... Args, size_t... I>
    EVEResult applyImpl(
            S* service, EVEResult (S::*handler)(EVECallArgs& args, Args...), EVECallArgs& args,
            std::index_sequence<I...>) {
        return (service->*handler) (args, getAs<std::decay_t<Args>>(I, args.tuple)...);
    }

    template<class... Args>
    EVEResult apply(
            S* service, EVEResult (S::*handler) (EVECallArgs& args, Args...), EVECallArgs& args) {
        return applyImpl(
                service, handler, args,
                std::make_index_sequence <sizeof...(Args)>{}
        );
    }

    EVEResult (S::*erasedHandler)() = nullptr;
    std::function <EVEResult (S* service, EVEResult (S::* erasedHandler)(), EVECallArgs& args)> handlerImpl;
    std::string signature;
};

#endif //EVEMU_CALLABLE_H
