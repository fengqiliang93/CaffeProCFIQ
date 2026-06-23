# CaffeProCFIQ Engineering Build Notes

## Goals

- Keep `include_cfiq/CFIQ.h` as the stable public ABI.
- Build the same source tree for `x86_64` and `aarch64`.
- Package runtime files by target architecture under `out/<arch>-cmake`.
- Prevent mixed `libnirvana-kernel.so.8` and `libnirvana-kernel.so.8.15.1`
  deployments.

## CMake Entry Points

Run CMake from Linux or WSL. On Windows, use the wrapper so the `Unix Makefiles`
presets execute inside WSL instead of accidentally creating a Windows CMake
cache:

Configure and build x86_64:

```bash
cmake --preset x86_64-linux
cmake --build --preset x86_64-linux-stage
```

Configure and build AArch64:

```bash
cmake --preset aarch64-linux
cmake --build --preset aarch64-linux-stage
```

Convenience wrapper:

```bash
scripts/build-cmake.sh x86_64
scripts/build-cmake.sh aarch64
```

The CMake build intentionally uses GNU++11. The legacy Linux source paths depend
on GNU/Linux predefined macros such as `linux`; strict ISO `-std=c++11` changes
that behavior and can remove Linux-only implementations at compile time.

## Runtime Package Rules

Runtime package assembly is implemented in `cmake/StageRuntime.cmake`; keep
root `CMakeLists.txt` focused on targets and dependency selection.

The staged package is designed to be movable:

- `lib/libQualityCtrl.so` has `RUNPATH=$ORIGIN`.
- `test/FingerQualityCtrlTest` has `RUNPATH=$ORIGIN:$ORIGIN/../lib`.
- CMake copies SONAME aliases such as `libprotobuf.so.26` and
  `libgfortran.so.5`, not only the fully-versioned source files.
- AArch64 packages also include the OpenCV 4.6 compatibility names
  `libopencv_*.so.406` and the OpenBLAS aliases required by
  `libQualityCtrl.so`.
- `libnirvana-kernel.so.8` and `libnirvana-kernel.so.8.15.1` are copied from
  the same source binary and verified with SHA-256 during staging.

## Dependency Layout

`CFIQ_DEPS_ROOT` selects the dependency package for the target architecture.
Both supported architectures use the same package shape under `deps/<arch>`.

- x86_64 default: `deps/x86_64`.
- aarch64 default: `deps/aarch64`.

Required libraries:

- `thirdpart_lib/libprotobuf.so.26.0.6`
- `opencv_lib/libopencv_core.so.3.2`
- `opencv_lib/libopencv_imgproc.so.3.2`
- `opencv_lib/libopencv_imgcodecs.so.3.2`
- `nirvana_lib/libnirvana-kernel.so.8.15.1`

Each dependency package must also keep these files byte-identical:

- `nirvana_lib/libnirvana-kernel.so.8`
- `nirvana_lib/libnirvana-kernel.so.8.15.1`
- `test_cfiq/lib/libnirvana-kernel.so.8`
- `test_cfiq/lib/libnirvana-kernel.so.8.15.1`

The CMake staging target copies `libnirvana-kernel.so.8.15.1` to both runtime
names and verifies SHA-256 equality.

The current dependency packages contain runtime binaries; common headers still
come from the repository include tree. Keep binary refreshes scoped under the
matching `deps/<arch>` directory.

The AArch64 test binary follows the legacy Makefile compatibility rule and links
with `--allow-shlib-undefined`. Target-side shared-library resolution is
validated by the staged runtime package and by running on the ARM host.

## Validation Baseline

The verified pre-refactor baseline is:

```text
F:/Code/CFIQ/LinuxQualityTest/Score_Linux.txt
```

Latest full-sample results:

- x86_64 current: `out/x86_full_current_rebuilt_score.tsv`
- aarch64 current: `out/arm_full_unified_score.tsv`
- true baseline comparison: `out/full_x86_arm_true_baseline_compare.tsv`
- summary: `out/full_x86_arm_true_baseline_summary.tsv`

Current conclusion for the 36 shared samples:

- x86_64 matches the baseline exactly.
- AArch64 maximum absolute score difference is `0.000347`.
- No AArch64 sample differs from x86_64 by more than `0.01`.

Current CMake package validation:

- `cmake --build --preset x86_64-linux-stage` succeeds.
- `cmake --build --preset aarch64-linux-stage` succeeds.
- The x86_64 staged package runs all 36 sample BMPs locally.
- Both staged packages contain byte-identical `libnirvana-kernel.so.8` and
  `libnirvana-kernel.so.8.15.1`.
- Both staged packages use relative RUNPATH values only.

## Existing Makefiles

The existing Makefile flow remains available while CMake is introduced:

```bash
make -C build
make -C test_cfiq/build
scripts/build-aarch64.sh
```

CMake should be treated as the canonical cross-platform project entry going
forward; Makefiles are retained as compatibility build scripts.
