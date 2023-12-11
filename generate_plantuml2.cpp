#include "utils.hpp"

#include <fstream>
#include <iostream>
#include <list>
#include <vector>
#include <optional>
#include <string>
#include <unordered_map>

using namespace std;

// --------------------------------------------------------------------------
struct EventGuardAction {
    std::optional<std::string> event;
    std::optional<std::string> guard;
    std::optional<std::string> action;

    std::string target;
};

// --------------------------------------------------------------------------
struct State {
    std::string name;
    bool is_initial = false;
    bool has_on_entry = false;
    bool has_on_exit = false;

    std::vector<std::string> entry_actions;
    std::vector<std::string> exit_actions;

    std::list<EventGuardAction> transitions;
};

// --------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    std::unordered_map<std::string, State> states;

    for (int i = 1; i < argc; ++i) {
        ifstream file(argv[i]);

        std::string currentState;

        cout << "@startuml" << endl << endl;
        for (string line; getline(file, line);) {
            line = trim(line, " \t,");

            bool is_struct = line.find("struct") != std::string::npos;
            bool is_on_entry = line.find("on_entry") != std::string::npos;
            bool is_on_exit = line.find("on_exit") != std::string::npos;
            bool is_transition_table = line.find("hsm::transition_table") != std::string::npos;
            bool is_hsm = line.find("hsm::") != std::string::npos;

            if (is_struct || is_on_entry || is_on_exit || is_transition_table || is_hsm) {
                //cout << "                   [" << (!is_struct ? "    " : "") << line << "]" << endl;

                if (is_struct) {
                    currentState = trim(line.substr(6), " {");

                    cout << "State: [" << currentState << "] detected" << endl;
                }
                else if (is_on_entry) {
                    states[currentState].has_on_entry = true;
                    cout << "   on_entry detected" << endl;

                    auto curly_bracket_count = std::count(begin(line), end(line), '{');

                    for (std::string inner_line; getline(file, inner_line);) {
                        curly_bracket_count +=  std::count(begin(inner_line), end(inner_line), '{') -  std::count(begin(inner_line), end(inner_line), '}');

                        if (inner_line.find("ctx.") != std::string::npos) {
                            cout << "      action detected: " << inner_line << endl;
                            states[currentState].entry_actions.push_back(inner_line);
                        }

                        if (curly_bracket_count == 0) {
                            break;
                        }
                    }
                }
                else if (is_on_exit) {
                    states[currentState].has_on_exit = true;
                    cout << "   on_exit detected" << endl;

                    auto curly_bracket_count = std::count(begin(line), end(line), '{');

                    for (std::string inner_line; getline(file, inner_line);) {
                        curly_bracket_count +=  std::count(begin(inner_line), end(inner_line), '{') -  std::count(begin(inner_line), end(inner_line), '}');

                        if (inner_line.find("ctx.") != std::string::npos) {
                            cout << "      action detected: " << inner_line << endl;
                            states[currentState].exit_actions.push_back(inner_line);
                        }

                        if (curly_bracket_count == 0) {
                            break;
                        }
                    }
                }
                else if (is_transition_table) {
                    cout << "   transition table detected" << endl;

                    std::string inner_line;
                    do {
                    } while (getline(file, inner_line) && inner_line.find("hsm::") == std::string::npos);

                    ifstream::pos_type current_pos = file.tellg();
                    do {
                        inner_line = trim(inner_line, " \t,");
                        cout << "           [" << inner_line << "]" << endl;
                        if (inner_line[0] == '*') {
                            states[currentState].is_initial = true;
                        }

                        current_pos = file.tellg();
                    } while (getline(file, inner_line) && inner_line.find("hsm::") != std::string::npos);

                    file.seekg(current_pos);
                }
            }
        }
        cout << endl << "@enduml" << endl;
    }
}
