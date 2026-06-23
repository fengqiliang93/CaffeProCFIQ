/*
 * main.cpp
 *
 *  Created on: Mar 18, 2019
 *      Author: lyon
 */
#include<dlfcn.h>
//#include"CaffeHeader.h"
//#include"caffe/caffe.hpp"
#include<iostream>
#include<string.h>
#include<fstream>
#include<cstdlib>
#include"MakeBmp.h"
#include"ExtractMnt.h"
#include "ScanNSortDirectory.h"
#include<pthread.h>
//#include <glog/logging.h>
#include<map>
using namespace std;
//using namespace caffe;
//typedef void(*pGAFIS_ExtractBack)(unsigned char *bufferPix, int width, int height, int fgp, unsigned char * foreGroundBuffer);
//typedef void(*pGAFIS_ExtractMNT_All)(XGWMNTEXTRACTSTR *);

extern "C" void InitializeDll(void **pForeGroundBuffer, void **pRowColPairStruct, void **pHeatMapFloat, void **pCaffeNet, void **pHInstance, void **pQuality, char *pPrototxtPath, char *pCaffeModelPath);
extern "C" void GetQualityScore(void *pForeGroundBuffer, void *pRowColPairStruct, void *pHeatMapFloat, void *pCaffeNet, void *pHInstance, void *pQuality, unsigned char *srcBmp, int width, int height, int fgp, int Core_X, int Core_Y, int localSize, int stepSize, unsigned char modeNumber, float *QualityScore);
extern "C" void DestructDLL(void **pForeGroundBuffer, void **pRowColPairStruct, void **pHeatMapFloat, void **pCaffeNet, void **pHInstance, void **pQuality);

//extern "C" void MakeGrayBmp(unsigned char *ImageDataBuffer, int width, int height, bool normalOrader, const char * FullFilePath);
//extern "C" void MakeRGBBmp(unsigned char *ImageDataBuffer, int width, int height, bool normalOrder, const char * FullFilePath);

typedef struct
{
	int threadID;
	int threadNUM;
	file_lists *pFileList;
	map<string,float> *pResultMap;
	pthread_mutex_t *pResultMutex;
	char *pPrototxtPath;
	char *pCaffeModelPath;
        int mode;
} ParaStruct;

void * MultiThreadFunc(void *para)
{
	
	ParaStruct *pParaStruct = (ParaStruct *)para;
	void *pForeGroundBuffer, *pRowColPairStruct, *pHeatMapFloat, *pCaffeNet, *pHInstance, *pQuality;
	InitializeDll(&pForeGroundBuffer, &pRowColPairStruct, &pHeatMapFloat, &pCaffeNet, &pHInstance, &pQuality, pParaStruct->pPrototxtPath, pParaStruct->pCaffeModelPath);
	
	unsigned char * pImageBuffer = new unsigned char[640 * 640];
	unsigned char * pForegroundBuffer= new unsigned char[640 * 640];
	unsigned char * pMntBuffer = new unsigned char[640 * 640];
	unsigned char * pQualityBuffer= new unsigned char[640 * 640];
	string PersonID, FGP;
	int cRow, cCol;
	float QualityScore;
	const char *traceSampleEnv = getenv("CFIQ_TRACE_SAMPLE");
	const bool traceSample = traceSampleEnv != NULL && strcmp(traceSampleEnv, "1") == 0;
	//int mode = 3;
	for(file_lists::size_type i = pParaStruct->threadID; i < pParaStruct->pFileList->size(); i += pParaStruct->threadNUM)
	{
		if(traceSample)
		{
			fprintf(stderr, "[sample] begin %s\n", pParaStruct->pFileList->at(i).c_str());
		}
		fstream InFile(pParaStruct->pFileList->at(i), ios::in|ios::binary);
		InFile.seekg(1024 + 54, ios::beg);
		InFile.read((char *)pImageBuffer, 640 * 640);
		InFile.close();
		PersonID = pParaStruct->pFileList->at(i).substr(pParaStruct->pFileList->at(i).find_last_of('/') + 1, pParaStruct->pFileList->at(i).length() - pParaStruct->pFileList->at(i).find_last_of('/') - 1);
		FGP = pParaStruct->pFileList->at(i).substr(pParaStruct->pFileList->at(i).find_last_of('_') + 1, pParaStruct->pFileList->at(i).find_last_of('.') - pParaStruct->pFileList->at(i).find_last_of('_') - 1);
		if(traceSample)
		{
			fprintf(stderr, "[sample] extract-mnt %s\n", PersonID.c_str());
		}
		ExtractSingleRaw(pImageBuffer, 640, 640, atoi(FGP.c_str()), (FINGERMNTSTRUCT *)pMntBuffer);
		if(traceSample)
		{
			fprintf(stderr, "[sample] core-position %s\n", PersonID.c_str());
		}
		cRow = 0;
		cCol = 0;
		const int coreValid = GetCorePosition((FINGERMNTSTRUCT *)pMntBuffer, &cRow, &cCol);
		if(traceSample)
		{
			fprintf(stderr, "[sample] core-position-result %s valid=%d row=%d col=%d\n", PersonID.c_str(), coreValid, cRow, cCol);
		}
		if(traceSample)
		{
			fprintf(stderr, "[sample] quality-score %s\n", PersonID.c_str());
		}
        if(pParaStruct->mode == 6)
        {
            //MakeGrayBmp(pImageBuffer, 640, 640, true, string("./SourceBmpLinux/" + PersonID).c_str());
            GetQualityScore(pForeGroundBuffer, pRowColPairStruct, pHeatMapFloat, pCaffeNet, pHInstance, pQuality, pImageBuffer, 640, 640, atoi(FGP.c_str()), cCol, cRow, 48, 16, pParaStruct->mode, &QualityScore);
            //MakeRGBBmp((unsigned char *)pQuality, 640, 640, true, string("./QualityBmpLinux/" + PersonID).c_str());
            pthread_mutex_lock(pParaStruct->pResultMutex);
            pParaStruct->pResultMap->insert(pair<string,float>(PersonID, QualityScore));
            pthread_mutex_unlock(pParaStruct->pResultMutex);
        }
        else
        {
            MakeGrayBmp(pImageBuffer, 640, 640, true, string("./SourceBmpLinux/" + PersonID).c_str());
            GetQualityScore(pForeGroundBuffer, pRowColPairStruct, pHeatMapFloat, pCaffeNet, pHInstance, pQuality, pImageBuffer, 640, 640, atoi(FGP.c_str()), cCol, cRow, 48, 16, pParaStruct->mode, &QualityScore);
            MakeRGBBmp((unsigned char *)pQuality, 640, 640, true, string("./QualityBmpLinux/" + PersonID).c_str());
            pthread_mutex_lock(pParaStruct->pResultMutex);
            pParaStruct->pResultMap->insert(pair<string,float>(PersonID, QualityScore));
            pthread_mutex_unlock(pParaStruct->pResultMutex);
        }

		cout<< PersonID << '\t' << QualityScore << endl;
		if(traceSample)
		{
			fprintf(stderr, "[sample] done %s %.9g\n", PersonID.c_str(), QualityScore);
		}
	}
	delete[] pImageBuffer;
	delete[] pForegroundBuffer;
	delete[] pMntBuffer;
	delete[] pQualityBuffer;
	DestructDLL(&pForeGroundBuffer, &pRowColPairStruct, &pHeatMapFloat, &pCaffeNet, &pHInstance, &pQuality);
	return NULL;
}


int main(int argc, char **argv)
{
	//google::InitGoogleLogging("Test");
	//google::ShutdownGoogleLogging();
	if(argc != 6)
	{
		cout<<"传递参数："<<".prototxt文件全路径\t"<<".caffemodel文件全路径\t"<<"源图像目录（例如：..../SourceBmpLinuxTest36)"<<"\t"<<"模式号"<<"\t"<< "线程数"<<endl;
		return -1;
	}
	file_lists fileList = ScanNSortDirectory(argv[3], "bmp");
	const int threadCount = atoi(argv[5]);
	pthread_t thread_t[threadCount];
	ParaStruct paraStruct[threadCount];
	map<string,float> resultMap;
	pthread_mutex_t resultMutex = PTHREAD_MUTEX_INITIALIZER;
	const char *inlineThreadEnv = getenv("CFIQ_INLINE_THREAD");
	const bool runInline = inlineThreadEnv != NULL && strcmp(inlineThreadEnv, "1") == 0;
	for(int i = 0; i < threadCount; i++)
	{
		paraStruct[i].threadID = i;
		paraStruct[i].threadNUM = threadCount;
		paraStruct[i].pFileList = &fileList;
		paraStruct[i].pResultMap = &resultMap;
		paraStruct[i].pResultMutex = &resultMutex;
		paraStruct[i].pPrototxtPath = argv[1];
		paraStruct[i].pCaffeModelPath = argv[2];
		paraStruct[i].mode = atoi(argv[4]);

		if(runInline && threadCount == 1)
		{
			MultiThreadFunc((void *)(paraStruct + i));
			continue;
		}

		int ret = pthread_create(thread_t + i, NULL, MultiThreadFunc, (void *)(paraStruct + i));
		if(ret != 0)
		{
			cout<< "create thread error"<<endl;
		}
	}

	if(!runInline || threadCount != 1)
	{
		for(int j = 0; j < threadCount; j++)
		{
			pthread_join(thread_t[j], NULL);
		}
	}

	map<string,float>::iterator mapIt;
	fstream OutFile("Score_Linux.txt", ios::out);
	for(mapIt = resultMap.begin(); mapIt != resultMap.end(); mapIt++)
	{
		OutFile<< mapIt->first << "\t" << mapIt->second<<endl;
	}
	OutFile.close();
	pthread_mutex_destroy(&resultMutex);
	DestructMntExtractor();
	return 0;
}
