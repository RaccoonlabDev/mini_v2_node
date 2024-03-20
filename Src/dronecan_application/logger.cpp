/*
* Copyright (C) 2024 Anastasiia Stepanova <asiiapine@gmail.com>
* Distributed under the terms of the GPL v3 license, available in the file LICENSE.
*/

#include "logger.hpp"
Logger::Logger(const char* source) {
    set_source(this->_msg, source);
}

int8_t Logger::init(const char* source = "") {
    return set_source(this->_msg, source);
}

void Logger::log_info(const char* text) {
    set_text(this->_msg, text);
    _msg.level = LOG_LEVEL_INFO;
    publish();
}
void Logger::log_warn(const char* text) {
    set_text(this->_msg, text);
    _msg.level = LOG_LEVEL_WARNING;
    publish();
}
void Logger::log_error(const char* text) {
    set_text(this->_msg, text);
    _msg.level = LOG_LEVEL_ERROR;
    publish();
}

void Logger::publish() {
    dronecan_protocol_debug_log_message_publish(&this->_msg, &_transfer_id);
    _transfer_id++;
}

#if NDEBUG
void Logger::log_debug(const char* text) {
    // Do nothing if _DEBUG is not defined
}
#else
void Logger::log_debug(const char* text) {
    set_text(this->_msg, text);
    _msg.level = LOG_LEVEL_DEBUG;
    publish();
}
#endif
