/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include <string.h>
#include <unistd.h>
#include <chrono>  // NOLINT [build/c++11]
#include <cstdlib>
#include <iostream>
#include "main.h"
#include "application.hpp"
#include "rom.h"

int main() {
    std::cout << "The app has been started." << std::endl;
    romInit(0, 1);
    application_entry_point();
    return 0;
}

void uavcanReadUniqueID(uint8_t out_uid[4]) {
    memset(out_uid, 0x00, 16);
}

void uavcanRestartNode() {
    constexpr const char* EXECUTABLE_SYMBOLIC_LINK = "/proc/self/exe";
    constexpr const int MAX_PATH_LENGTH = 1024;

    std::cout << "Restarting application..." << std::endl;

    // Get the current executable path
    char executable_path[MAX_PATH_LENGTH];
    auto count = readlink(EXECUTABLE_SYMBOLIC_LINK, executable_path, sizeof(executable_path) - 1);
    if (count == -1 || count >= MAX_PATH_LENGTH) {
        std::cerr << "Error getting executable path." << std::endl;
        exit(1);
    }
    executable_path[count] = '\0';

    // Execute a new instance of the current application
    execl(executable_path, executable_path, nullptr);

    // If execl returns, an error occurred
    std::cerr << "Error restarting application." << std::endl;
    exit(1);
}

uint32_t uavcanGetTimeMs() {
    return HAL_GetTick();
}

void HAL_NVIC_SystemReset() {
    std::cout << "The app has been receved Restart command. Ignore." << std::endl;
}

uint32_t HAL_GetTick() {
    static auto start_time = std::chrono::high_resolution_clock::now();
    auto crnt_time = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(crnt_time - start_time).count();
}
