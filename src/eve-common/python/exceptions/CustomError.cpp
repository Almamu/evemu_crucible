#include "CustomError.h"

CustomError::CustomError(const char *fmt, ...)
: UserError (EXCEPTION_NAME)
{
    va_list va;
    va_start (va, fmt);

    char* str = nullptr;
    vasprintf (&str, fmt, va);
    assert (str);

    va_end (va);

    this->AddFormatValue ("error", new PyString (str));

    SafeFree (str);
}

const char* CustomError::EXCEPTION_NAME = "CustomError";