#!/usr/bin/env sh
set -eu

repo_root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
deps_root=${DEPS_ROOT:-"$repo_root/deps/aarch64"}
sysroot=${SYSROOT:-}
jobs=${JOBS:-2}
cross_prefix=${CROSS_PREFIX:-aarch64-linux-gnu-}
stage_dir=${STAGE_DIR:-"$repo_root/out/aarch64"}
runner=

copy_if_exists() {
    src=$1
    dst_dir=$2
    if [ -e "$src" ]; then
        cp "$src" "$dst_dir/"
    fi
}

to_display_path() {
    if command -v cygpath >/dev/null 2>&1; then
        cygpath -m "$1"
    else
        printf '%s\n' "$1"
    fi
}

to_wsl_path() {
    if [ -n "${WSL_DISTRO_NAME:-}" ] || [ -n "${WSL_INTEROP:-}" ]; then
        printf '%s\n' "$1"
    elif command -v wsl.exe >/dev/null 2>&1; then
        wsl.exe -e wslpath -a "$1" | tr -d '\r'
    else
        printf '%s\n' "$1"
    fi
}

ensure_same_sha256() {
    left=$1
    right=$2
    left_hash=$(sha256sum -b "$left" | awk '{print $1}')
    right_hash=$(sha256sum -b "$right" | awk '{print $1}')
    if [ "$left_hash" != "$right_hash" ]; then
        echo "kernel alias mismatch: $left ($left_hash) != $right ($right_hash)" >&2
        exit 1
    fi
}

usage() {
    cat <<'USAGE'
Usage:
  DEPS_ROOT=/path/to/aarch64-deps SYSROOT=/path/to/sysroot scripts/build-aarch64.sh

Environment:
  DEPS_ROOT      ARM dependency root. Default: ./deps/aarch64
  SYSROOT        Target sysroot. Default: empty (use host multiarch runtime)
  CROSS_PREFIX   Toolchain prefix. Default: aarch64-linux-gnu-
  JOBS           Parallel make jobs. Default: 2
  STAGE_DIR      Output runtime package. Default: ./out/aarch64
USAGE
}

case "${1:-}" in
    -h|--help)
        usage
        exit 0
        ;;
esac

if command -v "${cross_prefix}g++" >/dev/null 2>&1; then
    runner=local
elif command -v wsl.exe >/dev/null 2>&1 \
    && wsl.exe -e bash -lc "command -v '${cross_prefix}g++' >/dev/null 2>&1"; then
    runner=wsl
else
    echo "missing cross compiler: ${cross_prefix}g++" >&2
    exit 1
fi

"$repo_root/scripts/check-arm-deps.sh" \
    --arch aarch64 \
    --deps-root "$deps_root" \
    --sysroot "$sysroot" \
    --cross-prefix "$cross_prefix"

case "$stage_dir" in
    "$repo_root"/out/*)
        ;;
    *)
        echo "refusing to remove STAGE_DIR outside repository out directory: $stage_dir" >&2
        exit 1
        ;;
esac

if [ "$runner" = "wsl" ]; then
    repo_root_exec=$(to_wsl_path "$repo_root")
    deps_root_exec=$(to_wsl_path "$deps_root")
    stage_dir_exec=$(to_wsl_path "$stage_dir")
    sysroot_exec=
    if [ -n "$sysroot" ]; then
        sysroot_exec=$(to_wsl_path "$sysroot")
    fi

    wsl.exe -e bash -lc "set -eu
        cd '$repo_root_exec/build'
        make clean
        make -j'$jobs' -f Makefile -f arm/aarch64-linux-gnu.mk \
            DEPS_ROOT='$deps_root_exec' \
            SYSROOT='${sysroot_exec}' \
            CROSS_COMPILE='${cross_prefix}'
    "
else
    make -C "$repo_root/build" clean
    make -C "$repo_root/build" -j"$jobs" -f Makefile -f arm/aarch64-linux-gnu.mk \
        DEPS_ROOT="$deps_root" \
        SYSROOT="$sysroot" \
        CROSS_COMPILE="$cross_prefix"
fi

rm -rf "$stage_dir"
mkdir -p "$stage_dir/lib" "$stage_dir/test"

cp "$repo_root/build/libQualityCtrl.so" "$stage_dir/lib/"
cp "$deps_root/nirvana_lib/libnirvana-kernel.so.8.15.1" "$stage_dir/lib/"
cp "$stage_dir/lib/libnirvana-kernel.so.8.15.1" "$stage_dir/lib/libnirvana-kernel.so.8"
copy_if_exists "$deps_root/opencv_lib/libopencv_core.so.4.6.0" "$stage_dir/lib"
copy_if_exists "$deps_root/opencv_lib/libopencv_imgproc.so.4.6.0" "$stage_dir/lib"
copy_if_exists "$deps_root/opencv_lib/libopencv_imgcodecs.so.4.6.0" "$stage_dir/lib"
copy_if_exists "$deps_root/thirdpart_lib/libopenblasp-r0.3.26.so" "$stage_dir/lib"
copy_if_exists "$deps_root/thirdpart_lib/libgfortran.so.5.0.0" "$stage_dir/lib"
copy_if_exists "$deps_root/thirdpart_lib/libprotobuf.so.26.0.6" "$stage_dir/lib"
ln -sf libopencv_core.so.4.6.0 "$stage_dir/lib/libopencv_core.so.406"
ln -sf libopencv_core.so.406 "$stage_dir/lib/libopencv_core.so.3.2"
ln -sf libopencv_imgproc.so.4.6.0 "$stage_dir/lib/libopencv_imgproc.so.406"
ln -sf libopencv_imgproc.so.406 "$stage_dir/lib/libopencv_imgproc.so.3.2"
ln -sf libopencv_imgcodecs.so.4.6.0 "$stage_dir/lib/libopencv_imgcodecs.so.406"
ln -sf libopencv_imgcodecs.so.406 "$stage_dir/lib/libopencv_imgcodecs.so.3.2"
ln -sf libopenblasp-r0.3.26.so "$stage_dir/lib/libopenblas.so.0"
ln -sf libopenblas.so.0 "$stage_dir/lib/libopenblas.so"
ln -sf libgfortran.so.5.0.0 "$stage_dir/lib/libgfortran.so.5"
ln -sf libgfortran.so.5 "$stage_dir/lib/libgfortran.so"
ln -sf libprotobuf.so.26.0.6 "$stage_dir/lib/libprotobuf.so.26"
ln -sf libprotobuf.so.26 "$stage_dir/lib/libprotobuf.so"
ensure_same_sha256 "$stage_dir/lib/libnirvana-kernel.so.8.15.1" "$stage_dir/lib/libnirvana-kernel.so.8"
cp -r "$repo_root/test_cfiq/build/FQNet_model" "$stage_dir/test/"
cp -r "$repo_root/test_cfiq/build/SourceBmpLinuxTest36" "$stage_dir/test/"
cp "$repo_root/test_cfiq/build/HeatMap.bin" "$stage_dir/test/"

if [ "$runner" = "wsl" ]; then
    wsl.exe -e bash -lc "set -eu
        cd '$repo_root_exec/test_cfiq/build'
        make clean
        make -j'$jobs' -f Makefile -f arm/aarch64-linux-gnu.mk \
            DEPS_ROOT='$deps_root_exec' \
            SYSROOT='${sysroot_exec}' \
            CROSS_COMPILE='${cross_prefix}' \
            RUNTIME_LIB_DIR='$stage_dir_exec/lib' \
            THIRDPART_LIB_DIR='$deps_root_exec/thirdpart_lib'
    "
else
    make -C "$repo_root/test_cfiq/build" clean
    make -C "$repo_root/test_cfiq/build" -j"$jobs" -f Makefile -f arm/aarch64-linux-gnu.mk \
        DEPS_ROOT="$deps_root" \
        SYSROOT="$sysroot" \
        CROSS_COMPILE="$cross_prefix" \
        RUNTIME_LIB_DIR="$stage_dir/lib" \
        THIRDPART_LIB_DIR="$deps_root/thirdpart_lib"
fi

cp "$repo_root/test_cfiq/build/FingerQualityCtrlTest" "$stage_dir/test/"

file "$stage_dir/lib/libQualityCtrl.so"
file "$stage_dir/test/FingerQualityCtrlTest"

display_stage_dir=$(to_display_path "$stage_dir")

cat <<EOF
ARM package staged at:
  $display_stage_dir

Run on target:
  cd $display_stage_dir/test
  LD_LIBRARY_PATH=../lib ./FingerQualityCtrlTest ./FQNet_model/deploy.prototxt ./FQNet_model/_iter_50000.caffemodel ./SourceBmpLinuxTest36 6 1
EOF
