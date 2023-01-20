#ifndef EVEMU_UNMARSHALEXCEPTION_H
#define EVEMU_UNMARSHALEXCEPTION_H

#include <string>
#include <exception>

class UnmarshalException {
public:
    UnmarshalException(const char* message, int extra = 0);

    const std::string& what() const;
    const int extra() const;

private:
    const std::string mWhat;
    const int mExtra;
};

#endif // EVEMU_UNMARSHALEXCEPTION_H
