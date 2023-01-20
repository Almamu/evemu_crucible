#include "Callable.h"
#include "Client.h"

EVECallArgs::EVECallArgs (Client* c, PyTuple* tup, PyDict* dict, PythonArena& arena) :
    client (c),
    tuple (tup),
    arena (arena) {
    for (auto cur : *dict)
        byname[cur.first->string()] = cur.second;
}

void EVECallArgs::dump(LogType type) const {
    if (!is_log_enabled(type))
        return;

    _log(type, "  Call Arguments:");
    tuple->dump(type, "    ");
    if (byname.empty()) {
        return;
    }

    _log(type, " Named Arguments:");
    for (auto cur : byname) {
        _log(type, "  %s", cur.first.c_str());
        cur.second->dump(type, "    ");
    }
}

/* EVEResult */
EVEResult::EVEResult (bool checkArenaOwner) :
    result (std::nullopt),
    named_result (std::nullopt),
    check_arena_owner (checkArenaOwner) {
}

EVEResult::EVEResult (PyDataType* result, bool checkArenaOwner) :
    result (result != nullptr ? std::optional<PyDataType*>(result) : std::nullopt),
    named_result (std::nullopt),
    check_arena_owner (checkArenaOwner) {
}

EVEResult::EVEResult (PyDataType* result, PyDict* namedResult, bool checkArenaOwner) :
    result (result != nullptr ? std::optional<PyDataType*>(result) : std::nullopt),
    named_result (namedResult),
    check_arena_owner (checkArenaOwner) {
}
