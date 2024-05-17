/***
 * Copyright (C) 2024 Anastasiia Stepanova  <asiiapine96@gmail.com>
 *  Distributed under the terms of the GPL v3 license, available in the file LICENSE.
***/ 

#ifndef SRC_LOGGING_H_
#define SRC_LOGGING_H_

#include "uavcan/protocol/debug/LogMessage.h"

static inline bool set_source(DebugLogMessage_t &msg, const char *str) {
    if (str == nullptr){
        return false;
    }

    msg.source_size = strnlen(str, 31);
    msg.source[30] = 0;
    memcpy((char*) msg.source, str,  msg.source_size);
    return true;
}

static inline bool set_text(DebugLogMessage_t &msg, const char *str) {
    if (str == nullptr){
        return false;
    }

    msg.text_size = strnlen(str, 90);
    memcpy((char*) msg.text, str,  msg.text_size);
    return true;
}

#endif // SRC_LOGGING_H_
