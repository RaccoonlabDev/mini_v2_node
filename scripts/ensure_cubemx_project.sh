#!/usr/bin/env bash
set -euo pipefail

usage() {
  cat <<'EOF'
Ensure a STM32CubeMX generated project exists.

Usage:
  ensure_cubemx_project.sh --name <archive-name-prefix> --ioc <ioc-file> --out <output-dir> --generator <script>

Behavior:
  1. Reuse <output-dir> if it has a matching .cubemx_ioc.sha256 stamp.
  2. Restore <name>-<ioc-sha256>.tar.gz from the local cache or download it.
  3. Otherwise, if STM32CubeMX is installed, generate the project locally.

Environment:
  CUBEMX_ARCHIVE_BASE_URL
      Default: https://github.com/AMEmbedded/firmware/releases/download/cubemx-v0.15.0
  CUBEMX_ARCHIVE_REPO
      Default: AMEmbedded/firmware
  CUBEMX_CACHE_TAG
      Default: cubemx-v0.15.0
  GITHUB_TOKEN
      Optional token for downloading assets from private repositories.
  STM32CUBEMX
      Optional explicit STM32CubeMX executable path.
EOF
}

NAME=""
IOC_FILE=""
OUTPUT_DIR=""
GENERATOR=""
CUBEMX_ARCHIVE_REPO="${CUBEMX_ARCHIVE_REPO:-AMEmbedded/firmware}"
CUBEMX_CACHE_TAG="${CUBEMX_CACHE_TAG:-cubemx-v0.15.0}"
CUBEMX_ARCHIVE_BASE_URL="${CUBEMX_ARCHIVE_BASE_URL:-https://github.com/AMEmbedded/firmware/releases/download/cubemx-v0.15.0}"
LOCAL_ARCHIVE_DIR="${CUBEMX_ARCHIVE_DIR_LOCAL:-$(realpath -m "$(dirname "$0")/../release/cubemx")}"

while [[ $# -gt 0 ]]; do
  case "$1" in
    --name)
      NAME="$2"
      shift 2
      ;;
    --ioc)
      IOC_FILE="$2"
      shift 2
      ;;
    --out)
      OUTPUT_DIR="$2"
      shift 2
      ;;
    --generator)
      GENERATOR="$2"
      shift 2
      ;;
    -h|--help)
      usage
      exit 0
      ;;
    *)
      echo "Unexpected argument: $1" >&2
      usage
      exit 1
      ;;
  esac
done

if [[ -z "${NAME}" || -z "${IOC_FILE}" || -z "${OUTPUT_DIR}" || -z "${GENERATOR}" ]]; then
  usage >&2
  exit 1
fi

IOC_FILE=$(realpath -m "${IOC_FILE}")
OUTPUT_DIR=$(realpath -m "${OUTPUT_DIR}")
GENERATOR=$(realpath -m "${GENERATOR}")
STAMP_FILE="${OUTPUT_DIR}/.cubemx_ioc.sha256"

if [[ ! -f "${IOC_FILE}" ]]; then
  echo "IOC file not found: ${IOC_FILE}" >&2
  exit 1
fi

if [[ ! -x "${GENERATOR}" ]]; then
  echo "CubeMX generator is not executable: ${GENERATOR}" >&2
  exit 1
fi

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
  echo "No SHA-256 tool found." >&2
  return 1
}

has_dir_content() {
  local dir_path="$1"
  [[ -d "${dir_path}" ]] && [[ -n "$(find "${dir_path}" -mindepth 1 -maxdepth 1 -print -quit)" ]]
}

stamp_matches() {
  [[ -f "${STAMP_FILE}" ]] && [[ "$(cat "${STAMP_FILE}")" == "${IOC_HASH}" ]]
}

find_cubemx() {
  local candidates=(
    "${STM32CUBEMX:-}"
    "/opt/stm32cubemx/STM32CubeMX"
    "/opt/st/STM32CubeMX/STM32CubeMX"
    "${HOME}/STM32CubeMX/STM32CubeMX"
    "/Applications/STMicroelectronics/STM32CubeMX.app/Contents/MacOs/STM32CubeMX"
  )

  local candidate
  for candidate in "${candidates[@]}"; do
    if [[ -n "${candidate}" && -x "${candidate}" ]]; then
      echo "${candidate}"
      return 0
    fi
  done

  if command -v STM32CubeMX >/dev/null 2>&1; then
    command -v STM32CubeMX
    return 0
  fi

  return 1
}

restore_local_archive() {
  local archive_name="${NAME}-${IOC_HASH}.tar.gz"
  if [[ ! -f "${LOCAL_ARCHIVE_DIR}/${archive_name}" ]]; then
    return 1
  fi

  echo "Restoring cached CubeMX project from local archive:"
  echo "  ${LOCAL_ARCHIVE_DIR}/${archive_name}"
  rm -rf "${OUTPUT_DIR}"
  mkdir -p "${OUTPUT_DIR}"
  tar -xzf "${LOCAL_ARCHIVE_DIR}/${archive_name}" -C "${OUTPUT_DIR}"
  if ! stamp_matches; then
    echo "Local CubeMX archive does not match IOC hash ${IOC_HASH}: ${archive_name}" >&2
    exit 1
  fi
  echo "Restored local cached CubeMX project: ${OUTPUT_DIR}"
}

download_archive() {
  local archive_name="${NAME}-${IOC_HASH}.tar.gz"
  local archive_url="${CUBEMX_ARCHIVE_BASE_URL%/}/${archive_name}"
  local tmp_dir
  tmp_dir=$(mktemp -d)
  trap 'rm -rf "${tmp_dir}"' RETURN

  echo "Downloading cached CubeMX project:"
  echo "  ${archive_url}"

  if [[ -n "${GITHUB_TOKEN:-}" ]] && command -v curl >/dev/null 2>&1; then
    download_private_github_asset "${archive_name}" "${tmp_dir}/${archive_name}"
  elif command -v curl >/dev/null 2>&1; then
    if ! curl -fL "${archive_url}" -o "${tmp_dir}/${archive_name}"; then
      echo "CubeMX archive is not available: ${archive_name}" >&2
      echo "Generate it locally with 'make cubemx_archives' and upload it to ${CUBEMX_ARCHIVE_BASE_URL}." >&2
      exit 1
    fi
  elif command -v wget >/dev/null 2>&1; then
    if ! wget -O "${tmp_dir}/${archive_name}" "${archive_url}"; then
      echo "CubeMX archive is not available: ${archive_name}" >&2
      echo "Generate it locally with 'make cubemx_archives' and upload it to ${CUBEMX_ARCHIVE_BASE_URL}." >&2
      exit 1
    fi
  else
    echo "Neither curl nor wget is available for downloading CubeMX archive." >&2
    exit 1
  fi

  rm -rf "${OUTPUT_DIR}"
  mkdir -p "${OUTPUT_DIR}"
  tar -xzf "${tmp_dir}/${archive_name}" -C "${OUTPUT_DIR}"

  if ! stamp_matches; then
    echo "Downloaded CubeMX archive does not match IOC hash ${IOC_HASH}: ${archive_name}" >&2
    exit 1
  fi

  if [[ ! -f "${OUTPUT_DIR}/Core/Src/main.c" ]]; then
    echo "Downloaded CubeMX archive is missing Core/Src/main.c: ${archive_name}" >&2
    exit 1
  fi

  echo "Restored cached CubeMX project: ${OUTPUT_DIR}"
}

download_private_github_asset() {
  local archive_name="$1"
  local output_path="$2"
  local api_url="https://api.github.com/repos/${CUBEMX_ARCHIVE_REPO}/releases/tags/${CUBEMX_CACHE_TAG}"
  local release_json asset_api_url

  echo "  using GitHub API auth for private release asset"

  release_json=$(curl -fsSL \
    -H "Authorization: Bearer ${GITHUB_TOKEN}" \
    -H "Accept: application/vnd.github+json" \
    -H "X-GitHub-Api-Version: 2022-11-28" \
    "${api_url}")

  asset_api_url=$(printf '%s' "${release_json}" | python3 -c '
import json
import sys

archive_name = sys.argv[1]
release = json.load(sys.stdin)
for asset in release.get("assets", []):
    if asset.get("name") == archive_name:
        print(asset["url"])
        break
' "${archive_name}")

  if [[ -z "${asset_api_url}" ]]; then
    echo "CubeMX archive is not attached to ${CUBEMX_ARCHIVE_REPO} release ${CUBEMX_CACHE_TAG}: ${archive_name}" >&2
    exit 1
  fi

  curl -fsSL \
    -H "Authorization: Bearer ${GITHUB_TOKEN}" \
    -H "Accept: application/octet-stream" \
    -H "X-GitHub-Api-Version: 2022-11-28" \
    "${asset_api_url}" \
    -o "${output_path}"
}

IOC_HASH=$(calc_file_hash "${IOC_FILE}")

if has_dir_content "${OUTPUT_DIR}" && stamp_matches; then
  echo "CubeMX project is up to date: ${OUTPUT_DIR}"
  exit 0
fi

if restore_local_archive; then
  exit 0
fi

if CUBEMX_BIN=$(find_cubemx); then
  echo "STM32CubeMX found: ${CUBEMX_BIN}"
  "${GENERATOR}" -i "${IOC_FILE}" -o "${OUTPUT_DIR}" -m "${CUBEMX_BIN}"
  exit 0
fi

if download_archive; then
  exit 0
fi

echo "Neither a cached CubeMX project nor STM32CubeMX is available for ${NAME}." >&2
echo "Generate it locally with 'make cubemx_archives' and add the archive to ${LOCAL_ARCHIVE_DIR} or ${CUBEMX_ARCHIVE_BASE_URL}." >&2
exit 1
