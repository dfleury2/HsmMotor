#pragma once

#include <string>

#ifdef __linux__
#include <cxxabi.h>
#endif

inline
std::string demangle(const char* mangled)
{
#ifdef __linux__
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
#else
    return mangled;
#endif
}

template<typename T>
auto demangle(const T& t) { return demangle(typeid(t).name()); }

template<typename T>
auto demangle() { return demangle(typeid(std::decay_t<T>).name()); }
