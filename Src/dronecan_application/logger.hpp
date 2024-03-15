/*
* Copyright (C) 2023 Anastasiia Stepanova <asiiapine@gmail.com>
* Distributed under the terms of the GPL v3 license, available in the file LICENSE.
*/

#ifndef SRC_LOGGER_HPP_
#define SRC_LOGGER_HPP_

#include "uavcan/protocol/debug/LogMessage.h"
#include "logging.h"
#include "main.h"
#include <cstdio> 

class Logger {
public:
    // Logger();
    Logger(const char* source);
    /**
     * @brief Optionally init source and len fields if the source is provided
     * It can be a brief task/module name or something that can't be changed later.
     */
    
    int8_t init(const char* source);

    /**
     * @brief Logging with a different severity.
     * Debug: use only if we compile with DEBUG build type (NDEBUG is not defined) , otherwise ignore.
     * Info/Warn: something that can be ignored or printed by the application severity level.
     * Error: always print.
     */
    void log_debug(const char* text);
    void log_info(const char* text);
    void log_warn(const char* text);
    void log_error(const char* text);
private:
    uint8_t _transfer_id;
    DebugLogMessage_t _msg;
    void publish();
};

#endif  // SRC_LOGGER_HPP_
