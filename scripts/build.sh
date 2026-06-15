#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"

usage() {
    cat <<'EOF'
Usage:
  build.sh --board <vendor/board> --target <target> [options] [-- <cmake args...>]

Options:
  --board <value>            Logical board identifier, e.g. rl/mini_v2
  --target <value>           Board target name, e.g. dronecan
  --image-kind <value>       Firmware image kind. Default: standalone
  --app-src-dir <path>       Optional app source directory with boards/modules/drivers
  --icd-doc <path>           Optional ICD markdown output path
  --build-dir <path>         Build root directory. Default: <node-core>/build
  --build-variant <value>    Override build subdirectory name
  --cmake-build-type <value> Forward CMAKE_BUILD_TYPE to CMake
  --help                     Show this help

Environment:
  NC_GENERATE_CUBEMX_HAL=1   Check/regenerate STM32CubeMX HAL before configuring CMake.
                             Missing generated HAL files are regenerated automatically.
  STM32CUBEMX=<path>         Optional STM32CubeMX executable for generation.
  CUBEMX_TOOLCHAIN=<name>    Optional CubeMX toolchain name.
  CUBEMX_FORCE=1             Force CubeMX generation even if .ioc hash matches.
  CUBEMX_VERBOSE=1           Print CubeMX generation details.
EOF
}

NC_BOARD=""
NC_TARGET=""
NC_IMAGE_KIND="standalone"
NC_APP_SRC_DIR=""
NC_DOCS_ICD_PATH=""
NC_BUILD_DIR="${ROOT_DIR}/build"
BUILD_VARIANT=""
CMAKE_BUILD_TYPE=""
CMAKE_EXTRA_ARGS=()

while [[ $# -gt 0 ]]; do
    case "$1" in
        --board)
            NC_BOARD="$2"
            shift 2
            ;;
        --target)
            NC_TARGET="$2"
            shift 2
            ;;
        --image-kind)
            NC_IMAGE_KIND="$2"
            shift 2
            ;;
        --app-src-dir)
            NC_APP_SRC_DIR="$2"
            shift 2
            ;;
        --icd-doc)
            NC_DOCS_ICD_PATH="$2"
            shift 2
            ;;
        --build-dir)
            NC_BUILD_DIR="$2"
            shift 2
            ;;
        --build-variant)
            BUILD_VARIANT="$2"
            shift 2
            ;;
        --cmake-build-type)
            CMAKE_BUILD_TYPE="$2"
            shift 2
            ;;
        --help)
            usage
            exit 0
            ;;
        --)
            shift
            CMAKE_EXTRA_ARGS=("$@")
            break
            ;;
        *)
            echo "Error: unknown argument '$1'." >&2
            usage >&2
            exit 2
            ;;
    esac
done

if [[ -z "${NC_BOARD}" ]]; then
    echo "Error: --board is required." >&2
    usage >&2
    exit 2
fi

if [[ -z "${NC_TARGET}" ]]; then
    echo "Error: --target is required." >&2
    usage >&2
    exit 2
fi

if [[ -z "${BUILD_VARIANT}" ]]; then
    BUILD_VARIANT="$(printf '%s_%s_%s' "${NC_BOARD}" "${NC_TARGET}" "${NC_IMAGE_KIND}" | tr '/' '_')"
fi

OBJ_DIR="${NC_BUILD_DIR}/${BUILD_VARIANT}/obj"

generate_cubemx_hal_if_requested() {
    local ioc_file=""
    local output_dir=""
    local required_files=()
    case "${NC_BOARD}" in
        rl/mini_v2)
            ioc_file="${ROOT_DIR}/Libs/stm32-cube-project/project_v2.ioc"
            output_dir="${ROOT_DIR}/Libs/stm32-cube-project"
            required_files=(
                "${output_dir}/Core/Inc/main.h"
                "${output_dir}/Core/Src/main.c"
                "${output_dir}/Drivers"
            )
            ;;
        rl/mini_v3)
            ioc_file="${ROOT_DIR}/Libs/mini-v3-ioc/project_v3.ioc"
            output_dir="${ROOT_DIR}/Libs/mini-v3-ioc"
            required_files=(
                "${output_dir}/Core/Inc/main.h"
                "${output_dir}/Core/Src/main.c"
                "${output_dir}/Drivers"
            )
            ;;
        rl/node_v4)
            ioc_file="${ROOT_DIR}/Libs/node-v4-h7-ioc/STM32H753IIK6-V4.ioc"
            output_dir="${ROOT_DIR}/Libs/node-v4-h7-ioc"
            required_files=(
                "${output_dir}/Core/Inc/main.h"
                "${output_dir}/Core/Src/main.c"
                "${output_dir}/Core/Src/fdcan.c"
                "${output_dir}/Drivers"
            )
            ;;
        *)
            if [[ "${NC_GENERATE_CUBEMX_HAL:-0}" == "1" ]]; then
                echo "CubeMX HAL generation is not configured for board '${NC_BOARD}'." >&2
                exit 2
            fi
            return 0
            ;;
    esac

    local generation_needed=0
    if [[ "${NC_GENERATE_CUBEMX_HAL:-0}" == "1" ]]; then
        generation_needed=1
    else
        local required_file
        for required_file in "${required_files[@]}"; do
            if [[ ! -e "${required_file}" ]]; then
                generation_needed=1
                break
            fi
        done
    fi

    if [[ "${generation_needed}" -eq 0 ]]; then
        return 0
    fi

    local args=(
        "${ROOT_DIR}/scripts/generate_cubemx_hal.sh"
        --ioc "${ioc_file}"
        --out "${output_dir}"
    )
    if [[ -n "${STM32CUBEMX:-}" ]]; then
        args+=(--cubemx "${STM32CUBEMX}")
    fi
    if [[ -n "${CUBEMX_TOOLCHAIN:-}" ]]; then
        args+=(--toolchain "${CUBEMX_TOOLCHAIN}")
    fi
    if [[ "${CUBEMX_FORCE:-0}" == "1" ]]; then
        args+=(--force)
    fi
    if [[ "${CUBEMX_VERBOSE:-0}" == "1" ]]; then
        args+=(--verbose)
    fi

    echo "Generating STM32CubeMX HAL for ${NC_BOARD}"
    "${args[@]}"
}

generate_cubemx_hal_if_requested

mkdir -p "${OBJ_DIR}"

need_config=0
if [[ ! -f "${OBJ_DIR}/CMakeCache.txt" ]]; then
    need_config=1
elif [[ ! -f "${OBJ_DIR}/Makefile" ]]; then
    need_config=1
elif ! grep -q "^NC_BOARD:STRING=${NC_BOARD}$" "${OBJ_DIR}/CMakeCache.txt"; then
    need_config=1
elif ! grep -q "^NC_TARGET:STRING=${NC_TARGET}$" "${OBJ_DIR}/CMakeCache.txt"; then
    need_config=1
elif ! grep -q "^NC_IMAGE_KIND:STRING=${NC_IMAGE_KIND}$" "${OBJ_DIR}/CMakeCache.txt"; then
    need_config=1
elif ! grep -q "^NC_APP_SRC_DIR:PATH=${NC_APP_SRC_DIR}$" "${OBJ_DIR}/CMakeCache.txt"; then
    need_config=1
elif ! grep -q "^NC_DOCS_ICD_PATH:FILEPATH=${NC_DOCS_ICD_PATH}$" "${OBJ_DIR}/CMakeCache.txt"; then
    need_config=1
elif [[ "${#CMAKE_EXTRA_ARGS[@]}" -gt 0 ]]; then
    need_config=1
fi

if [[ "${need_config}" -eq 1 ]]; then
    echo "Configuring ${OBJ_DIR}"
    cmake \
        -S "${ROOT_DIR}" \
        -B "${OBJ_DIR}" \
        -DNC_BOARD="${NC_BOARD}" \
        -DNC_TARGET="${NC_TARGET}" \
        -DNC_IMAGE_KIND="${NC_IMAGE_KIND}" \
        -DNC_APP_SRC_DIR="${NC_APP_SRC_DIR}" \
        -DNC_DOCS_ICD_PATH="${NC_DOCS_ICD_PATH}" \
        -DCMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}" \
        "${CMAKE_EXTRA_ARGS[@]}" \
        -G "Unix Makefiles"
else
    echo "Configuration is up to date: ${OBJ_DIR}"
fi

exec make -C "${OBJ_DIR}"
