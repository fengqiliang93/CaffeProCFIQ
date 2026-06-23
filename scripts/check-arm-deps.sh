#!/usr/bin/env sh
set -eu

usage() {
    cat <<'USAGE'
Usage:
  scripts/check-arm-deps.sh --arch aarch64 --deps-root <path> [--sysroot <path>] [--cross-prefix aarch64-linux-gnu-]

Checks that the selected ARM toolchain exists and that dependency libraries
under <path> are built for the requested architecture.
USAGE
}

arch=
deps_root=
sysroot=
cross_prefix=
tool_runner=
runner_prefix=
deps_root_exec=
sysroot_exec=

to_wsl_path() {
    if [ -n "${WSL_DISTRO_NAME:-}" ] || [ -n "${WSL_INTEROP:-}" ]; then
        printf '%s\n' "$1"
    elif command -v wsl.exe >/dev/null 2>&1; then
        wsl.exe -e wslpath -a "$1" | tr -d '\r'
    else
        printf '%s\n' "$1"
    fi
}

while [ "$#" -gt 0 ]; do
    case "$1" in
        --arch)
            arch=$2
            shift 2
            ;;
        --deps-root)
            deps_root=$2
            shift 2
            ;;
        --sysroot)
            sysroot=$2
            shift 2
            ;;
        --cross-prefix)
            cross_prefix=$2
            shift 2
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            echo "unknown argument: $1" >&2
            usage >&2
            exit 2
            ;;
    esac
done

if [ -z "$arch" ] || [ -z "$deps_root" ]; then
    usage >&2
    exit 2
fi

case "$arch" in
    aarch64|arm64)
        expected_pattern='ARM aarch64|AArch64'
        cross_prefix=${cross_prefix:-aarch64-linux-gnu-}
        ;;
    armhf|armv7|arm)
        expected_pattern='ARM'
        cross_prefix=${cross_prefix:-arm-linux-gnueabihf-}
        ;;
    *)
        echo "unsupported arch: $arch" >&2
        exit 2
        ;;
esac

cxx="${cross_prefix}g++"
objdump="${cross_prefix}objdump"

if command -v "$cxx" >/dev/null 2>&1 && command -v "$objdump" >/dev/null 2>&1; then
    tool_runner=local
    deps_root_exec=$deps_root
    sysroot_exec=$sysroot
elif command -v wsl.exe >/dev/null 2>&1 \
    && wsl.exe -e bash -lc "command -v '$cxx' >/dev/null 2>&1 && command -v '$objdump' >/dev/null 2>&1"; then
    tool_runner=wsl
    runner_prefix='wsl.exe -e bash -lc'
    deps_root_exec=$(to_wsl_path "$deps_root")
    sysroot_exec=
    if [ -n "$sysroot" ]; then
        sysroot_exec=$(to_wsl_path "$sysroot")
    fi
else
    echo "missing cross compiler: $cxx" >&2
    exit 1
fi

if [ -n "$sysroot_exec" ] && [ "$tool_runner" = "local" ] && [ ! -d "$sysroot_exec" ]; then
    echo "missing sysroot directory: $sysroot" >&2
    exit 1
fi

if [ ! -d "$deps_root" ]; then
    echo "missing deps root: $deps_root" >&2
    exit 1
fi

required_files="
$deps_root_exec/thirdpart_lib/libprotobuf.so.26.0.6
$deps_root_exec/thirdpart_lib/libgfortran.so
$deps_root_exec/thirdpart_lib/libopenblas.so.0
$deps_root_exec/opencv_lib/libopencv_core.so.3.2
$deps_root_exec/opencv_lib/libopencv_imgcodecs.so.3.2
$deps_root_exec/opencv_lib/libopencv_imgproc.so.3.2
$deps_root_exec/nirvana_lib/libnirvana-kernel.so.8.15.1
"

failed=0
kernel_main="$deps_root_exec/nirvana_lib/libnirvana-kernel.so.8.15.1"
kernel_alias="$deps_root_exec/nirvana_lib/libnirvana-kernel.so.8"
kernel_test="$deps_root_exec/test_cfiq/lib/libnirvana-kernel.so.8.15.1"
kernel_test_alias="$deps_root_exec/test_cfiq/lib/libnirvana-kernel.so.8"

run_check_cmd() {
    if [ "$tool_runner" = "local" ]; then
        sh -c "$1"
    else
        eval "$runner_prefix \"$1\""
    fi
}

compare_kernel_hashes() {
    left=$1
    right=$2
    label=$3
    if ! run_check_cmd "[ -e '$right' ]"; then
        return 0
    fi
    left_hash=$(run_check_cmd "sha256sum -b '$left' | cut -d ' ' -f 1")
    right_hash=$(run_check_cmd "sha256sum -b '$right' | cut -d ' ' -f 1")
    if [ "$left_hash" != "$right_hash" ]; then
        echo "kernel version mismatch ($label): $left_hash != $right_hash" >&2
        failed=1
    fi
}

for file_path in $required_files; do
    if [ "$tool_runner" = "local" ]; then
        file_exists_cmd="[ -e \"$file_path\" ]"
        file_desc_cmd="file -L \"$file_path\""
    else
        file_exists_cmd="[ -e '$file_path' ]"
        file_desc_cmd="file -L '$file_path'"
    fi

    if [ "$tool_runner" = "local" ]; then
        if ! sh -c "$file_exists_cmd"; then
            echo "missing dependency: $file_path" >&2
            failed=1
            continue
        fi
        description=$(sh -c "$file_desc_cmd")
    else
        if ! eval "$runner_prefix \"$file_exists_cmd\""; then
            echo "missing dependency: $file_path" >&2
            failed=1
            continue
        fi
        description=$(eval "$runner_prefix \"$file_desc_cmd\"")
    fi

    if [ -z "$description" ]; then
        echo "missing dependency: $file_path" >&2
        failed=1
        continue
    fi
    printf '%s\n' "$description"
    if ! printf '%s\n' "$description" | grep -Eq "$expected_pattern"; then
        echo "architecture mismatch: $file_path" >&2
        failed=1
    fi
done

compare_kernel_hashes "$kernel_main" "$kernel_alias" "nirvana_lib .so.8 alias"
compare_kernel_hashes "$kernel_main" "$kernel_test" "test_cfiq packaged kernel"
compare_kernel_hashes "$kernel_main" "$kernel_test_alias" "test_cfiq .so.8 alias"

if [ "$failed" -ne 0 ]; then
    echo "ARM dependency check failed for deps root: $deps_root" >&2
fi

exit "$failed"
