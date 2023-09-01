#include <iostream>
#include <string>
#include <vector>

using namespace std;

// --------------------------------------------------------------------------
std::vector<std::string> split(const std::string& str, char sep)
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
std::string trim_left(const std::string& str, const char* tokens)
{
    std::string::size_type begin = str.find_first_not_of(tokens);
    if (begin == std::string::npos) {
        begin = str.size();
    }

    return {str, begin};
}

// --------------------------------------------------------------------------
std::string trim_right(const std::string& str, const char* tokens)
{
    std::string::size_type end = str.find_last_not_of(tokens);
    if (end == std::string::npos) {
        end = -1;
    }

    return {str, 0, end + 1};
}

// --------------------------------------------------------------------------
std::string trim(const std::string& str, const char* tokens = " \t")
{
    return trim_left(trim_right(str, tokens), tokens);
}

std::string get_state(const std::string& str)
{
    static const string StateBegin = "hsm::state<";
    static const string StateEnd = ">";

    auto start_position = str.find(StateBegin) + StateBegin.size();
    auto end_position = str.find(StateEnd);

    return str.substr(start_position, end_position - start_position);
}

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
