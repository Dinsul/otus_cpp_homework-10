#include "mtbulk.h"
#include <fstream>
#include <iostream>

int main (int argc, char** argv)
{
    if (argc != 2)
    {
        std::cout << "usage: bulk N\n\tN - number of commands in bulk." << std::endl;

        return 1;
    }

    size_t commandsCount   = static_cast<size_t>(atoi(argv[1]));
    size_t linesCounter    = 0;
    size_t commandsCounter = 0;

    MTWorker wrk;

    BulkController ctrl(commandsCount, wrk);

    std::string cmd;
    cmd.clear();

    for(std::string line; std::getline(std::cin, line);)
    {
        ctrl.addString(line);

        ++linesCounter;

        if (line != ctrl.signShiftUp() && line != ctrl.signShiftDown())
        {
            ++commandsCounter;
        }
    }

//    ctrl.waiteWorker();

    return 0;
}
