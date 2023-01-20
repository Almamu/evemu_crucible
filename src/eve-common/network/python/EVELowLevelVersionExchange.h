#pragma once

#include <string>

class PyDataType;
class PyTuple;

class EVELowLevelVersionExchange {
public:
    EVELowLevelVersionExchange();
    explicit EVELowLevelVersionExchange(const PyTuple* payload);

    int birthday;
    int macho_version;
    int user_count;
    double version_number;
    int build_version;
    std::string project_version;
    // this doesn't use any value, so for now a simple flag to add it or not is enough
    bool has_update_info;

    PyDataType* operator= (const EVELowLevelVersionExchange& oth) const;
    operator PyDataType*() const;
};
