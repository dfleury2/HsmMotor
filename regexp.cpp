#include <algorithm>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>

using namespace std;

int main(int argc, char* argv[])
{
    // Using multiple regex_search
    std::regex re_source(R"(hsm::state<\s*(\w*)\s*>.*=)");
    std::regex re_target(R"(=\s*hsm::state<\s*(\w*)\s*>)");
    std::regex re_event(R"(\+\s*hsm::event<\s*(\w*)\s*>)");
    std::regex re_guard(R"(\[\s*(\w*)\s*\])");


    std::regex re{// source
                  R"(hsm::state<(\w*)>\s*)"
                  // optional event
                  R"(\+?\s*)"
                  R"((?:hsm::event<)?(\w*)?(?:>)?\s*)"
                  // optional guard
                  R"(\[?(\w*)\]?\s*)"
                  // optional action
                  R"(/?\s*)"
                  R"((\w*)\s*)"
                  // target
                  R"(=\s*)"
                  R"(hsm::state<(\w*)>)"};

    ifstream file(argv[1]);

    for (std::string line; getline(file, line);) {
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
