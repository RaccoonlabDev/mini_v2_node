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
  --build-dir <path>         Build root directory. Default: <node-core>/build
  --build-variant <value>    Override build subdirectory name
  --cmake-build-type <value> Forward CMAKE_BUILD_TYPE to CMake
  --help                     Show this help
EOF
}

NC_BOARD=""
NC_TARGET=""
NC_IMAGE_KIND="standalone"
NC_APP_SRC_DIR=""
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
        -DCMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}" \
        "${CMAKE_EXTRA_ARGS[@]}" \
        -G "Unix Makefiles"
else
    echo "Configuration is up to date: ${OBJ_DIR}"
fi

exec make -C "${OBJ_DIR}"
