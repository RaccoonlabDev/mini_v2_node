#!/usr/bin/env bash
set -euo pipefail

DEFAULT_LOCAL_IOC_PATH="Src/boards/rl/mini_v3/board/bootloader.ioc"

usage() {
  cat <<'EOF'
Generate STM32CubeMX HAL sources from an .ioc file (CLI/headless).

Usage:
  ./generate_cubemx_hal.sh [options]

Options:
  -i, --ioc <ioc_file>
                      Path to .ioc file.
                      Default: Src/boards/rl/mini_v3/board/bootloader.ioc
  -o, --out <output_dir>
                      Output directory for generated project.
                      Default: build/<ioc-name>
  -m, --cubemx <path> Path to STM32CubeMX executable.
                      Default: auto-detect common install paths
  -t, --toolchain <name>
                      Optional CubeMX toolchain name (e.g. STM32CubeIDE)
  -f, --force         Force generation even if output exists or .ioc hash is unchanged
  -v, --verbose       Verbose mode
  -h, --help          Show this help

Examples:
  ./generate_cubemx_hal.sh
  ./generate_cubemx_hal.sh -i Src/boards/rl/mini_v3/board/bootloader.ioc -o build/stm32cubemx/rl/mini_v3/bootloader
  ./generate_cubemx_hal.sh --force -t STM32CubeIDE
  ./generate_cubemx_hal.sh -m /opt/stm32cubemx/STM32CubeMX -t STM32CubeIDE
EOF
}

SCRIPT_NAME=$(basename "$0")
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
REPO_ROOT="$(dirname "$SCRIPT_DIR")"
IOC_FILE="${REPO_ROOT}/${DEFAULT_LOCAL_IOC_PATH}"
OUTPUT_DIR=""
CUBEMX_BIN=""
TOOLCHAIN=""
VERBOSE=0
CUBEMX_RUNNER=""
FORCE=0

find_cubemx() {
  local candidates=(
    "${STM32CUBEMX:-}"
    "/opt/stm32cubemx/STM32CubeMX"
    "/opt/st/STM32CubeMX/STM32CubeMX"
    "${HOME}/STM32CubeMX/STM32CubeMX"
    "/Applications/STMicroelectronics/STM32CubeMX.app/Contents/MacOs/STM32CubeMX"
  )

  local c
  for c in "${candidates[@]}"; do
    if [[ -n "${c}" && -x "${c}" ]]; then
      echo "${c}"
      return 0
    fi
  done

  return 1
}

resolve_cubemx_runner() {
  local input_path="$1"
  local candidate=""

  if [[ -d "${input_path}" ]]; then
    local in_dir=(
      "${input_path}/STM32CubeMX"
      "${input_path}/stm32cubemx"
      "${input_path}/STM32CubeMX.linux"
    )
    local p
    for p in "${in_dir[@]}"; do
      if [[ -x "${p}" ]]; then
        echo "exec:${p}"
        return 0
      fi
    done
    return 1
  fi

  if [[ -x "${input_path}" ]]; then
    echo "exec:${input_path}"
    return 0
  fi

  if [[ -f "${input_path}" && "${input_path}" == *.jar ]]; then
    if command -v java >/dev/null 2>&1; then
      echo "jar:${input_path}"
      return 0
    fi
    echo "${SCRIPT_NAME}: '${input_path}' looks like a jar, but java is not installed." >&2
    return 1
  fi

  return 1
}

has_dir_content() {
  local dir_path="$1"
  if [[ ! -d "${dir_path}" ]]; then
    return 1
  fi
  if find "${dir_path}" -mindepth 1 -maxdepth 1 | read -r _; then
    return 0
  fi
  return 1
}

calc_file_hash() {
  local file_path="$1"
  if command -v sha256sum >/dev/null 2>&1; then
    sha256sum "${file_path}" | awk '{print $1}'
    return 0
  fi
  if command -v shasum >/dev/null 2>&1; then
    shasum -a 256 "${file_path}" | awk '{print $1}'
    return 0
  fi
  return 1
}

find_main_c_file() {
  local base_dir="$1"
  local direct="${base_dir}/Core/Src/main.c"
  if [[ -f "${direct}" ]]; then
    echo "${direct}"
    return 0
  fi

  local found
  found=$(find "${base_dir}" -maxdepth 5 -type f -path "*/Core/Src/main.c" | head -n 1 || true)
  if [[ -n "${found}" ]]; then
    echo "${found}"
    return 0
  fi
  return 1
}

insert_line_between_markers() {
  local file_path="$1"
  local begin_marker="$2"
  local end_marker="$3"
  local insert_line="$4"

  local tmp_file
  tmp_file=$(mktemp)

  if ! awk -v begin="${begin_marker}" -v end="${end_marker}" -v ins="${insert_line}" '
    BEGIN { in_block=0; found_block=0; present=0; }
    {
      line = $0;
      sub(/\r$/, "", line);

      if (line == begin) {
        in_block=1;
        found_block=1;
        present=0;
        print $0;
        next;
      }
      if (in_block == 1) {
        if (line == ins) {
          present=1;
        }
        if (line == end) {
          if (present == 0) {
            if ($0 ~ /\r$/) {
              print ins "\r";
            } else {
              print ins;
            }
          }
          print $0;
          in_block=0;
          next;
        }
      }
      print $0;
    }
    END {
      if (found_block == 0) {
        exit 2;
      }
      if (in_block == 1) {
        exit 3;
      }
    }
  ' "${file_path}" > "${tmp_file}"; then
    rm -f "${tmp_file}"
    return 1
  fi

  mv "${tmp_file}" "${file_path}"
  return 0
}

patch_generated_main_c() {
  local output_dir="$1"
  local main_c_path=""
  if ! main_c_path=$(find_main_c_file "${output_dir}"); then
    echo "${SCRIPT_NAME}: generated main.c not found under: ${output_dir}" >&2
    return 1
  fi

  if ! insert_line_between_markers \
    "${main_c_path}" \
    "/* USER CODE BEGIN Includes */" \
    "/* USER CODE END Includes */" \
    "#include \"application.hpp\""; then
    echo "${SCRIPT_NAME}: failed to insert include into ${main_c_path}" >&2
    return 1
  fi

  if ! insert_line_between_markers \
    "${main_c_path}" \
    "  /* USER CODE BEGIN 2 */" \
    "  /* USER CODE END 2 */" \
    "  application_entry_point();"; then
    echo "${SCRIPT_NAME}: failed to insert application entry call into ${main_c_path}" >&2
    return 1
  fi

  if [[ ${VERBOSE} -eq 1 ]]; then
    echo "Patched generated main.c: ${main_c_path}"
  fi
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    -i|--ioc)
      [[ $# -ge 2 ]] || { echo "${SCRIPT_NAME}: option '$1' requires an argument" >&2; usage; exit 1; }
      IOC_FILE="$2"
      shift 2
      ;;
    -o|--out)
      [[ $# -ge 2 ]] || { echo "${SCRIPT_NAME}: option '$1' requires an argument" >&2; usage; exit 1; }
      OUTPUT_DIR="$2"
      shift 2
      ;;
    -m|--cubemx)
      [[ $# -ge 2 ]] || { echo "${SCRIPT_NAME}: option '$1' requires an argument" >&2; usage; exit 1; }
      CUBEMX_BIN="$2"
      shift 2
      ;;
    -t|--toolchain)
      [[ $# -ge 2 ]] || { echo "${SCRIPT_NAME}: option '$1' requires an argument" >&2; usage; exit 1; }
      TOOLCHAIN="$2"
      shift 2
      ;;
    -f|--force)
      FORCE=1
      shift
      ;;
    -v|--verbose)
      VERBOSE=1
      shift
      ;;
    -h|--help)
      usage
      exit 0
      ;;
    --)
      shift
      break
      ;;
    -*)
      echo "${SCRIPT_NAME}: invalid option '$1'" >&2
      usage
      exit 1
      ;;
    *)
      echo "${SCRIPT_NAME}: unexpected argument: $1" >&2
      usage
      exit 1
      ;;
  esac
done

if [[ $# -gt 0 ]]; then
  echo "${SCRIPT_NAME}: unexpected argument(s): $*" >&2
  usage
  exit 1
fi

IOC_FILE=$(realpath -m "${IOC_FILE}")
if [[ ! -f "${IOC_FILE}" ]]; then
  echo "${SCRIPT_NAME}: .ioc file not found: ${IOC_FILE}" >&2
  exit 1
fi

if [[ -z "${OUTPUT_DIR}" ]]; then
  IOC_NAME=$(basename "${IOC_FILE}" .ioc)
  OUTPUT_DIR="${REPO_ROOT}/build/${IOC_NAME}"
fi
OUTPUT_DIR=$(realpath -m "${OUTPUT_DIR}")
mkdir -p "${OUTPUT_DIR}"

IOC_HASH=""
STAMP_FILE="${OUTPUT_DIR}/.cubemx_ioc.sha256"
if IOC_HASH=$(calc_file_hash "${IOC_FILE}"); then
  :
fi

if [[ ${FORCE} -eq 0 ]] && has_dir_content "${OUTPUT_DIR}"; then
  if [[ -n "${IOC_HASH}" && -f "${STAMP_FILE}" ]]; then
    PREV_HASH=$(cat "${STAMP_FILE}" 2>/dev/null || true)
    if [[ "${PREV_HASH}" == "${IOC_HASH}" ]]; then
      patch_generated_main_c "${OUTPUT_DIR}"
      echo "No IOC changes detected. Skipping generation: ${OUTPUT_DIR}"
      exit 0
    fi
  else
    patch_generated_main_c "${OUTPUT_DIR}"
    echo "Output directory is not empty. Skipping generation: ${OUTPUT_DIR}"
    echo "Use --force to regenerate."
    exit 0
  fi
fi

if [[ -z "${CUBEMX_BIN}" ]]; then
  if ! CUBEMX_BIN=$(find_cubemx); then
    echo "${SCRIPT_NAME}: STM32CubeMX executable not found." >&2
    echo "Set STM32CUBEMX env var or pass -m <path>." >&2
    exit 1
  fi
fi

if ! CUBEMX_RUNNER=$(resolve_cubemx_runner "${CUBEMX_BIN}"); then
  echo "${SCRIPT_NAME}: STM32CubeMX path is invalid or not executable: ${CUBEMX_BIN}" >&2
  if [[ -e "${CUBEMX_BIN}" ]]; then
    ls -ld "${CUBEMX_BIN}" >&2 || true
  fi
  if alt_cubemx_bin=$(find_cubemx); then
    echo "Detected STM32CubeMX candidate: ${alt_cubemx_bin}" >&2
    echo "Try: ./generate_cubemx_hal.sh -i \"${IOC_FILE}\" -o \"${OUTPUT_DIR}\" -m \"${alt_cubemx_bin}\" ${TOOLCHAIN:+-t \"${TOOLCHAIN}\"} -v" >&2
  fi
  echo "Hint: pass an executable binary path, CubeMX install directory, or a .jar path." >&2
  exit 1
fi

MX_SCRIPT=$(mktemp)
trap 'rm -f "${MX_SCRIPT}"' EXIT

{
  echo "config load \"${IOC_FILE}\""
  if [[ -n "${TOOLCHAIN}" ]]; then
    echo "project toolchain \"${TOOLCHAIN}\""
  fi
  echo "project path \"${OUTPUT_DIR}\""
  echo "project generate"
  echo "exit"
} >"${MX_SCRIPT}"

if [[ ${VERBOSE} -eq 1 ]]; then
  echo "STM32CubeMX path: ${CUBEMX_BIN}"
  echo "STM32CubeMX runner: ${CUBEMX_RUNNER}"
  echo "IOC file: ${IOC_FILE}"
  echo "IOC sha256: ${IOC_HASH:-<unavailable>}"
  echo "Output dir: ${OUTPUT_DIR}"
  echo "Toolchain: ${TOOLCHAIN:-<from .ioc/default>}"
  echo "Force: ${FORCE}"
  echo "CubeMX script:"
  sed 's/^/  /' "${MX_SCRIPT}"
fi

if [[ "${CUBEMX_RUNNER}" == exec:* ]]; then
  "${CUBEMX_RUNNER#exec:}" -q "${MX_SCRIPT}"
elif [[ "${CUBEMX_RUNNER}" == jar:* ]]; then
  java -jar "${CUBEMX_RUNNER#jar:}" -q "${MX_SCRIPT}"
else
  echo "${SCRIPT_NAME}: internal error: unsupported runner '${CUBEMX_RUNNER}'" >&2
  exit 1
fi

patch_generated_main_c "${OUTPUT_DIR}"

if [[ -n "${IOC_HASH}" ]]; then
  echo "${IOC_HASH}" > "${STAMP_FILE}"
fi
echo "CubeMX generation finished: ${OUTPUT_DIR}"
