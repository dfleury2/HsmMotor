#pragma once

#include <string>
#include <cxxabi.h>

inline
std::string demangle(const char* mangled)
{
    char* buffer;
    int status;

    buffer = __cxxabiv1::__cxa_demangle(mangled, nullptr, nullptr, &status);

    if (status == 0) {
        std::string n(buffer);
        free(buffer);

        return n;
    }
    else {
        return {"demangle failure"};
    }
}

template<typename T>
auto demangle(const T& t) { return demangle(typeid(t).name()); }

template<typename T>
auto demangle() { return demangle(typeid(std::decay_t<T>).name()); }
