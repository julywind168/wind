#ifndef WIND_CMD_ARGS_H
#define WIND_CMD_ARGS_H

#include <string>


enum class Mode {
    DUMP,
    RUN,
    BUILD
};

class CmdArgs {
public:
    Mode mode = Mode::DUMP;
    bool optimize = false;
    bool debug = false;
    std::string filename = "main.wd";

    CmdArgs(int argc, char* argv[]);
};

#endif
