# CaffeProCFIQ

CaffeProCFIQ 是一个面向 Linux x86_64 与 ARM64 的指纹图像质量评分工程。工程将 CFIQ 质量评分、前景分割、质量图生成和测试工具统一到 CMake 构建链中，并输出可直接部署的多架构运行包。

当前交付目标：

- 使用同一套源码构建 `x86_64` 与 `aarch64` 两类 Linux 版本。
- 发布目录自带 `libQualityCtrl.so`、`libnirvana-kernel.so.8.15.1`、模型、测试工具和 `CFIQ.h`。
- 支持 mode 1-6 的质量评分与质量图输出。
- 支持业务侧通过 C ABI 集成算法库。
- 支持多线程调用，但每个线程必须持有独立算法上下文。

## 支持平台

| 架构 | 构建方式 | 交付目录 |
| --- | --- | --- |
| x86_64 | Linux/WSL 原生构建 | `out/x86_64-cmake/` |
| aarch64 | Linux/WSL 交叉编译 | `out/aarch64-cmake/` |

建议构建和运行系统均使用 Ubuntu 24.04 或更高版本。当前发布包依赖 `glibc 2.38+` 和 GCC 13 运行时符号，目标机不要求安装编译器，但必须满足运行时版本要求。

## 工程结构

```text
CaffeProCFIQ/
├── CMakeLists.txt
├── CMakePresets.json
├── cmake/
│   ├── StageRuntime.cmake
│   ├── VerifySameSha256.cmake
│   └── toolchains/
├── deps/
│   ├── x86_64/
│   └── aarch64/
├── include_cfiq/
│   └── CFIQ.h
├── src_cfiq/
├── test_cfiq/
└── out/
```

主要目录说明：

- `src_cfiq/`：CFIQ 算法封装与质量模式实现。
- `include_cfiq/CFIQ.h`：对外 C ABI 头文件。
- `deps/x86_64/`：x86_64 架构依赖库。
- `deps/aarch64/`：ARM64 架构依赖库。
- `test_cfiq/`：测试程序源码、模型和样本输入。
- `cmake/`：运行包 staging、校验和交叉编译配置。
- `out/<arch>-cmake/`：构建后可直接打包发布的交付目录。

## 系统环境要求

### 构建环境

推荐在 Ubuntu 24.04 或更新版本上构建。Windows 开发机建议通过 WSL2 或 Git Bash 调用脚本进入 Linux 环境，不建议使用 Windows 原生 Visual Studio 生成器。

基础构建依赖：

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

ARM64 交叉编译还需要：

```bash
sudo apt install -y \
  gcc-aarch64-linux-gnu \
  g++-aarch64-linux-gnu \
  binutils-aarch64-linux-gnu
```

版本要求：

- Ubuntu 24.04 或更高版本。
- `glibc` 2.38 或更高版本。
- GCC/G++ 13.x 或更高版本。
- CMake 3.16 或更高版本。

### 运行环境

x86_64 与 ARM64 运行机器均建议使用 Ubuntu 24.04 或更高版本，并安装以下系统运行时依赖：

```bash
sudo apt update
sudo apt install -y \
  ca-certificates \
  libstdc++6 \
  libgcc-s1 \
  libc6 \
  libgomp1 \
  libglib2.0-0 \
  libglx0 \
  libglvnd0 \
  libgl1 \
  libx11-6 \
  libxext6 \
  libsm6 \
  libice6 \
  libtbb12 \
  libgdcm3.0 \
  libopenjp2-7 \
  libopenexr-3-1-30 \
  libgdal34
sudo ldconfig
```

说明：

- Ubuntu 24.04 可能自动选择 `libglib2.0-0t64`、`libgdcm3.0t64`、`libgdal34t64`，这是正常的包名变体。
- 发布包已携带 OpenCV、protobuf、OpenBLAS 和 nirvana kernel 等核心运行库。
- OpenCV 的部分间接依赖仍需由目标系统提供。
- `libstdc++` 需要支持 `GLIBCXX_3.4.32`。
- ARM64 发布包中的 `libblas.so.3` 和 `liblapack.so.3` 应优先解析到随包 OpenBLAS，避免落到系统参考 BLAS 导致性能下降。

运行前可检查：

```bash
uname -m
ldd --version | head -1
ldconfig -p | grep -E 'libGLX.so.0|libtbb.so.12|libgdcmMSFF.so.3.0|libgdcmDSED.so.3.0|libopenjp2.so.7|libOpenEXR-3_1.so.30|libgdal.so.34'
```

## 构建

工程通过 `CMakePresets.json` 管理构建入口。`cmake --preset` 负责生成 Unix Makefiles 构建目录，`cmake --build --preset` 会调用对应构建目录中的 Makefile 完成编译和 staging。

查看可用预设：

```bash
cmake --list-presets
```

构建 x86_64 交付包：

```bash
cmake --preset x86_64-linux
cmake --build --preset x86_64-linux-stage
```

构建 ARM64 交付包：

```bash
cmake --preset aarch64-linux
cmake --build --preset aarch64-linux-stage
```

也可以使用脚本：

```bash
scripts/build-cmake.sh x86_64
scripts/build-cmake.sh aarch64
```

构建完成后生成：

```text
out/
├── x86_64-cmake/
└── aarch64-cmake/
```

这两个架构目录就是可直接打包发布的运行目录。

## 交付目录

典型交付目录如下：

```text
<arch>-cmake/
├── README.md
├── include/
│   └── CFIQ.h
├── lib/
│   ├── libQualityCtrl.so
│   ├── libnirvana-kernel.so.8
│   ├── libnirvana-kernel.so.8.15.1
│   └── ...
└── test/
    ├── FingerQualityCtrlTest
    ├── HeatMap.bin
    ├── FQNet_model/
    └── SourceBmpLinuxTest36/
```

交付前建议检查：

```bash
cd out/<arch>-cmake
file lib/libQualityCtrl.so
file lib/libnirvana-kernel.so.8.15.1
sha256sum lib/libnirvana-kernel.so.8 lib/libnirvana-kernel.so.8.15.1
readelf -Ws lib/libnirvana-kernel.so.8.15.1 | grep GAFIS_ExtractBack
ldd test/FingerQualityCtrlTest | grep "not found" || true
ldd lib/*.so* | grep "not found" || true
```

期望：

- `file` 显示目标架构正确。
- `libnirvana-kernel.so.8` 与 `libnirvana-kernel.so.8.15.1` 内容一致。
- `GAFIS_ExtractBack` 符号存在。
- `ldd` 不输出 `not found`。

## 测试工具

测试程序：

```text
test/FingerQualityCtrlTest
```

参数格式：

```text
FingerQualityCtrlTest <deploy.prototxt> <caffemodel> <source-bmp-dir> <mode> <thread-count>
```

单线程 mode 6 分数测试：

```bash
cd out/x86_64-cmake/test
export LD_LIBRARY_PATH="../lib:$PWD${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
CFIQ_INLINE_THREAD=1 ./FingerQualityCtrlTest \
  ./FQNet_model/deploy.prototxt \
  ./FQNet_model/_iter_50000.caffemodel \
  ./SourceBmpLinuxTest36 \
  6 \
  1
```

mode 1-5 会生成质量图。运行前建议清理并准备输出目录：

```bash
rm -rf SourceBmpLinux QualityBmpLinux
mkdir -p SourceBmpLinux QualityBmpLinux
CFIQ_INLINE_THREAD=1 ./FingerQualityCtrlTest \
  ./FQNet_model/deploy.prototxt \
  ./FQNet_model/_iter_50000.caffemodel \
  ./SourceBmpLinuxTest36 \
  3 \
  1
```

输出文件：

- `Score_Linux.txt`：样本质量分数。
- `QualityBmpLinux/*.bmp`：mode 1-5 的质量图。
- `SourceBmpLinux/*.bmp`：测试工具生成的中间图。

结果检查：

```bash
wc -l Score_Linux.txt
find QualityBmpLinux -type f -name '*.bmp' | wc -l
awk 'NR==1{min=$2;max=$2} {if($2<min)min=$2;if($2>max)max=$2} END{print "count=" NR, "min=" min, "max=" max}' Score_Linux.txt
```

## 算法接口

对外库和头文件：

```text
lib/libQualityCtrl.so
include/CFIQ.h
```

核心 C ABI：

```cpp
extern "C" void InitializeDll(
    void **pForeGroundBuffer,
    void **pRowColPairStruct,
    void **pHeatMapFloat,
    void **pCaffeNet,
    void **pHInstance,
    void **pQuality,
    char *pPrototxtPath,
    char *pCaffeModelPath);

extern "C" void GetQualityScore(
    void *pForeGroundBuffer,
    void *pRowColPairStruct,
    void *pHeatMapFloat,
    void *pCaffeNet,
    void *pHInstance,
    void *pQuality,
    unsigned char *srcBmp,
    int width,
    int height,
    int fgp,
    int Core_X,
    int Core_Y,
    int localSize,
    int stepSize,
    unsigned char modeNumber,
    float *QualityScore);

extern "C" void DestructDLL(
    void **pForeGroundBuffer,
    void **pRowColPairStruct,
    void **pHeatMapFloat,
    void **pCaffeNet,
    void **pHInstance,
    void **pQuality);
```

关键参数：

- `srcBmp`：640x640 灰度像素数据，不包含 BMP 文件头。
- `width` / `height`：固定传 `640` / `640`。
- `fgp`：指位号，范围 `1-10`。
- `Core_X` / `Core_Y`：中心点列号/行号；如果 Core 提取失败可传 `0,0`。
- `localSize`：固定传 `48`。
- `stepSize`：固定传 `16`。
- `modeNumber`：`1-6`。
- `QualityScore`：输出质量分数，通常在 `0-1` 范围内。
- `pQuality`：mode 1-5 输出 BGR 质量图像素，大小为 `640 * 640 * 3`。

单线程调用示例：

```cpp
void *fg = nullptr;
void *row_col = nullptr;
void *heatmap = nullptr;
void *net = nullptr;
void *kernel = nullptr;
void *quality = nullptr;

InitializeDll(&fg, &row_col, &heatmap, &net, &kernel, &quality,
              prototxt_path, caffemodel_path);

float score = 0.0f;
GetQualityScore(fg, row_col, heatmap, net, kernel, quality,
                src_pixels, 640, 640, fgp,
                core_x, core_y, 48, 16,
                mode, &score);

DestructDLL(&fg, &row_col, &heatmap, &net, &kernel, &quality);
```

## Mode 说明

| mode | 是否矫正算分 | 是否使用热图 | 是否输出质量图 | 质量图位置 |
| --- | --- | --- | --- | --- |
| 1 | 是 | 是 | 是 | 矫正后位置 |
| 2 | 是 | 是 | 是 | 原始位置 |
| 3 | 是 | 是 | 是 | 完整前景，原始位置 |
| 4 | 是 | 否 | 是 | 完整前景，原始位置 |
| 5 | 否 | 否 | 是 | 完整前景，原始位置 |
| 6 | 是 | 是 | 否 | 不输出 |

建议：

- 只需要分数时使用 mode 6。
- 需要质量图并保留热图加权逻辑时使用 mode 1-3。
- 需要排除热图影响时使用 mode 4 或 mode 5。
- mode 3 会额外保留局部块网格边框样式，适合检查质量图覆盖区域。

## 多线程调用

多线程调用时，每个线程必须拥有独立的 `InitializeDll` 上下文。不要跨线程共享以下指针：

- `pForeGroundBuffer`
- `pRowColPairStruct`
- `pHeatMapFloat`
- `pCaffeNet`
- `pHInstance`
- `pQuality`

CPP 伪代码：

```cpp
struct ThreadContext {
    void *fg = nullptr;
    void *row_col = nullptr;
    void *heatmap = nullptr;
    void *net = nullptr;
    void *kernel = nullptr;
    void *quality = nullptr;
};

struct Job {
    unsigned char *src_pixels;
    int fgp;
    int core_x;
    int core_y;
    unsigned char mode;
};

void worker(const std::vector<Job> &jobs,
            char *prototxt_path,
            char *caffemodel_path,
            std::mutex &result_mutex,
            std::map<std::string, float> &result) {
    ThreadContext ctx;
    InitializeDll(&ctx.fg, &ctx.row_col, &ctx.heatmap,
                  &ctx.net, &ctx.kernel, &ctx.quality,
                  prototxt_path, caffemodel_path);

    for (const Job &job : jobs) {
        float score = 0.0f;
        GetQualityScore(ctx.fg, ctx.row_col, ctx.heatmap,
                        ctx.net, ctx.kernel, ctx.quality,
                        job.src_pixels,
                        640, 640,
                        job.fgp,
                        job.core_x,
                        job.core_y,
                        48, 16,
                        job.mode,
                        &score);

        {
            std::lock_guard<std::mutex> lock(result_mutex);
            result[job_id(job)] = score;
        }

        if (job.mode >= 1 && job.mode <= 5) {
            // ctx.quality points to a 640 * 640 * 3 BGR buffer.
            // Copy it to a thread-local output buffer or write it
            // to a unique filename before the next call reuses it.
        }
    }

    DestructDLL(&ctx.fg, &ctx.row_col, &ctx.heatmap,
                &ctx.net, &ctx.kernel, &ctx.quality);
}
```

注意事项：

- `InitializeDll` 和 `DestructDLL` 必须在线程内成对调用。
- 输出分数容器、日志和质量图文件名由调用方自行保证并发安全。
- 输入像素缓冲区必须在 `GetQualityScore` 调用期间保持有效。
- 不建议多个线程同时写同一个输出文件。

## 当前评分规则说明

当前版本包含以下规则：

- 局部质量块权重使用 `1 / 3 / 5`。
- 当 `Core_X == 0 && Core_Y == 0` 时，视为 Core 提取失败。
- Core 提取失败时，使用指纹前景区域所有像素点的重心作为替代中心点。

## 发布包

Release 资产通常包含：

- `x86_64-cmake-<date>.tar.gz`
- `aarch64-cmake-<date>.tar.gz`
- `SHA256SUMS-<date>.txt`

每个架构包都是完整运行目录，可独立部署。部署时保留目录内的 `lib/`、`include/`、`test/` 和 `README.md`。

## 常见问题

### 运行时提示动态库 not found

检查系统依赖和随包库路径：

```bash
cd out/<arch>-cmake/test
export LD_LIBRARY_PATH="../lib:$PWD${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
ldd ./FingerQualityCtrlTest | grep "not found" || true
ldd ../lib/*.so* | grep "not found" || true
```

没有输出 `not found` 才表示动态库解析完整。

### ARM64 机器提示 Exec format error

说明二进制架构不匹配，检查：

```bash
file ./FingerQualityCtrlTest
file ../lib/libQualityCtrl.so
file ../lib/libnirvana-kernel.so.8.15.1
```

ARM64 机器上应显示 `AArch64` 或 `ARM aarch64`。

### 质量图没有生成

mode 6 只输出分数，不生成质量图。需要质量图时使用 mode 1-5，并确认输出目录存在：

```bash
mkdir -p SourceBmpLinux QualityBmpLinux
```

### x86_64 与 ARM64 分数差异过大

优先检查：

- 两端是否使用同一批 BMP。
- 两端是否使用同一个 mode。
- 两端是否使用同一版模型和 `HeatMap.bin`。
- 两端是否加载了同一版本 `libQualityCtrl.so` 与 `libnirvana-kernel.so.8.15.1`。
- ARM64 是否优先使用随包 OpenBLAS。
- `GAFIS_ExtractBack` 是否能在当前 kernel 中查到。

### 首次调用得到异常分数

请确认调用方在首次 `GetQualityScore` 前已经成功执行 `InitializeDll`，并且 `srcBmp`、模型路径、动态库路径均有效。当前工程已在初始化流程中处理 kernel 冷启动路径，业务侧不需要额外预热。

