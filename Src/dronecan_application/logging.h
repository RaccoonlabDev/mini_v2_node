#ifndef SRC_LOGGING_H_
#define SRC_LOGGING_H_


#include "uavcan/protocol/debug/LogMessage.h"
#include <stdarg.h>


static inline bool set_source(DebugLogMessage_t &msg, const char *str) {
    if (str == nullptr){
        return false;
    }

    size_t str_len = strlen(str);
    if (strlen(str) > 31) {
        return false;
    }

    strcpy((char*) msg.source, str);
    msg.source_size = str_len;
    return true;
}

static inline bool set_text(DebugLogMessage_t &msg, const char *str) {
    if (str == nullptr){
        return false;
    }
    size_t str_len = strlen(str);

    if (str_len > 90) {
    return false;
    }

    // TODO: test if char and unsigned char are casted correctly
    strcpy((char*) msg.text, str);
    msg.text_size = str_len;
    return true;
}
#endif // SRC_LOGGING_H_
