#include "UnmarshalException.h"

UnmarshalException::UnmarshalException(const char *message, int extra) :
  mWhat(message), mExtra(extra)
{}

const std::string& UnmarshalException::what() const {
    return mWhat;
}

const int UnmarshalException::extra() const {
    return mExtra;
}