#pragma once

#include <string>
#include <typeinfo>
#include <vector>

#ifdef __linux__
#include <cxxabi.h>
#endif

inline std::string demangle(const char* mangled)
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
        return mangled;
    }
#else
    return mangled;
#endif
}

template <typename T>
auto demangle(const T& t)
{
    return demangle(typeid(t).name());
}

template <typename T>
auto demangle()
{
    return demangle(typeid(std::decay_t<T>).name());
}

// --------------------------------------------------------------------------
inline std::vector<std::string> split(const std::string& str, char sep)
{
    std::vector<std::string> columns;
    size_t begin = 0;
    while (begin < str.size()) {
        size_t end = str.find(sep, begin);
        if (end == std::string::npos)
            end = str.size();

        columns.push_back(str.substr(begin, end - begin));
        begin = end + 1;
    }
    if (str.empty() || *str.rbegin() == sep)
        columns.emplace_back("");

    return columns;
}

// --------------------------------------------------------------------------
inline std::string trim_left(const std::string& str, const char* tokens)
{
    std::string::size_type begin = str.find_first_not_of(tokens);
    if (begin == std::string::npos) {
        begin = str.size();
    }

    return {str, begin};
}

// --------------------------------------------------------------------------
inline std::string trim_right(const std::string& str, const char* tokens)
{
    std::string::size_type end = str.find_last_not_of(tokens);
    if (end == std::string::npos) {
        end = -1;
    }

    return {str, 0, end + 1};
}

// --------------------------------------------------------------------------
inline std::string trim(const std::string& str, const char* tokens = " \t")
{
    return trim_left(trim_right(str, tokens), tokens);
}
