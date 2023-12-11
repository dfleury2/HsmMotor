#include <fstream>
#include <iostream>
#include <optional>
#include <regex>
#include <string>

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
        os << " source: [" << t.source << "]";
        if (t.event) {
            os << "\n event: [" << t.event.value() << "]";
        }
        if (t.guard) {
            os << "\n guard: [" << t.event.value() << "]";
        }
        if (t.action) {
            os << "\n action: [" << t.action.value() << "]";
        }
        os << "\n target: [" << t.target << "]";

        return os;
    }
};

int main(int argc, char* argv[])
{
//    std::regex re{".*(hsm::state<.[^>]*>).*\\+?.*(hsm::event<.[^>]*>)?.*(/.[^=]*)?.*(hsm::state<.[^>]*>).*"};
//    std::regex re{".*(hsm::state<.[^>]*>).*(hsm::event<.[^>]*>)?.*(/.[^=]*)?=?.*(hsm::state<.[^>]*>).*"};
    std::regex re{".*(hsm::event<.[^>]*>).*(hsm::state<.[^>]*>).*"};



    ifstream file(argv[1]);

    for (std::string line; getline(file, line);) {
        cout << "[" << line << "]" << endl;
        std::smatch match;

        if (!std::regex_match(line, match, re)) {
            continue;
        }

//        cout << "m.prefix.first [" << *match.prefix().first << "]" << endl;
//        cout << "m.prefix.second [" << *match.prefix().second << "]" << endl;
//        cout << "m.prefix.matched [" << match.prefix().matched << "]" << endl;
//        cout << "m.suffix.first [" << *match.suffix().first << "]" << endl;
//        cout << "m.suffix.second [" << *match.suffix().second << "]" << endl;
//        cout << "m.suffix.matched [" << match.suffix().matched << "]" << endl;
        const char* names[] = {"", "source", "event", "action", "target"};
        for (size_t i = 1; i < match.size(); ++i) {
            cout << "   m[" << i << "].positon [" << match.position(i) << "]" << endl;
//            cout << "   m[" << i << "].first [" << *match[i].first << "]" << endl;
//            cout << "   m[" << i << "].second [" << *match[i].second << "]" << endl;
            cout << "   m[" << i << "].length [" << match[i].length() << "]" << endl;
//            cout << "   m[" << i << "].matched [" << match[i].matched << "]" << endl;
            cout << "   m[" << i << "]: " << names[i] << ": [" << match[i] << "]" << endl;
        }
    }
}
