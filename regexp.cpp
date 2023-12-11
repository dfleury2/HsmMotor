#include <algorithm>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>

using namespace std;

int main(int argc, char* argv[])
{
    std::regex re{// source
                  R"(hsm::state<(\w)>)"
                  // optional event
                  R"(\+?)"
                  R"((?:hsm::event<)?(\w*)(?:>)?)"
                  // optional guard
                  R"(\[?(\w*)\]?)"
                  // optional action
                  R"(/?)"
                  R"((\w*))"
                  // target
                  R"(=)"
                  R"(hsm::state<(\w)>)"};

    ifstream file(argv[1]);

    for (std::string line; getline(file, line);) {
        line.erase(std::remove_if(begin(line), end(line), [](auto c) { return std::isspace(c); }), end(line));
        cout << "[" << line << "]" << endl;

        std::smatch match;

        if (std::regex_match(line, match, re)) {
            const char* names[] = {"", "source", "event ", "guard ", "action", "target", "", "", ""};
            for (size_t i = 1; i < match.size(); ++i) {
                cout << "   m[" << i << "]: " << names[i] << ": [" << match[i] << "]" << endl;
            }
        }
    }
}
