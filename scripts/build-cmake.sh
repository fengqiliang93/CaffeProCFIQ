#!/usr/bin/env sh
set -eu

repo_root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
arch=${ARCH:-${1:-x86_64}}
preset=
build_preset=

usage() {
    cat <<'USAGE'
Usage:
  scripts/build-cmake.sh [x86_64|aarch64]

Environment:
  ARCH        Target architecture. Default: x86_64
  STAGE       Set to 1 to build the stage_runtime target. Default: 1
  SYSROOT     Optional aarch64 sysroot used by cmake/toolchains/aarch64-linux-gnu.cmake.
USAGE
}

case "$arch" in
    -h|--help)
        usage
        exit 0
        ;;
    x86_64)
        preset=x86_64-linux
        ;;
    aarch64|arm64)
        preset=aarch64-linux
        ;;
    *)
        echo "unsupported ARCH: $arch" >&2
        usage >&2
        exit 2
        ;;
esac

if [ "${STAGE:-1}" = "1" ]; then
    build_preset="${preset}-stage"
else
    build_preset="$preset"
fi

run_cmake() {
    cmake --preset "$preset"
    cmake --build --preset "$build_preset"
}

if [ -z "${WSL_DISTRO_NAME:-}" ] && command -v wsl.exe >/dev/null 2>&1; then
    repo_root_wsl=$(wsl.exe -e wslpath -a "$repo_root" | tr -d '\r')
    wsl.exe -e bash -lc "set -eu; cd '$repo_root_wsl'; cmake --preset '$preset'; cmake --build --preset '$build_preset'"
elif command -v cmake >/dev/null 2>&1; then
    cd "$repo_root"
    run_cmake
else
    echo "missing cmake" >&2
    exit 1
fi
