# CaffeProCFIQ ARM64 发布包说明

本目录是可直接打包发布的 ARM64 Linux 运行包。

## 1. 目录结构

```text
aarch64-cmake/
├── README.md
├── include/
│   └── CFIQ.h
├── lib/
│   ├── libQualityCtrl.so
│   ├── libnirvana-kernel.so.8
│   ├── libnirvana-kernel.so.8.15.1
│   ├── libopenblas.so.0
│   ├── libblas.so.3
│   ├── liblapack.so.3
│   ├── libopencv_core.so.406
│   ├── libopencv_imgcodecs.so.406
│   ├── libopencv_imgproc.so.406
│   └── libprotobuf.so.26
└── test/
    ├── FingerQualityCtrlTest
    ├── HeatMap.bin
    ├── FQNet_model/
    └── SourceBmpLinuxTest36/
```

## 2. 系统环境要求

目标机器要求：

- CPU 架构：`aarch64`。
- OS：Linux，建议 Ubuntu 24.04 或更新版本。
- `glibc`：2.38 或更新版本。
- `libstdc++`：支持 `GLIBCXX_3.4.32`，对应 GCC 13 运行时。
- 基础工具：`bash`、`file`、`readelf`、`sha256sum`。

说明：

- 当前 ARM64 发布包中的 `FingerQualityCtrlTest` 需要 `GLIBC_2.38`。
- 随包 OpenCV/protobuf/OpenBLAS/libgfortran 运行库也需要到 `GLIBC_2.38`。
- `libQualityCtrl.so` 本体需要到 `GLIBC_2.34`。
- 运行机器不要求安装 gcc/g++ 编译器，但系统 C/C++ 运行时必须满足上述版本。
- OpenCV、protobuf、OpenBLAS、nirvana kernel 等核心运行库已随包放在 `lib/` 下；OpenCV 间接依赖的系统图形/影像运行库仍需要在新机安装。
- `libblas.so.3` 和 `liblapack.so.3` 必须优先解析到随包 OpenBLAS；否则可能落到系统参考 BLAS，导致 ARM 推理速度显著下降。

### Ubuntu 24.04 新机运行依赖

在全新 Ubuntu 24.04 ARM64 机器上，先安装系统运行时依赖：

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

- Ubuntu 24.04 可能自动选择 `libglib2.0-0t64`、`libgdcm3.0t64`、`libgdal34t64`，这是正常的 `t64` 包名变体。
- 上述依赖覆盖已验证缺失库：`libGLX.so.0`、`libtbb.so.12`、`libgdcmMSFF.so.3.0`、`libgdcmDSED.so.3.0`、`libopenjp2.so.7`、`libOpenEXR-3_1.so.30`、`libgdal.so.34`。
- 如果 `apt` 提示包名不存在，先执行 `apt-cache search libgdcm`、`apt-cache search libgdal`、`apt-cache search openexr` 查询当前源中的实际包名。

环境检查：

```bash
uname -m
ldd --version | head -1
strings /usr/lib/aarch64-linux-gnu/libstdc++.so.6 | grep GLIBCXX_3.4.32
ldconfig -p | grep -E 'libGLX.so.0|libtbb.so.12|libgdcmMSFF.so.3.0|libgdcmDSED.so.3.0|libopenjp2.so.7|libOpenEXR-3_1.so.30|libgdal.so.34'
```

## 3. 环境配置步骤

进入测试目录并配置动态库路径：

```bash
cd aarch64-cmake/test
export LD_LIBRARY_PATH="../lib:$PWD${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
ldd ./FingerQualityCtrlTest | grep "not found" || true
ldd ../lib/*.so* | grep "not found" || true
```

如果将 `test/`、`lib/` 内容合并到同一个业务目录，例如 JNI 调用目录，则在该目录下使用：

```bash
export LD_LIBRARY_PATH="$PWD/lib:$PWD${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
ldd ./lib/*.so* | grep "not found" || true
```

上述 `ldd` 命令没有输出 `not found` 才表示运行库解析完整。

检查核心库：

```bash
file ../lib/libQualityCtrl.so
file ../lib/libnirvana-kernel.so.8.15.1
sha256sum ../lib/libnirvana-kernel.so.8 ../lib/libnirvana-kernel.so.8.15.1
readelf -Ws ../lib/libnirvana-kernel.so.8.15.1 | grep GAFIS_ExtractBack
```

期望：

- `file` 显示 `ARM aarch64` 或 `AArch64`。
- 两个 `libnirvana-kernel` 文件 SHA-256 一致。
- `GAFIS_ExtractBack` 能查到。

## 4. 测试工具使用说明

测试工具：

```text
test/FingerQualityCtrlTest
```

参数格式：

```text
FingerQualityCtrlTest <deploy.prototxt> <caffemodel> <source-bmp-dir> <mode> <thread-count>
```

示例，mode 6 单线程分数测试：

```bash
cd aarch64-cmake/test
export LD_LIBRARY_PATH="../lib:$PWD${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
CFIQ_INLINE_THREAD=1 ./FingerQualityCtrlTest \
  ./FQNet_model/deploy.prototxt \
  ./FQNet_model/_iter_50000.caffemodel \
  ./SourceBmpLinuxTest36 \
  6 \
  1
```

输出：

```text
Score_Linux.txt
```

mode 1-5 会生成质量图，运行前建议准备输出目录：

```bash
rm -rf SourceBmpLinux QualityBmpLinux
mkdir -p SourceBmpLinux QualityBmpLinux
CFIQ_INLINE_THREAD=1 ./FingerQualityCtrlTest \
  ./FQNet_model/deploy.prototxt \
  ./FQNet_model/_iter_50000.caffemodel \
  ./SourceBmpLinuxTest36 \
  4 \
  1
```

结果检查：

```bash
wc -l Score_Linux.txt
find QualityBmpLinux -type f -name '*.bmp' | wc -l
awk 'NR==1{min=$2;max=$2} {if($2<min)min=$2;if($2>max)max=$2} END{print "count=" NR, "min=" min, "max=" max}' Score_Linux.txt
```

## 5. 算法库接口使用说明

算法库：

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

- `srcBmp`：640x640 灰度像素数据，不包含 BMP 文件头，顺序从左上到右下。
- `width` / `height`：当前固定传 `640` / `640`。
- `fgp`：指位号，范围 `1-10`。
- `Core_X` / `Core_Y`：中心点列号/行号；提取失败时可传 `0,0`。
- `localSize`：当前固定传 `48`。
- `stepSize`：当前固定传 `16`。
- `modeNumber`：`1-6`。
- `QualityScore`：输出分数，范围通常为 `0-1`。
- `pQuality`：mode 1-5 输出 BGR 质量图像素，大小 `640 * 640 * 3`。

单线程调用顺序：

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

## 6. 不同 mode 的区别

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
- 需要质量图并保持现有热图加权逻辑时使用 mode 1-3。
- 需要排除热图影响时使用 mode 4 或 mode 5。

## 7. 多线程调用算法库接口

多线程调用时，每个线程必须拥有独立的 `InitializeDll` 上下文，不能跨线程共享 `pForeGroundBuffer`、`pRowColPairStruct`、`pHeatMapFloat`、`pCaffeNet`、`pHInstance`、`pQuality`。

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

void worker(std::vector<Job> jobs,
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
            // ctx.quality 指向 BGR 质量图像素，可复制到线程私有输出缓冲区或立即写文件。
        }
    }

    DestructDLL(&ctx.fg, &ctx.row_col, &ctx.heatmap,
                &ctx.net, &ctx.kernel, &ctx.quality);
}
```

注意：

- `InitializeDll` 和 `DestructDLL` 必须在线程内成对调用。
- 结果容器、日志文件、质量图输出目录需要调用方自行加锁或分线程隔离。
- 不建议多个线程同时写同一个文件名。
- 输入像素缓冲区必须在 `GetQualityScore` 调用期间保持有效。
