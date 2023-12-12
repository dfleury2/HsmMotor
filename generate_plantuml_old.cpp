#include "utils.hpp"

#include <iostream>
#include <string>
#include <vector>

using namespace std;

// --------------------------------------------------------------------------
std::string get_state(const std::string& str)
{
    static const string StateBegin = "hsm::state<";
    static const string StateEnd = ">";

    auto start_position = str.find(StateBegin) + StateBegin.size();
    auto end_position = str.find(StateEnd);

    return str.substr(start_position, end_position - start_position);
}

// --------------------------------------------------------------------------
std::string get_event(const std::string& str)
{
    static const string StateBegin = "hsm::event<";
    static const string StateEnd = ">";

    string result;

    if (auto start_position = str.find(StateBegin) + StateBegin.size(); start_position != string::npos) {
        auto end_position = str.find(StateEnd, start_position);

        result = str.substr(start_position, end_position - start_position);
    }

    return result;
}

// --------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    cout << "@startuml" << endl;
    for (string line; getline(cin, line);) {
        auto found = line.find("hsm::state");
        if (found != string::npos) {
            // Transition detected
            line = trim(line, " \t,");
            for (size_t pos = 0; (pos = line.find("  ")) != string::npos;) {
                line.erase(pos, 1);
            }
            //            cout << "                   [" << line << "]" << endl;

            auto sourceTarget = split(line, '=');
            if (sourceTarget.size() != 2) {
                // No target ?
                continue;
            }
            if (line.find('*') != std::string::npos) {
                cout << "[*] --> " << get_state(sourceTarget[0]) << endl;
            }

            cout << get_state(sourceTarget[0]) << " --> " << get_state(sourceTarget[1]);

            auto event = get_event(sourceTarget[0]);
            if (!event.empty()) {
                cout << " : " << event;
            }

            cout << endl;
        }
    }
    cout << "@enduml" << endl;
}
