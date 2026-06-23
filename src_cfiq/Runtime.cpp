#include "CFIQ.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dlfcn.h>
#include <fstream>
#include <iostream>
#include <limits.h>
#include <new>
#include <unistd.h>

#include "StructDef.h"
#include "caffe/net.hpp"

using namespace caffe;
using namespace std;

void InitializeDll(void **pForeGroundBuffer,
                   void **pRowColPairStruct,
                   void **pHeatMapFloat,
                   void **pCaffeNet,
                   void **pHInstance,
                   void **pQuality,
                   char *pPrototxtPath,
                   char *pCaffeModelPath)
{
    fprintf(stderr, "[init] alloc-buffers\n");
    *pHeatMapFloat = new float[640 * 640 * 10];
    memset((unsigned char *)*pHeatMapFloat, 0, sizeof(float) * 640 * 640 * 10);
    *pForeGroundBuffer = new unsigned char[640 * 640];
    memset((unsigned char *)*pForeGroundBuffer, 0, 640 * 640);
    *pRowColPairStruct = new RowColPairStruct[640 * 640];
    memset((unsigned char *)*pRowColPairStruct, 0, sizeof(RowColPairStruct) * 640 * 640);
    *pQuality = new unsigned char[640 * 640 * 3];

    char *curDllPath = new char[PATH_MAX];
    memset(curDllPath, 0, PATH_MAX);

    int cnt = readlink("/proc/self/exe", curDllPath, PATH_MAX);
    if (cnt < 0 || cnt >= PATH_MAX)
    {
        printf("获取当前路径失败！\n");
        fgetc(stdin);
        exit(-1);
    }

    char *pPath = strrchr(curDllPath, '/');

#ifdef CPU_ONLY
    Caffe::set_mode(Caffe::CPU);
#else
    Caffe::SetDevice(0);
    Caffe::set_mode(Caffe::GPU);
#endif

    fprintf(stderr, "[init] net-ctor %s\n", pPrototxtPath);
    Net<float> *pNet = new (std::nothrow) Net<float>(pPrototxtPath, caffe::TEST);
    fprintf(stderr, "[init] net-copy %s\n", pCaffeModelPath);
    pNet->CopyTrainedLayersFrom(pCaffeModelPath);
    fprintf(stderr, "[init] net-ready\n");
    *pCaffeNet = pNet;

    *pPath = '\0';
    strcat(curDllPath, "/HeatMap.bin");
    fprintf(stderr, "[init] heatmap-open %s\n", curDllPath);
    fstream inFile(curDllPath, ios::in | ios::binary);
    if (!inFile)
    {
        cout << "HeatMap.bin Load Error" << endl;
        fgetc(stdin);
        exit(-1);
    }
    inFile.read((char *)*pHeatMapFloat, 640 * 640 * 4 * 10);
    inFile.close();
    fprintf(stderr, "[init] heatmap-ready\n");

    *pPath = '\0';
    char *libPath = new char[PATH_MAX];
    memset(libPath, 0, PATH_MAX);

    strcat(libPath, curDllPath);
    strcat(libPath, "/../lib/libnirvana-kernel.so.8.15.1");
    fprintf(stderr, "[init] dlopen %s\n", libPath);
    void *pSoHandle = dlopen(libPath, RTLD_LAZY);
    if (pSoHandle == NULL)
    {
        memset(libPath, 0, PATH_MAX);
        strcat(libPath, curDllPath);
        strcat(libPath, "/libnirvana-kernel.so.8.15.1");
        fprintf(stderr, "[init] dlopen fallback %s\n", libPath);
        pSoHandle = dlopen(libPath, RTLD_LAZY);
    }
    if (pSoHandle == NULL)
    {
        printf("Load libnirvana-kernel.so.8.15.1 Error:(%s)\n", dlerror());
        fgetc(stdin);
        delete[] libPath;
        exit(-1);
    }
    *pHInstance = (void *)pSoHandle;
    fprintf(stderr, "[init] ready\n");
    delete[] libPath;
    delete[] curDllPath;
}

void DestructDLL(void **pForeGroundBuffer,
                 void **pRowColPairStruct,
                 void **pHeatMapFloat,
                 void **pCaffeNet,
                 void **pHInstance,
                 void **pQuality)
{
    delete[] (RowColPairStruct *)*pRowColPairStruct;
    *pRowColPairStruct = NULL;

    delete[] (unsigned char *)*pForeGroundBuffer;
    *pForeGroundBuffer = NULL;

    delete ((Net<float> *)*pCaffeNet);
    *pCaffeNet = NULL;

    delete[] (float *)*pHeatMapFloat;
    *pHeatMapFloat = NULL;

    delete[] (unsigned char *)*pQuality;
    *pQuality = NULL;

    dlclose(*pHInstance);
}
