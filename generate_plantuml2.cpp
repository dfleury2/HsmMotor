#include "utils.hpp"

#include <fmt/format.h>

#include <fstream>
#include <iostream>
#include <list>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include <regex>

using namespace std;

// --------------------------------------------------------------------------
struct EventGuardAction {
    std::string source;

    std::optional<std::string> event;
    std::optional<std::string> guard;
    std::optional<std::string> action;

    std::string target;

    friend std::ostream& operator<<(std::ostream& os, const EventGuardAction& t)
    {
        os << " S: [" << t.source << "]";
        if (t.event) {
            os << " + E: [" << t.event.value() << "]";
        }
        if (t.guard) {
            os << " [" << t.guard.value() << "]";
        }
        if (t.action) {
            os << " / " << t.action.value();
        }
        os << " => T: [" << t.target << "]";

        return os;
    }
};

// --------------------------------------------------------------------------
struct State {
    std::string name;
    bool is_state_machine = false;
    bool is_initial = false;
    bool has_on_entry = false;
    bool has_on_exit = false;

    std::vector<std::string> entry_actions;
    std::vector<std::string> exit_actions;

    std::list<EventGuardAction> transitions;

    friend std::ostream& operator<<(std::ostream& os, const State& s) {
        os << "Name: " << s.name << "\n";
        os << "  is_state_machine: " << s.is_state_machine << ", is_initial: " << s.is_initial << ", has_on_entry: " << s.has_on_entry << ", has_on_exit: " << s.has_on_exit << "\n";
        if (!s.entry_actions.empty()) {
            os << "  E: " << fmt::format("{}\n", fmt::join(s.entry_actions, ", "));
        }
        if (!s.exit_actions.empty()) {
            os << "  X: " << fmt::format("{}\n", fmt::join(s.exit_actions, ", "));
        }

        if (!s.transitions.empty()) {
            os << "  T:\n";
            for (const auto& t : s.transitions) {
                os << "  " << t << endl;
            }
        }
        return os;
    }

};

// --------------------------------------------------------------------------
EventGuardAction ExtractTransitionInformation(const std::string& line)
{
//    cout << "Extract Transition from: " << line << endl;

    bool has_event = line.find("event") != std::string::npos;
    bool has_guard = line.find('[') != std::string::npos;
    bool has_action = line.find('/') != std::string::npos;
//    cout << "has_event: " << has_event << ", has_guard: " << has_guard << ", hac_action: " << has_action << endl;

    EventGuardAction t;

    auto split_states = split(line, '=');   // 0: source information, 1: target information
    t.target = trim(split_states.at(1));

    if (has_event) {
        auto split_source = split(split_states.at(0), '+');   // 0: source state, 1: event ...
        t.source = trim(split_source.at(0), " *");

        if (has_guard) {
            auto split_event = split(split_source.at(1), '[');  // 0: event, 1: guard...

            t.event = trim(split_event.at(0));

            if (has_action) {
                auto split_guard = split(split_event.at(1), '/');

                t.guard = trim(split_guard.at(0), " []");
                t.action = trim(split_guard.at(1));
            }
            else {
                t.guard = trim(split_event.at(1), " []");
            }
        }
        else if (has_action) {
            auto split_event = split(split_source.at(1), '/');

            t.event = trim(split_event.at(0));
            t.action = trim(split_event.at(1), " /");
        }
    }
    else if (has_guard) {
        auto split_source = split(split_states.at(0), '[');   // 0: source state, 1: guard ...
        t.source = trim(split_source.at(0));

        if (has_action) {
            auto split_guard = split(split_source.at(1), ']');   // 0: guard , 1: ...
            t.guard = trim(split_guard.at(0));
        }
        else {
            t.guard = trim(split_source.at(1));
        }
    }
    else if (has_action) {
        auto split_source = split(split_states.at(0), '/');   // 0: source state, 1: action
        t.source = trim(split_source.at(0), " *");
        t.action = trim(split_source.at(1));
    }
    else {
        t.source = trim(split_states.at(0), " *");
    }

    static const std::regex re_state("hsm::state<(.*)>");
    t.source = std::regex_replace(t.source, re_state, "$1");
    t.target = std::regex_replace(t.target, re_state, "$1");

    if (t.event) {
        static const std::regex re_event("hsm::event<(.*)>");
        t.event = std::regex_replace(t.event.value(), re_event, "$1");
    }


    return t;
}

// --------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    std::unordered_map<std::string, State> states;

    for (int i = 1; i < argc; ++i) {
        ifstream file(argv[i]);

        std::string currentState;

        for (string line; getline(file, line);) {
            line = trim(line, " \t,");

            bool is_struct = line.find("struct") != std::string::npos;
            bool is_on_entry = line.find("on_entry") != std::string::npos;
            bool is_on_exit = line.find("on_exit") != std::string::npos;
            bool is_transition_table = line.find("hsm::transition_table") != std::string::npos;
            bool is_hsm = line.find("hsm::") != std::string::npos;

            if (is_struct || is_on_entry || is_on_exit || is_transition_table || is_hsm) {
                // cout << "                   [" << (!is_struct ? "    " : "") << line << "]" << endl;

                if (is_struct) {
                    currentState = trim(line.substr(6), " {");

                    states[currentState].name = currentState;

//                    cout << "State: [" << currentState << "] detected" << endl;
                }
                else if (is_on_entry) {
                    states[currentState].has_on_entry = true;
//                    cout << "   on_entry detected" << endl;

                    auto curly_bracket_count = std::count(begin(line), end(line), '{');

                    for (std::string inner_line; getline(file, inner_line);) {
                        curly_bracket_count +=
                            std::count(begin(inner_line), end(inner_line), '{') - std::count(begin(inner_line), end(inner_line), '}');

                        if (inner_line.find("ctx.") != std::string::npos) {
//                            cout << "      action detected: " << inner_line << endl;
                            states[currentState].entry_actions.push_back(trim(inner_line, " ;"));
                        }

                        if (curly_bracket_count == 0) {
                            break;
                        }
                    }
                }
                else if (is_on_exit) {
                    states[currentState].has_on_exit = true;
//                    cout << "   on_exit detected" << endl;

                    auto curly_bracket_count = std::count(begin(line), end(line), '{');

                    for (std::string inner_line; getline(file, inner_line);) {
                        curly_bracket_count +=
                            std::count(begin(inner_line), end(inner_line), '{') - std::count(begin(inner_line), end(inner_line), '}');

                        if (inner_line.find("ctx.") != std::string::npos) {
//                            cout << "      action detected: " << inner_line << endl;
                            states[currentState].exit_actions.push_back(trim(inner_line, " ;"));
                        }

                        if (curly_bracket_count == 0) {
                            break;
                        }
                    }
                }
                else if (is_transition_table) {
//                    cout << "   transition table detected" << endl;

                    states[currentState].is_state_machine = true;

                    std::string inner_line;
                    do {
                    } while (getline(file, inner_line) && inner_line.find("hsm::") == std::string::npos);

                    ifstream::pos_type current_pos{};
                    do {
                        inner_line = trim(inner_line, " \t,");
//                        cout << "----------[" << inner_line << "]" << endl;
                        bool is_initial = (inner_line[0] == '*');

                        auto transition = ExtractTransitionInformation(trim(inner_line, " ;"));
//                        cout << "Transition: \n" << transition << endl;

                        if (is_initial) {
                            states[transition.source].is_initial = true;
                        }

                        states[currentState].transitions.push_back(transition);

                        current_pos = file.tellg();
                    } while (getline(file, inner_line) && inner_line.find("hsm::") != std::string::npos);

                    file.seekg(current_pos);
                }
            }
        }
    }

    for (auto&& [name, s] : states) {
        cout << "-------------------- " << name << "\n";
        cout << s << endl;
    }

//    cout << "@startuml" << endl << endl;
//    cout << endl << "@enduml" << endl;
}
