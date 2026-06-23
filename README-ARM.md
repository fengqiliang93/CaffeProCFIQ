# CaffeProCFIQ ARM Linux Build

This branch prepares the project for an ARM Linux build without changing the
public C ABI in `include_cfiq/CFIQ.h`.

## Target

The recommended first ARM target is 64-bit Linux:

```text
aarch64-linux-gnu
```

The 32-bit hard-float target can be added with the same Makefile variables, but
all dependencies must use the same ABI.

## System Version Requirements

Use Ubuntu 24.04 or newer for the current ARM64 deployment package. The package
requires `glibc` 2.38 or newer and a GCC 13 compatible C++ runtime
(`GLIBCXX_3.4.32`). The verified cross compiler is
`aarch64-linux-gnu-g++` 13.3.0.

The target device does not need the compiler installed, but its runtime must
provide the required `glibc` and `libstdc++` symbol versions.

## Dependency Package Layout

Create an ARM dependency root with the same logical layout as this repository:

```text
deps/aarch64/
├── include/
├── nirvana_lib/
│   ├── libnirvana-kernel.so.8.15.1
│   └── libnirvana-kernel.so.8
├── opencv_lib/
│   ├── libopencv_core.so.3.2
│   ├── libopencv_imgcodecs.so.3.2
│   └── libopencv_imgproc.so.3.2
├── test_cfiq/
│   └── lib/
│       ├── libQualityCtrl.so
│       ├── libnirvana-kernel.so.8.15.1
│       └── libnirvana-kernel.so.8
└── thirdpart_lib/
    ├── blas_LINUX.a
    ├── cblas_LINUX.a
    ├── libgfortran.so
    └── libprotobuf.so.26.0.6
```

Every binary in that package must be ARM Linux. The existing repository
libraries are x86-64 and cannot be reused for an ARM target.

`libnirvana-kernel.so.8` and `libnirvana-kernel.so.8.15.1` must resolve to the
same binary content. `FingerQualityCtrlTest` and `libQualityCtrl.so` both touch
the kernel: mixing an old `.so.8` with a new `.8.15.1` loads two kernel copies in
one process and changes MNT/core output.

## Check ARM Dependencies

```bash
scripts/check-arm-deps.sh \
  --arch aarch64 \
  --deps-root deps/aarch64 \
  --cross-prefix aarch64-linux-gnu-
```

This checks:

- the cross compiler exists;
- required dependency files exist;
- dependency binaries report an ARM architecture via `file`.
- all staged `libnirvana-kernel.so.8*` entries have the same SHA-256 content.

## One-Step Build

Preferred CMake build:

```bash
scripts/build-cmake.sh aarch64
```

This produces a movable runtime package under:

```text
out/aarch64-cmake/
```

The staged package contains relative RUNPATH values only:

- `lib/libQualityCtrl.so`: `$ORIGIN`
- `test/FingerQualityCtrlTest`: `$ORIGIN:$ORIGIN/../lib`

It also includes runtime SONAME aliases such as `libprotobuf.so.26`,
`libopencv_core.so.406`, `libopenblas.so.0`, and both
`libnirvana-kernel.so.8*` names.

Legacy Makefile build:

After the ARM dependency package is prepared:

```bash
DEPS_ROOT=deps/aarch64 \
SYSROOT=/usr/aarch64-linux-gnu \
scripts/build-aarch64.sh
```

The script:

1. validates the ARM toolchain and dependency package;
2. builds `build/libQualityCtrl.so`;
3. stages an ARM runtime package under `out/aarch64`;
4. builds `FingerQualityCtrlTest` against the staged ARM runtime package.

The CMake path is the canonical engineering entry going forward; the Makefile
path remains available for compatibility and low-level debugging.

## Manual Build `libQualityCtrl.so`

```bash
cd build
make clean
make -f Makefile -f arm/aarch64-linux-gnu.mk \
  DEPS_ROOT=../deps/aarch64 \
  SYSROOT=/path/to/aarch64-sysroot
```

Useful dry-run/config check:

```bash
make -f Makefile -f arm/aarch64-linux-gnu.mk print-config
make -n -f Makefile -f arm/aarch64-linux-gnu.mk \
  DEPS_ROOT=../deps/aarch64 \
  SYSROOT=/path/to/aarch64-sysroot
```

Source-only cross-compile check, useful before the ARM dependency package is
complete:

```bash
make clean
make -f Makefile -f arm/aarch64-linux-gnu.mk objects \
  DEPS_ROOT=../deps/aarch64 \
  SYSROOT=/usr/aarch64-linux-gnu
```

This verifies the project sources compile with the AArch64 compiler. It does
not link `libQualityCtrl.so`; linking still requires the ARM third-party
libraries listed above.

## Build Test Program

After copying the ARM runtime libraries into the dependency package:

```bash
cd test_cfiq/build
make clean
make -f Makefile -f arm/aarch64-linux-gnu.mk \
  DEPS_ROOT=../../../deps/aarch64 \
  SYSROOT=/path/to/aarch64-sysroot
```

Run the test on the ARM target device. The x86 host cannot execute the ARM test
binary without a matching emulator and runtime sysroot.

## Runtime Acceptance

On the ARM device:

```bash
cd out/aarch64-cmake/test
file ../lib/libQualityCtrl.so
file ../lib/libnirvana-kernel.so.8.15.1
readelf -Ws ../lib/libnirvana-kernel.so.8.15.1 | grep GAFIS_ExtractBack
LD_LIBRARY_PATH=../lib ./FingerQualityCtrlTest \
  ./FQNet_model/deploy.prototxt \
  ./FQNet_model/_iter_50000.caffemodel \
  ./SourceBmpLinuxTest36 \
  6 \
  1
```

Expected:

- `libQualityCtrl.so` and `libnirvana-kernel.so.8.15.1` are ARM Linux binaries.
- `libnirvana-kernel.so.8` and `libnirvana-kernel.so.8.15.1` have the same
  SHA-256 content.
- `GAFIS_ExtractBack` is exported by `libnirvana-kernel.so.8.15.1`.
- The sample test exits with code `0` and prints one score per BMP.
