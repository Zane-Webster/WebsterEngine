#ifndef WE_CORE_LOGGER_H_
#define WE_CORE_LOGGER_H_

#include <iostream>
#include <sstream>
#include <string>

#include "core/WebsterEngine.h"
#include "utils/GLMStream.h"

class Logger {
public:
    template<typename T>
    static void Debug(const T& value) { Log("[DEBUG] ", value); }
    template<typename T>
    static void Info(const T& value) { Log("[INFO] ", value); }
    template<typename T>
    static void Warn(const T& value) { Log("[WARN] ", value); }
    template<typename T>
    static void Error(const T& value) { Log("[ERROR] ", value); }

    static void LoadState(const std::string function, const WE::LOAD_STATE state);

private:
    template<typename T>
    static void Log(const char* prefix, const T& value)
    {
        std::cout << prefix << value << std::endl;
    }

};

#endif // WE_CORE_LOGGER_H_