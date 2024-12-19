/**
 * This program is free software under the GNU General Public License v3.
 * See <https://www.gnu.org/licenses/> for details.
 * Author: Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "main.hpp"
#include "application.hpp"
// #include "common/algorithms.hpp"
// #include "params.hpp"

REGISTER_MODULE(BootloaderModule)

void BootloaderModule::init() {
}

void BootloaderModule::spin_once() {
    bootloader_entry_point();
}
