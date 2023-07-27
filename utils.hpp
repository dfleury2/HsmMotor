#pragma once

#include <string>
#include <cxxabi.h>

inline
std::string demangle(const std::string& mangled)
{
    char* buffer;
    int status;

    buffer = __cxxabiv1::__cxa_demangle(mangled.c_str(), nullptr, nullptr, &status);

    if (status == 0) {
        std::string n(buffer);
        free(buffer);

        return n;
    }
    else {
        return {"demangle failure"};
    }
}
