#!/usr/bin/env bash
set -euo pipefail

usage() {
  cat <<'EOF'
Package one STM32CubeMX generated project as a deterministic cache archive.

Usage:
  package_cubemx_archive.sh --name <archive-name-prefix> --ioc <ioc-file> --out <output-dir> --generator <script> --archive-dir <dir>
EOF
}

NAME=""
IOC_FILE=""
OUTPUT_DIR=""
GENERATOR=""
ARCHIVE_DIR=""

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
    --archive-dir)
      ARCHIVE_DIR="$2"
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

if [[ -z "${NAME}" || -z "${IOC_FILE}" || -z "${OUTPUT_DIR}" || -z "${GENERATOR}" || -z "${ARCHIVE_DIR}" ]]; then
  usage >&2
  exit 1
fi

IOC_FILE=$(realpath -m "${IOC_FILE}")
OUTPUT_DIR=$(realpath -m "${OUTPUT_DIR}")
GENERATOR=$(realpath -m "${GENERATOR}")
ARCHIVE_DIR=$(realpath -m "${ARCHIVE_DIR}")

if command -v sha256sum >/dev/null 2>&1; then
  IOC_HASH=$(sha256sum "${IOC_FILE}" | awk '{print $1}')
elif command -v shasum >/dev/null 2>&1; then
  IOC_HASH=$(shasum -a 256 "${IOC_FILE}" | awk '{print $1}')
else
  echo "No SHA-256 tool found." >&2
  exit 1
fi

"${GENERATOR}" -i "${IOC_FILE}" -o "${OUTPUT_DIR}"

if [[ ! -f "${OUTPUT_DIR}/.cubemx_ioc.sha256" ]]; then
  echo "Generated project is missing .cubemx_ioc.sha256: ${OUTPUT_DIR}" >&2
  exit 1
fi

if [[ "$(cat "${OUTPUT_DIR}/.cubemx_ioc.sha256")" != "${IOC_HASH}" ]]; then
  echo "Generated project stamp does not match IOC hash: ${OUTPUT_DIR}" >&2
  exit 1
fi

mkdir -p "${ARCHIVE_DIR}"
ARCHIVE_PATH="${ARCHIVE_DIR}/${NAME}-${IOC_HASH}.tar.gz"
tar -C "${OUTPUT_DIR}" -czf "${ARCHIVE_PATH}" .

echo "${ARCHIVE_PATH}"
