/// This software is distributed under the terms of the MIT License.
/// Copyright (c) 2022 Dmitry Ponomarev.
/// Author: Dmitry Ponomarev <ponomarevda96@gmail.com>

#include <cstdint>
#include <array>

#define VCS_REVISION_ID 12345
#define FW_VERSION_MAJOR 10
#define FW_VERSION_MINOR 11
#define DIRTY_BUILD 1
#define BUILD_TIMESTAMP_UTC 1234567


alignas(16) struct AppDescriptor
{
    std::uint64_t               magic = 0x5E44'1514'6FC0'C4C7ULL;
    std::array<std::uint8_t, 8> signature{{'A', 'P', 'D', 'e', 's', 'c', '0', '0'}};

    std::uint64_t                               image_crc  = 0;     // Populated after build
    std::uint32_t                               image_size = 0;     // Populated after build
    [[maybe_unused]] std::array<std::byte, 4>   _reserved_a{};
    std::uint8_t                                version_major = 1;
    std::uint8_t                                version_minor = 1;
    std::uint8_t                                flags =
#if RELEASE_BUILD
        Flags::ReleaseBuild;
#else
        0;
#endif
    [[maybe_unused]] std::array<std::byte, 1>   _reserved_b{};
    std::uint32_t                               build_timestamp_utc = 0;
    std::uint64_t                               vcs_revision_id     = 0;
    [[maybe_unused]] std::array<std::byte, 16>  _reserved_c{};

    struct Flags
    {
        static constexpr std::uint8_t ReleaseBuild = 1U;
        static constexpr std::uint8_t DirtyBuild   = 2U;
    };
};
static const volatile AppDescriptor _app_descriptor
    __attribute__((used, section(".app_descriptor")));
