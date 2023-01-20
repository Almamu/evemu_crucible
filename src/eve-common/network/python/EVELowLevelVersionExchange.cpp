#include "EVELowLevelVersionExchange.h"

#include "python/Types.h"

EVELowLevelVersionExchange::EVELowLevelVersionExchange() {}
EVELowLevelVersionExchange::EVELowLevelVersionExchange(const PyTuple *payload) {
  if (payload->size() != 6)
      throw std::runtime_error("EVELowLevelVersionExchange::EVELowLevelVersionExchange - invalid tuple length");

  this->birthday = payload->at(0)->i32();
  this->macho_version = payload->at(1)->i32();
  this->user_count = payload->at(2)->i32();
  this->version_number = payload->at(3)->decimal();
  this->build_version = payload->at(4)->i32();
  this->project_version = payload->at(5)->string();
}

EVELowLevelVersionExchange::operator PyDataType *() const {
    if (this->has_update_info) {
        return new PyTuple {
            new PyInt (this->birthday),
            new PyInt (this->macho_version),
            new PyInt (this->user_count),
            new PyFloat (this->version_number),
            new PyInt (this->build_version),
            new PyString (this->project_version),
            new PyNone()
        };
    } else {
        return new PyTuple {
            new PyInt (this->birthday),
            new PyInt (this->macho_version),
            new PyInt (this->user_count),
            new PyFloat (this->version_number),
            new PyInt (this->build_version),
            new PyString (this->project_version),
        };
    }
}