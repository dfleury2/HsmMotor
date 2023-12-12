#include "utils.hpp"

#include <fmt/format.h>

#include <fstream>
#include <iostream>
#include <optional>
#include <regex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

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
            os << " G: [" << t.guard.value() << "]";
        }
        if (t.action) {
            os << " / A: " << t.action.value();
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

    std::vector<EventGuardAction> transitions;

    friend std::ostream& operator<<(std::ostream& os, const State& s)
    {
        os << "Name: " << s.name << "\n";
        os << "  is_state_machine: " << s.is_state_machine << ", is_initial: " << s.is_initial << ", has_on_entry: " << s.has_on_entry
           << ", has_on_exit: " << s.has_on_exit << "\n";
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

    std::regex re{// source
                  R"(.*hsm::(?:state|exit)<(.[^>]*)>\s*)"
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

    EventGuardAction t;

    std::smatch match;

    if (std::regex_match(line, match, re)) {
        t.source = match[1];
        if (match[2].matched) {
            t.event = match[2];
        }
        if (match[3].matched) {
            t.guard = match[3];
        }
        if (match[4].matched) {
            t.action = match[4];
        }
        t.target = match[5];
    }
    else {
        throw std::runtime_error("Invalide transition format: " + line);
    }

    return t;
}

// --------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    std::unordered_map<std::string, State> states;
    std::vector<std::string> state_machines;

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

                    state_machines.push_back(currentState);

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

    cout << "-------------------- StateMachines\n";

    std::unordered_set<std::string> active_states;
    cout << "@startuml" << endl;
    cout << "hide empty description" << endl << endl;
    bool init_found = false;
    for (const auto& s : state_machines) {
        cout << "state " << s << " {" << endl;

        for (const auto& t : states[s].transitions) {

            if (!init_found && states[t.source].is_initial) {
                cout << "[*] --> " << t.source << endl;
                init_found = true;
            }

            cout << fmt::format("{} --> {}", t.source, t.target);

            active_states.insert(t.source);
            active_states.insert(t.target);

            std::string transition_text;

            if (t.event) {
                transition_text += t.event.value();
            }
            if (t.guard && !t.guard.value().empty()) {
                transition_text += "[" + t.guard.value() + "]";
            }
            if (t.action && t.action.value() != "log_action") {
                transition_text += "/" + t.action.value();
            }
            if (!transition_text.empty()) {
                cout << " : " << transition_text << endl;
            }
            else {
                cout << endl;
            }
        }

        cout << "}" << endl;
    }
    cout << endl;

    for(const auto& s : active_states) {
        if (auto found = states.find(s); found != end(states)) {
            const State& state = found->second;

            std::string text;
            for (const auto& e : state.entry_actions) {
                text += (text.empty() ? "" : "\\n") + fmt::format("E: {}", e);
            }
            for (const auto& e : state.exit_actions) {
                text += (text.empty() ? "" : "\\n") + fmt::format("X: {}", e);
            }

            if (!text.empty()) {
                cout << state.name << ": " << text << endl;
            }
        }
    }


    cout << endl << "@enduml" << endl;
}
