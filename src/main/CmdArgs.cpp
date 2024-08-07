#include "CmdArgs.h"

bool endsWith(const std::string& str, const std::string& suffix) {
    if (str.length() >= suffix.length()) {
        return (str.substr(str.length() - suffix.length()) == suffix);
    }
    return false;
}

CmdArgs::CmdArgs(int argc, char* argv[]) {
    for(int i = 1; i < argc; ++i) {
        auto str = std::string(argv[i]);
        if (std::string("dump").compare(argv[i]) == 0) {
            mode = Mode::DUMP;
        }
        if (std::string("run").compare(argv[i]) == 0) {
            mode = Mode::RUN;
        }
        if (std::string("build").compare(argv[i]) == 0) {
            mode = Mode::BUILD;
        }
        if (std::string("-debug").compare(argv[i]) == 0) {
            debug = true;
        }
        if (std::string("-O").compare(argv[i]) == 0) {
            optimize = true;
        }
        if (endsWith(str, ".wd")) {
            filename = str;
        }
    }
}
