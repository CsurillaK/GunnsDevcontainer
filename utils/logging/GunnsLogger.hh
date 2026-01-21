#pragma once

#include "simulation/hs/TsHsMngr.hh"
#include "simulation/hs/TsHsMsg.hh"

#include <cstdlib>
#include <iostream>

class GunnsLogger : public TsHsMngr
{
public:
    GunnsLogger() {}

    ~GunnsLogger()
    {
        shutdown();
    }

    void msg(const std::string &file, const int line, const std::string &function,
             TS_HS_MSG_TYPE type, [[maybe_unused]] const std::string &subsys, const std::string &mtext)
    {
        std::ostringstream message;
        message << file << ":" << line << " ";
        if (function.length() > 0)
        {
            message << function << "() ";
        }
        message << mtext << std::endl;

        if (type >= TS_HS_MSG_TYPE::TS_HS_ERROR)
        {
            std::cerr << message.str() << std::endl;
            exit(EXIT_FAILURE);
        }
        else
        {
            std::cout << message.str() << std::endl;
        }
    }

    void restart() {}

    void shutdown()
    {
        tsGlobalHsMngr = nullptr;
    }

    void update() {}
};
