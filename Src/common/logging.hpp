/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 * Author: Anastasiia Stepanova <asiiapine@gmail.com>
 */

#ifndef SRC_COMMON_LOGGING_HPP_
#define SRC_COMMON_LOGGING_HPP_

#include <stdint.h>

class Logging {
public:
    explicit Logging(const char* source_);

    void log_debug(const char* text) const;
    void log_info(const char* text) const;
    void log_warn(const char* text) const;
    void log_error(const char* text) const;

    void log(uint8_t severity, const char* text) const;
private:
    const char* source{nullptr};
};

#endif  // SRC_COMMON_LOGGING_HPP_
