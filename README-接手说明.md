# CaffeProCFIQ 接手说明

本仓库是一个 Linux C++ 指纹质量评分动态库项目。核心交付物是
`libQualityCtrl.so`，测试程序 `FingerQualityCtrlTest` 用来验证动态库、模型、
热图和私有依赖是否能在目标环境中协同工作。

## 1. 项目用途

项目基于裁剪过的 Caffe 前向推理代码，对 640x640 指纹灰度图进行局部块质量
分类，并结合手指位置热图输出整体质量分数和质量可视化图。

主要输入：

- 640x640 的 8-bit 灰度 BMP 像素数据。
- 指位号 `fgp`，取值 1-10。
- 指纹中心点坐标 `Core_X`、`Core_Y`。
- Caffe 网络结构文件 `deploy.prototxt`。
- Caffe 模型文件 `_iter_50000.caffemodel`。
- 运行目录下的 `HeatMap.bin`。

主要输出：

- `QualityScore`，范围通常为 0-1。
- `pQuality` 指向的 BGR 质量图像素缓冲区。

## 2. 目录结构

```text
.
├── build/                 # 主动态库 Makefile；libQualityCtrl.so 为构建输出
├── deps/
│   ├── x86_64/            # x86_64 运行依赖包
│   └── aarch64/           # ARM64 运行依赖包
├── include/               # Caffe、OpenCV、Boost、protobuf 等头文件
├── include_cfiq/          # 动态库对外 API 和内部结构声明
├── src/                   # 裁剪后的 Caffe 源码和 caffe.pb.cc
├── src_cfiq/              # CFIQ 业务实现
└── test_cfiq/             # 测试程序、模型、热图、样例图片
```

重点文件：

- `include_cfiq/CFIQ.h`：对外 C ABI 接口说明。
- `src_cfiq/CFIQ.cpp`：初始化、打分模式、Caffe 推理、热图加权、质量图生成。
- `src_cfiq/GetCutInfo.cpp`：在前景图中按局部块切分候选区域。
- `test_cfiq/src/main.cpp`：真实调用示例。
- `test_cfiq/build/ReadMe.txt`：原始运行说明，编码为 GBK。

## 3. 构建方式

主动态库：

```bash
cd build
make
```

默认使用 `deps/x86_64`。构建 ARM64 时使用 CMake 预设，或显式传入
`TARGET_ARCH=aarch64 DEPS_ROOT=../deps/aarch64`。

构建输出：

```text
build/libQualityCtrl.so
```

测试程序：

```bash
cd test_cfiq/build
make
```

测试程序默认从 `deps/x86_64/test_cfiq/lib` 和
`deps/x86_64/thirdpart_lib` 链接运行依赖。

构建输出：

```text
test_cfiq/build/FingerQualityCtrlTest
```

当前 Makefile 默认 `CPU_ONLY=1`。如果要切 GPU/CUDNN，需要同步确认 CUDA、
CUDNN、`nvcc -arch/-code` 和 `note/fixbug.txt` 中提到的 workspace 对齐补丁。

## 4. 运行环境

当前 staging 部署包建议 Ubuntu 24.04 或更新版本，运行侧需要 `glibc`
2.38+ 和 GCC 13 对应的 `libstdc++` 运行时。旧说明中的 Ubuntu 18.04
只适合追溯历史环境，不能直接作为当前交付包的运行基线。

运行前需要：

- 当前目录包含 `libQualityCtrl.so`、`libSegment.so`、`libnirvana-kernel.so.8`、
  OpenCV/protobuf 等依赖动态库。
- 当前目录包含 `HeatMap.bin`。
- `LD_LIBRARY_PATH` 包含当前目录 `.`，或系统动态库配置能找到当前目录。
- `/etc/nirvana.conf` 中包含 `/tmp`，或将测试包内对应配置复制到 `/etc/`。

测试命令：

```bash
cd test_cfiq/build
./FingerQualityCtrlTest ./FQNet_model/deploy.prototxt ./FQNet_model/_iter_50000.caffemodel ./SourceBmpLinuxTest36 3 8
```

参数含义：

```text
argv[1] deploy.prototxt 路径
argv[2] caffemodel 路径
argv[3] 待测 BMP 目录
argv[4] 模式号
argv[5] 线程数
```

验收现象：

- 终端输出每张图片的分数。
- 生成或更新 `Score_Linux.txt`。
- 非 mode 6 时，在 `SourceBmpLinux/` 和 `QualityBmpLinux/` 下输出原图和质量图。

## 5. 对外 API

头文件：`include_cfiq/CFIQ.h`

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

典型调用顺序：

```cpp
void *pForeGroundBuffer = nullptr;
void *pRowColPairStruct = nullptr;
void *pHeatMapFloat = nullptr;
void *pCaffeNet = nullptr;
void *pHInstance = nullptr;
void *pQuality = nullptr;

InitializeDll(
    &pForeGroundBuffer,
    &pRowColPairStruct,
    &pHeatMapFloat,
    &pCaffeNet,
    &pHInstance,
    &pQuality,
    prototxtPath,
    caffeModelPath);

GetQualityScore(
    pForeGroundBuffer,
    pRowColPairStruct,
    pHeatMapFloat,
    pCaffeNet,
    pHInstance,
    pQuality,
    srcBmp,
    640,
    640,
    fgp,
    coreX,
    coreY,
    48,
    16,
    modeNumber,
    &qualityScore);

DestructDLL(
    &pForeGroundBuffer,
    &pRowColPairStruct,
    &pHeatMapFloat,
    &pCaffeNet,
    &pHInstance,
    &pQuality);
```

多线程约束：

- 每个线程必须独立调用一次 `InitializeDll`。
- 线程内可重复调用 `GetQualityScore`。
- 线程退出前必须调用 `DestructDLL`。
- 不要跨线程共享 `pCaffeNet`、`pQuality` 等状态指针。

## 6. 核心算法流程

1. `InitializeDll` 分配固定大小缓冲区，加载 Caffe 网络和模型。
2. 从可执行文件所在目录读取 `HeatMap.bin`，大小应为 `640 * 640 * 10 * sizeof(float)`。
3. `GetQualityScore` 按 `modeNumber` 分发到 mode 1、2、3、4、5、6。
4. 调用私有函数 `Segment` 生成前景掩膜。
5. `GetCutInfo` 在前景掩膜中寻找可切分的 48x48 局部块，步长通常为 16。
6. 每个局部块归一化后送入 Caffe 网络，输出 3 类质量等级。
7. mode 1/2/3/6 根据 `fgp` 选择对应热图通道，对局部块质量做加权求分。
8. mode 1/2/3/4/5 会生成质量图；mode 6 只计算分数。

## 7. 打分模式

代码实际支持：

- mode 1：矫正后算分，质量图也按矫正后位置显示。
- mode 2：矫正后算分，质量图按原位置显示。
- mode 3：矫正后算分，绘制完整前景质量图，按原位置显示。
- mode 4：矫正后算分，不使用热图，绘制完整前景质量图，按原位置显示。
- mode 5：未矫正算分，不使用热图，绘制完整前景质量图，按原位置显示。
- mode 6：只算分，不输出质量图。

注意：`CFIQ.h` 与 `GetQualityScore` 入口现在统一支持 mode 1-6。

## 8. 已知风险

- `src_cfiq/QualityMode*.cpp` 之间仍有大量重复逻辑，后续可按“前景提取、局部块推理、质量图绘制、分数归一化”继续抽取。
- 多处 `new[]` 使用了 `delete` 而非 `delete[]`，后续修内存问题时优先排查。
- `GetCutInfo.cpp` 中 `memset(pRowInterArray, 0, width)` 只按字节清零，数组元素是
  `unsigned short int`，严格来说应按 `sizeof(unsigned short int) * width`。
- `test_cfiq/src/main.cpp` 多线程同时向同一个 `std::map` 写入，缺少锁。
- `InitializeDll` 在库函数中直接 `exit(-1)` 和 `fgetc(stdin)`，嵌入服务时会直接杀进程或卡住。
- `HeatMap.bin` 查找路径绑定到 `/proc/self/exe` 所在目录，不是动态库所在目录。
- 图像尺寸、局部块大小、热图尺寸都强绑定 640x640 / 48 / 16。
- `fgp` 未显式校验，越界会读错热图通道。
- 非 1-6 的 `modeNumber` 静默无输出。
- Windows 和 Linux 释放 Caffe Net 的注释互相矛盾，跨平台构建前需要重新验证。

## 9. 建议接手顺序

1. 先在 Ubuntu 24.04 或兼容容器中跑通 `test_cfiq/build` 下的样例命令。
2. 固定一份基准输出：`Score_Linux.txt`、若干质量图、命令行日志。
3. 修最小安全问题：`delete[]`、`memset` 大小、`fgp/modeNumber` 参数校验。
4. 给测试程序的 `resultMap` 写入加锁，避免多线程未定义行为。
5. 把 `CFIQ.cpp` 按职责拆分：
   - runtime 初始化和资源释放；
   - 前景分割和切块；
   - Caffe 局部块推理；
   - 热图加权评分；
   - 质量图绘制；
   - mode 分发。
6. 拆分前后持续对比基准输出，避免算法行为漂移。
