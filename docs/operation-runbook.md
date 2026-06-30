# CaffeProCFIQ 编译、部署、测试全流程

本文档描述当前 CFIQ 工程从系统环境准备、依赖布置、x86_64/ARM64 编译、运行部署到测试验收的完整操作流程。

## 1. 适用范围

当前工程支持两个 Linux 目标架构：

- `x86_64`：本地 Linux/WSL 构建和运行。
- `aarch64`：ARM64 Linux 交叉编译，产物部署到 ARM64 机器运行。

CMake 是当前推荐的主构建入口。旧 Makefile 流程仍保留用于兼容和问题定位。

## 2. 必须的系统环境

### 2.1 Windows 开发机

如果在 Windows 上操作，建议使用 Git Bash 调用脚本，脚本会自动切到 WSL 执行 CMake/Make：

- Windows 10/11。
- Git for Windows，提供 Git Bash。
- WSL2，建议 Ubuntu 24.04 或更新版本。
- 工程放在 Windows 盘符下也可以，例如 `F:\Code\CFIQ\CaffeProCFIQ`；脚本会用 `wslpath` 转换为 `/mnt/f/...`。

不要直接用 Windows 原生 CMake 生成 Visual Studio 工程。当前工程的 CMake 预设使用 `Unix Makefiles`，应在 Linux/WSL 环境中执行。

### 2.2 WSL/Linux 构建环境

推荐构建系统：

- Ubuntu 24.04 或更新版本。
- `glibc` 2.38 或更新版本。
- `gcc/g++` 13.x 或更新版本；当前已验证版本为 GCC 13.3.0。
- CMake 3.16 或更新版本。

WSL 或 Linux 上必须安装：

```bash
sudo apt update
sudo apt install -y \
  build-essential \
  cmake \
  make \
  file \
  binutils \
  coreutils \
  patchelf
```

说明：

- `cmake`：读取 `CMakePresets.json` 并生成构建目录。
- `make`/`g++`：编译 x86_64 版本。
- `file`/`readelf`/`sha256sum`：用于架构、符号和二进制一致性检查。
- `patchelf`：用于检查或修正 RUNPATH 时使用，正常 CMake 构建不依赖它。

### 2.3 ARM64 交叉编译环境

推荐交叉编译系统：

- Ubuntu 24.04 或更新版本。
- `glibc` 2.38 或更新版本。
- `aarch64-linux-gnu-g++` 13.x 或更新版本；当前已验证版本为 13.3.0。

在 WSL/Linux 上构建 ARM64 版本时必须安装：

```bash
sudo apt update
sudo apt install -y \
  gcc-aarch64-linux-gnu \
  g++-aarch64-linux-gnu \
  binutils-aarch64-linux-gnu
```

可选安装 ARM64 sysroot：

```bash
sudo apt install -y libc6-dev-arm64-cross
```

默认交叉编译前缀为：

```text
aarch64-linux-gnu-
```

如果使用其他工具链，通过 `CROSS_PREFIX` 或 CMake toolchain 文件调整。

### 2.4 x86_64 运行机器

部署运行的 x86_64 机器必须满足：

- CPU 架构：`x86_64`。
- OS：Linux，建议 Ubuntu 24.04 或更新版本。
- `glibc` 2.38 或更新版本。当前完整 staging 包中 `FingerQualityCtrlTest`
  需要 `GLIBC_2.38`；`libQualityCtrl.so` 本体需要到 `GLIBC_2.34`。
- `libstdc++` 支持 `GLIBCXX_3.4.32`，对应 GCC 13 运行时。
- 可执行基础命令：`bash`、`file`、`readelf`、`sha256sum`。
- 有足够内存加载 Caffe 模型和运行 640x640 样本推理。

运行机不要求安装 gcc 编译器，但系统 C/C++ 运行时版本必须满足上述符号版本要求。

### 2.5 ARM64 运行机器

部署运行的 ARM 机器必须满足：

- CPU 架构：`aarch64`。
- OS：Linux，建议 Ubuntu 24.04 或更新版本。
- `glibc` 2.38 或更新版本。当前 ARM64 staging 包中 `FingerQualityCtrlTest`
  和随包 OpenCV/protobuf/OpenBLAS/libgfortran 运行库都需要到 `GLIBC_2.38`。
- `libstdc++` 支持 `GLIBCXX_3.4.32`，对应 GCC 13 运行时。
- 可执行基础命令：`bash`、`file`、`readelf`、`sha256sum`。
- 有足够内存加载 Caffe 模型和运行 640x640 样本推理。

目标机不需要安装 OpenCV、protobuf、OpenBLAS 等系统包；运行包会携带这些动态库。运行时通过 `LD_LIBRARY_PATH=../lib` 指向随包库目录。

## 3. 工程关键目录

```text
CaffeProCFIQ/
├── CMakeLists.txt
├── CMakePresets.json
├── cmake/
│   ├── StageRuntime.cmake
│   └── toolchains/aarch64-linux-gnu.cmake
├── deps/
│   ├── x86_64/
│   └── aarch64/
├── include_cfiq/
├── src_cfiq/
├── test_cfiq/
│   └── build/
│       ├── FQNet_model/
│       ├── HeatMap.bin
│       └── SourceBmpLinuxTest36/
└── out/
```

其中：

- `src_cfiq/`：CFIQ 业务实现，包括不同质量模式。
- `include_cfiq/CFIQ.h`：对外 ABI。
- `deps/<arch>/`：不同架构的二进制依赖包。
- `out/<arch>-cmake/`：CMake staging 后的可部署运行包。
- `test_cfiq/build/FQNet_model/`：测试模型文件。
- `test_cfiq/build/HeatMap.bin`：热图数据。
- `test_cfiq/build/SourceBmpLinuxTest36/`：36 张测试样本。

## 4. 依赖包要求

### 4.1 x86_64 依赖目录

默认路径：

```text
deps/x86_64/
```

必须包含：

```text
deps/x86_64/
├── nirvana_lib/
│   ├── libnirvana-kernel.so.8
│   └── libnirvana-kernel.so.8.15.1
├── opencv_lib/
│   ├── libopencv_core.so.3.2
│   ├── libopencv_imgcodecs.so.3.2
│   └── libopencv_imgproc.so.3.2
└── thirdpart_lib/
    ├── blas_LINUX.a
    ├── cblas_LINUX.a
    ├── libgfortran.so
    └── libprotobuf.so.26.0.6
```

### 4.2 ARM64 依赖目录

默认路径：

```text
deps/aarch64/
```

必须包含：

```text
deps/aarch64/
├── nirvana_lib/
│   ├── libnirvana-kernel.so.8
│   └── libnirvana-kernel.so.8.15.1
├── opencv_lib/
│   ├── libopencv_core.so.3.2
│   ├── libopencv_imgcodecs.so.3.2
│   └── libopencv_imgproc.so.3.2
├── test_cfiq/
│   └── lib/
│       ├── libnirvana-kernel.so.8
│       └── libnirvana-kernel.so.8.15.1
└── thirdpart_lib/
    ├── libgfortran.so
    ├── libopenblas.so.0
    └── libprotobuf.so.26.0.6
```

ARM 依赖包内所有 `.so` 必须是 ARM64 Linux 二进制，不能混用 x86_64 动态库。
发布目录中的 `libblas.so.3` 和 `liblapack.so.3` 应指向随包 OpenBLAS，避免运行时解析到系统参考 BLAS 造成明显性能下降。

### 4.3 libnirvana-kernel 一致性要求

以下文件必须来自同一个二进制内容：

```text
libnirvana-kernel.so.8
libnirvana-kernel.so.8.15.1
```

验证：

```bash
sha256sum deps/aarch64/nirvana_lib/libnirvana-kernel.so.8*
sha256sum deps/x86_64/nirvana_lib/libnirvana-kernel.so.8*
```

部署包 staging 时会自动把 `libnirvana-kernel.so.8.15.1` 复制成两个运行名，并校验 SHA-256 一致。

同时必须确认 `GAFIS_ExtractBack` 在 kernel 中导出：

```bash
readelf -Ws deps/aarch64/nirvana_lib/libnirvana-kernel.so.8.15.1 | grep GAFIS_ExtractBack
readelf -Ws deps/x86_64/nirvana_lib/libnirvana-kernel.so.8.15.1 | grep GAFIS_ExtractBack
```

## 5. 编译前检查

### 5.1 检查 CMake 预设

```bash
cmake --list-presets
```

应能看到：

```text
x86_64-linux
aarch64-linux
```

### 5.2 检查 ARM 依赖

```bash
scripts/check-arm-deps.sh \
  --arch aarch64 \
  --deps-root deps/aarch64 \
  --cross-prefix aarch64-linux-gnu-
```

该脚本会检查：

- 交叉编译器和 objdump 是否存在。
- ARM 依赖文件是否齐全。
- 依赖库是否为 ARM/AArch64 架构。
- `libnirvana-kernel.so.8` 和 `libnirvana-kernel.so.8.15.1` 内容是否一致。

## 6. 编译 x86_64 版本

推荐一键构建并 staging：

```bash
scripts/build-cmake.sh x86_64
```

等价 CMake 命令：

```bash
cmake --preset x86_64-linux
cmake --build --preset x86_64-linux-stage
```

成功后输出：

```text
out/x86_64-cmake/
├── lib/
│   ├── libQualityCtrl.so
│   ├── libnirvana-kernel.so.8
│   ├── libnirvana-kernel.so.8.15.1
│   └── ...
└── test/
    ├── FingerQualityCtrlTest
    ├── FQNet_model/
    ├── HeatMap.bin
    └── SourceBmpLinuxTest36/
```

## 7. 编译 ARM64 版本

推荐一键构建并 staging：

```bash
scripts/build-cmake.sh aarch64
```

等价 CMake 命令：

```bash
cmake --preset aarch64-linux
cmake --build --preset aarch64-linux-stage
```

成功后输出：

```text
out/aarch64-cmake/
├── lib/
│   ├── libQualityCtrl.so
│   ├── libnirvana-kernel.so.8
│   ├── libnirvana-kernel.so.8.15.1
│   ├── libopenblas.so.0
│   ├── libblas.so.3
│   ├── liblapack.so.3
│   ├── libopencv_core.so.406
│   └── ...
└── test/
    ├── FingerQualityCtrlTest
    ├── FQNet_model/
    ├── HeatMap.bin
    └── SourceBmpLinuxTest36/
```

## 8. 部署运行包

### 8.1 x86_64 本机部署

x86_64 可直接在 WSL/Linux 运行：

```bash
cd out/x86_64-cmake/test
LD_LIBRARY_PATH=../lib ./FingerQualityCtrlTest \
  ./FQNet_model/deploy.prototxt \
  ./FQNet_model/_iter_50000.caffemodel \
  ./SourceBmpLinuxTest36 \
  6 \
  1
```

### 8.2 ARM64 目标机部署

把整个目录复制到 ARM 机器：

```bash
scp -r out/aarch64-cmake root@<arm-ip>:/opt/cfiq/
```

在 ARM 机器上运行：

```bash
cd /opt/cfiq/aarch64-cmake/test
LD_LIBRARY_PATH=../lib ./FingerQualityCtrlTest \
  ./FQNet_model/deploy.prototxt \
  ./FQNet_model/_iter_50000.caffemodel \
  ./SourceBmpLinuxTest36 \
  6 \
  1
```

## 9. 测试流程

测试程序参数格式：

```text
FingerQualityCtrlTest <deploy.prototxt> <caffemodel> <source-bmp-dir> <mode> <thread-count>
```

常用参数：

- `<source-bmp-dir>`：`./SourceBmpLinuxTest36`
- `<mode>`：`1` 到 `6`
- `<thread-count>`：建议先用 `1` 做确定性验证。

单线程确定性运行建议加：

```bash
CFIQ_INLINE_THREAD=1
```

### 9.1 mode 6 分数测试

mode 6 只输出分数，不生成质量图：

```bash
cd out/x86_64-cmake/test
LD_LIBRARY_PATH=../lib CFIQ_INLINE_THREAD=1 ./FingerQualityCtrlTest \
  ./FQNet_model/deploy.prototxt \
  ./FQNet_model/_iter_50000.caffemodel \
  ./SourceBmpLinuxTest36 \
  6 \
  1
```

输出结果：

```text
Score_Linux.txt
```

### 9.2 mode 1-5 质量图测试

mode 1-5 会生成分数和质量图：

```bash
cd out/x86_64-cmake/test
rm -rf SourceBmpLinux QualityBmpLinux
mkdir -p SourceBmpLinux QualityBmpLinux

LD_LIBRARY_PATH=../lib CFIQ_INLINE_THREAD=1 ./FingerQualityCtrlTest \
  ./FQNet_model/deploy.prototxt \
  ./FQNet_model/_iter_50000.caffemodel \
  ./SourceBmpLinuxTest36 \
  4 \
  1
```

期望：

- `Score_Linux.txt` 行数等于输入 BMP 数量。
- `QualityBmpLinux/*.bmp` 数量等于输入 BMP 数量。
- 分数在 `0` 到 `1` 范围内。

统计命令：

```bash
wc -l Score_Linux.txt
find QualityBmpLinux -type f -name '*.bmp' | wc -l
awk 'NR==1{min=$2;max=$2} {if($2<min)min=$2;if($2>max)max=$2} END{print "count=" NR, "min=" min, "max=" max}' Score_Linux.txt
```

### 9.3 x86_64 与 ARM64 对比测试

在 x86_64 和 ARM64 上分别运行同一批样本、同一 mode、同一线程数，然后比较 `Score_Linux.txt`。

x86_64：

```bash
cd out/x86_64-cmake/test
LD_LIBRARY_PATH=../lib CFIQ_INLINE_THREAD=1 ./FingerQualityCtrlTest \
  ./FQNet_model/deploy.prototxt \
  ./FQNet_model/_iter_50000.caffemodel \
  ./SourceBmpLinuxTest36 \
  6 \
  1
cp Score_Linux.txt x86_64_mode6_score.txt
```

ARM64：

```bash
cd /opt/cfiq/aarch64-cmake/test
LD_LIBRARY_PATH=../lib CFIQ_INLINE_THREAD=1 ./FingerQualityCtrlTest \
  ./FQNet_model/deploy.prototxt \
  ./FQNet_model/_iter_50000.caffemodel \
  ./SourceBmpLinuxTest36 \
  6 \
  1
cp Score_Linux.txt arm64_mode6_score.txt
```

把 ARM 结果取回后比较：

```bash
awk '
  NR==FNR {score[$1]=$2; next}
  {
    diff = $2 - score[$1]
    if (diff < 0) diff = -diff
    print $1, score[$1], $2, diff
  }
' x86_64_mode6_score.txt arm64_mode6_score.txt
```

验收建议：

- 每个样本都能在两端找到对应分数。
- 单样本差异应接近 0。
- 全量样本最大差异应小于工程当前接受阈值，历史验证中 36 个样本 ARM64 与 x86_64 最大绝对差异约 `0.000347`。

### 9.4 与重构前基准对比

重构前基准：

```text
F:/Code/CFIQ/LinuxQualityTest/Score_Linux.txt
```

在 WSL 中比较：

```bash
baseline=/mnt/f/Code/CFIQ/LinuxQualityTest/Score_Linux.txt
current=/mnt/f/Code/CFIQ/CaffeProCFIQ/out/x86_64-cmake/test/Score_Linux.txt

awk '
  NR==FNR {score[$1]=$2; next}
  {
    diff = $2 - score[$1]
    if (diff < 0) diff = -diff
    print $1, score[$1], $2, diff
  }
' "$baseline" "$current"
```

## 10. 部署验收检查清单

部署包验收：

```bash
cd out/aarch64-cmake
file lib/libQualityCtrl.so
file lib/libnirvana-kernel.so.8.15.1
sha256sum lib/libnirvana-kernel.so.8 lib/libnirvana-kernel.so.8.15.1
readelf -Ws lib/libnirvana-kernel.so.8.15.1 | grep GAFIS_ExtractBack
readelf -d lib/libQualityCtrl.so | grep -E 'RPATH|RUNPATH'
readelf -d test/FingerQualityCtrlTest | grep -E 'RPATH|RUNPATH'
```

期望：

- `file` 显示目标架构正确。
- 两个 `libnirvana-kernel` 文件 SHA-256 一致。
- `GAFIS_ExtractBack` 存在。
- `RUNPATH` 使用相对路径：`$ORIGIN` 或 `$ORIGIN:$ORIGIN/../lib`。
- `FingerQualityCtrlTest` 运行退出码为 `0`。
- `Score_Linux.txt` 行数等于样本数。

## 11. 常见问题

### 11.1 Load libnirvana-kernel 失败

检查：

```bash
ls -l ../lib/libnirvana-kernel.so.8*
LD_LIBRARY_PATH=../lib ldd ./FingerQualityCtrlTest
```

重点确认当前运行目录是 `out/<arch>-cmake/test`，并且 `LD_LIBRARY_PATH=../lib`。

### 11.2 ARM 运行提示 Exec format error

说明二进制架构不匹配。检查：

```bash
file ./FingerQualityCtrlTest
file ../lib/libQualityCtrl.so
```

ARM 机器上必须显示 `ARM aarch64` 或 `AArch64`。

### 11.3 x86_64 和 ARM64 分数差异过大

优先检查：

- 两端是否使用同一批 BMP。
- 两端是否使用同一个 mode。
- 两端是否使用同一版模型和 `HeatMap.bin`。
- `libnirvana-kernel.so.8` 和 `libnirvana-kernel.so.8.15.1` 是否内容一致。
- `GAFIS_ExtractBack` 是否来自当前 kernel。
- 是否错误混入旧目录中的动态库。

### 11.4 质量图没有生成

mode 6 不生成质量图。需要质量图时使用 mode 1-5，并确认运行目录下存在：

```bash
mkdir -p SourceBmpLinux QualityBmpLinux
```

## 12. 推荐交付步骤

每次发版建议按以下顺序执行：

1. `git status --short` 确认源码状态。
2. `scripts/check-arm-deps.sh --arch aarch64 --deps-root deps/aarch64 --cross-prefix aarch64-linux-gnu-`。
3. `scripts/build-cmake.sh x86_64`。
4. `scripts/build-cmake.sh aarch64`。
5. 在 x86_64 跑 mode 6 全量样本。
6. 在 ARM64 跑 mode 6 同批样本。
7. 比较 x86_64、ARM64、重构前基准。
8. 检查 staging 包的 `file`、`sha256sum`、`readelf`、`RUNPATH`。
9. 归档 `Score_Linux.txt`、对比结果和构建日志。
