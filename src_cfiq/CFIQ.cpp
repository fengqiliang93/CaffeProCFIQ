/*
 * CFIQ.cpp
 *
 *  Created on: Mar 19, 2019
 *      Author: lyon
 */
#include<dlfcn.h>
#include<unistd.h>
#include<iostream>
#include<string>
#include<fstream>

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include"StructDef.h"
#include"InsideFunctionDef.h"
#include"caffe/net.hpp"
#include"CFIQ.h"
using namespace cv;
using namespace caffe;
using namespace std;
//typedef void(*pGAFIS_ExtractBack)(unsigned char *bufferPix, int width, int height, int fgp, unsigned char * foreGroundBuffer);
extern "C"  void GAFIS_ExtractBack(unsigned char *bufferPix, int width, int height, int fgp, unsigned char * foreGroundBuffer);

void  InitializeDll(void **pForeGroundBuffer, void **pRowColPairStruct, void **pHeatMapFloat, void **pCaffeNet, void **pHInstance, void **pQuality, char *pPrototxtPath, char *pCaffeModelPath)
{
	*pHeatMapFloat = new float[640 * 640 * 10];
	memset((unsigned char *)*pHeatMapFloat, 0, sizeof(float) * 640 * 640 * 10);
	*pForeGroundBuffer = new unsigned char[640 * 640];
	memset((unsigned char *)*pForeGroundBuffer, 0, 640 * 640);
	*pRowColPairStruct = new RowColPairStruct[640 * 640];
	memset((unsigned char *)*pRowColPairStruct, 0, 640 * 640);
	*pQuality = new unsigned char[640 * 640 * 3];

	//Caffe 初始化
	char *curDllPath = new char[PATH_MAX];
	memset(curDllPath, 0, PATH_MAX);

	int cnt = readlink("/proc/self/exe", curDllPath, PATH_MAX);
	if(cnt < 0 || cnt >= PATH_MAX)
	{
		printf("获取当前路径失败！\n");
		fgetc(stdin);
		exit(-1);
	}

	char *pPath = strrchr(curDllPath, '/');
	
        //有待改进
	#ifdef CPU_ONLY
	  Caffe::set_mode(Caffe::CPU); 
	#else
 	  Caffe::SetDevice(0);
   	  Caffe::set_mode(Caffe::GPU);
	#endif
    
	Net<float> *pNet = new(std::nothrow)Net<float>(pPrototxtPath, caffe::TEST);
	
	pNet->CopyTrainedLayersFrom(pCaffeModelPath);
	*pCaffeNet = pNet;

	*pPath = '\0';
	strcat(curDllPath, "/HeatMap.bin");
	fstream inFile(curDllPath, ios::in | ios::binary);
	if (!inFile)
	{
		cout << "HeatMap.bin Load Error" << endl;
		fgetc(stdin);
		exit(-1);
	}
	inFile.read((char *)*pHeatMapFloat, 640 * 640 * 4 * 10);
	inFile.close();

	*pPath = '\0';
	strcat(curDllPath, "/libnirvana-kernel.so.8.15.1");
	//void * pSoHandle = dlopen(curDllPath, RTLD_LAZY);
	//if (pSoHandle == NULL)
	//{
	//	//LOG(ERROR) << "Can not Load texfdll64_od.dll Library" << endl;
	//	printf("Load libnirvana-kernel.so.8.15.1 Error:(%s)\n", dlerror());
	//	fgetc(stdin);
	//	exit(-1);
	//}
	//*pHInstance = (void *)pSoHandle;
	*pHInstance = NULL;
	delete curDllPath;
}

void  DestructDLL(void **pForeGroundBuffer, void **pRowColPairStruct, void **pHeatMapFloat, void **pCaffeNet, void **pHInstance, void **pQuality)
{
	//google::ShutdownGoogleLogging();

	delete (unsigned char *)*pRowColPairStruct;
	*pRowColPairStruct = NULL;

	delete (unsigned char *)*pForeGroundBuffer;
	*pForeGroundBuffer = NULL;

	//delete[] (Net<float> *)(*pCaffeNet); Windows 上正确
	delete ((Net<float> *)*pCaffeNet); // Linux 上正确
	*pCaffeNet = NULL;
	delete (unsigned char *)*pHeatMapFloat;
	*pHeatMapFloat = NULL;

	delete (unsigned char *)*pQuality;
	*pQuality = NULL;

	//dlclose(*pHInstance);
	//FreeLibrary((HINSTANCE)*pHInstance);
}

/*
* （矫正条件下，用热图数据，算分数）（矫正后的质量图， 矫正后显示）
*/
void  GetQualityScoreModeOne(void *pForeground, void *pRowColPair, void *pHeatMapFloat, void *pCaffeNet, void *pHInstance, void *pQuality, unsigned char *srcBmp, int width, int height, int fgp, int Core_X, int Core_Y, int localSize, int stepSize, float *QualityScore)
{
	int x_correct = Core_X - 320;
	int y_correct = Core_Y - 256;

	if (pCaffeNet == NULL)
	{
		cout << "please pass parameter correctly! The Caffe Net Pointer Is NULL!" << endl;
		fgetc(stdin);
		return;
	}
	if (width > 640 || height > 640)
	{
		cout << "The Image Width OR Height Is Too Large!" << endl;
		fgetc(stdin);
		return;
	}

	Net<float> *pNet = (Net<float> *)pCaffeNet;
	unsigned char *pForegroundBuffer = (unsigned char *)pForeground;
	unsigned char *pQualityImageBuffer = (unsigned char *)pQuality;
	RowColPairStruct *pRowColPairStruct = (RowColPairStruct *)pRowColPair;


	//pGAFIS_ExtractBack GAFIS_ExtractBack = (pGAFIS_ExtractBack)dlsym(pHInstance, "GAFIS_ExtractBack");

	GAFIS_ExtractBack(srcBmp, width, height, fgp, pForegroundBuffer);

	cv::Mat srcBmpMat(width, height, CV_8UC1, srcBmp);
	//cv::imwrite(".\\srcbmp\\" + string(pPersonID) + "_" + to_string(fgp) + "_" + "src" + ".bmp", srcBmpMat);
	//cv::Mat_<float> srcBmpFloatMat = srcBmpMat * 1.0f / 255.0f;
	cv::Mat srcBmpFloatMat(width, height, CV_32F);
	srcBmpMat.convertTo(srcBmpFloatMat, CV_32F);
	srcBmpFloatMat = srcBmpFloatMat * 1.0f / 255.0f;


	cv::Mat ForegroundBufferMat(width, height, CV_8UC1, pForegroundBuffer);
	////cv::imwrite(".\\bmp\\" + string(pPersonID) + "_" + to_string(fgp) + "_" + "foreground" + ".bmp", ForegroundBufferMat * 255);
	int localRankNum = 0;

	GetCutInfo(ForegroundBufferMat.data, width, height, localSize, stepSize, pRowColPairStruct, &localRankNum);

	Blob<float> *input_blobs = pNet->input_blobs()[0];
	Blob<float> *output_blobs = pNet->output_blobs()[0];
	const float *pCpuData = output_blobs->cpu_data();

	Mat localRankMat(48, 48, CV_32FC1);
	Mat BadRankMat = cv::Mat::zeros(height, width, CV_8UC1);
	Mat MiddleRankMat = cv::Mat::zeros(height, width, CV_8UC1);
	Mat GoodRankMat = cv::Mat::zeros(height, width, CV_8UC1);
	Mat TotalRankMat = cv::Mat::zeros(height, width, CV_8UC1);
	cv::Mat_<float> QualityBmpMat(width, height, (float *)pHeatMapFloat + (fgp - 1) * 640 * 640);

	int rowMinPosition = 0, colMinPosition = 0;
	float heatMap = 0.0f, score = 0.0f;
	//fstream OutFile("rank_linux_test.txt", ios::out);
	
	for (int localRankCount = 0; localRankCount < localRankNum; localRankCount++)
	{
		rowMinPosition = pRowColPairStruct[localRankCount].RowID - y_correct;
		colMinPosition = pRowColPairStruct[localRankCount].ColID - x_correct;

		if (rowMinPosition >= 0 && rowMinPosition + localSize < height && colMinPosition >= 0 && colMinPosition + localSize < width)
		{

			localRankMat = srcBmpFloatMat(Range(pRowColPairStruct[localRankCount].RowID, pRowColPairStruct[localRankCount].RowID + localSize), Range(pRowColPairStruct[localRankCount].ColID, pRowColPairStruct[localRankCount].ColID + localSize)).clone();
						
			input_blobs->set_cpu_data((float *)(localRankMat.data));
			
			pNet->Forward();
			int  maxChannel = 0;
			for (int channelNum = 1; channelNum < output_blobs->channels(); channelNum++)
			{
				if (pCpuData[maxChannel] < pCpuData[channelNum])
					maxChannel = channelNum;
			}
			pRowColPairStruct[localRankCount].Rank = output_blobs->channels() - maxChannel;
			pRowColPairStruct[localRankCount].boolForecast = true;
			if (pRowColPairStruct[localRankCount].Rank == 1)
			{
				BadRankMat(Range(rowMinPosition, rowMinPosition + localSize), Range(colMinPosition, colMinPosition + localSize)) += 1;
				heatMap = cv::mean(QualityBmpMat(Range(rowMinPosition, rowMinPosition + localSize), Range(colMinPosition, colMinPosition + localSize)))[0];
				score += 1 * heatMap;
			}
			else if (pRowColPairStruct[localRankCount].Rank == 2)
			{
				MiddleRankMat(Range(rowMinPosition, rowMinPosition + localSize), Range(colMinPosition, colMinPosition + localSize)) += 1;
				heatMap = cv::mean(QualityBmpMat(Range(rowMinPosition, rowMinPosition + localSize), Range(colMinPosition, colMinPosition + localSize)))[0];
				score += 2 * heatMap;
			}
			else if (pRowColPairStruct[localRankCount].Rank == 3)
			{
				GoodRankMat(Range(rowMinPosition, rowMinPosition + localSize), Range(colMinPosition, colMinPosition + localSize)) += 1;
				heatMap = cv::mean(QualityBmpMat(Range(rowMinPosition, rowMinPosition + localSize), Range(colMinPosition, colMinPosition + localSize)))[0];
				score += 3 * heatMap;
			}
		}
	}
	//OutFile.close();
	//计算矫正后图像剩余大小，计算分数考虑前景面积所占比例的因素
	int y_BegRow = 0, y_EndRow = 0;
	int x_BegCol = 0, x_EndCol = 0;
	if (y_correct > 0)
	{
		y_BegRow = y_correct;
		y_EndRow = height;
	}
	else
	{
		y_BegRow = 0;
		y_EndRow = height + y_correct;
	}
	if (x_correct > 0)
	{
		x_BegCol = x_correct;
		x_EndCol = width;
	}
	else
	{
		x_BegCol = 0;
		x_EndCol = width + x_correct;
	}
	//float meanHeatMap = 0.0f;
	float meanHeatMap = cv::mean(QualityBmpMat(Range(y_BegRow, y_EndRow), Range(x_BegCol, x_EndCol)))[0];
	int TotalCut = ((y_EndRow - y_BegRow - localSize) / stepSize + 1 + 1) * ((x_EndCol - x_BegCol - localSize) / stepSize + 1 + 1); //适当增大切割数
	if (abs(meanHeatMap) > 1e-6 && TotalCut > 0)
	{
		*QualityScore = score / (3 * TotalCut * meanHeatMap);
		if (*QualityScore > 1.0f)
			*QualityScore = 1.0f;
	}
	else
	{
		*QualityScore = 0.0f;
	}

	TotalRankMat = BadRankMat + MiddleRankMat + GoodRankMat;
	Mat QualityMat = cv::Mat::zeros(height, width, CV_8UC3);
	for (int row = 0; row < height; row++)
	{
		for (int col = 0; col < width; col++)
		{
			if (BadRankMat.at<uchar>(row, col) > MiddleRankMat.at<uchar>(row, col) && BadRankMat.at<uchar>(row, col) > GoodRankMat.at<uchar>(row, col) && TotalRankMat.at<uchar>(row, col) > 0)
			{
				QualityMat.at<Vec3b>(row, col)[0] = 0;
				QualityMat.at<Vec3b>(row, col)[1] = 0;
				QualityMat.at<Vec3b>(row, col)[2] = 255;

			}
			else if ((BadRankMat.at<uchar>(row, col) <= MiddleRankMat.at<uchar>(row, col) || BadRankMat.at<uchar>(row, col) <= GoodRankMat.at<uchar>(row, col)) && TotalRankMat.at<uchar>(row, col) > 0)
			{
				QualityMat.at<Vec3b>(row, col)[0] = 255;
				QualityMat.at<Vec3b>(row, col)[1] = 255;
				QualityMat.at<Vec3b>(row, col)[2] = 255;
			}
			else{}
		}
	}
	for (int localRankCount = 0; localRankCount < localRankNum; localRankCount++)
	{

		if (pRowColPairStruct[localRankCount].boolForecast == true)
		{
			rowMinPosition = pRowColPairStruct[localRankCount].RowID - y_correct;
			colMinPosition = pRowColPairStruct[localRankCount].ColID - x_correct;
			rectangle(QualityMat, Point(colMinPosition, rowMinPosition), Point(colMinPosition + localSize, rowMinPosition + localSize), Scalar(0, 0, 0), 1);
		}

	}
	memcpy(pQualityImageBuffer, QualityMat.data, width * height * 3);
}

/*
* （矫正条件下，用热图数据，算分数）（矫正后的质量图， 原位置不加矫正显示）
*/
void  GetQualityScoreModeTwo(void *pForeground, void *pRowColPair, void *pHeatMapFloat, void *pCaffeNet, void *pHInstance, void *pQuality, unsigned char *srcBmp, int width, int height, int fgp, int Core_X, int Core_Y, int localSize, int stepSize, float *QualityScore)
{
	int x_correct = Core_X - 320;
	int y_correct = Core_Y - 256;

	if (pCaffeNet == NULL)
	{
		cout << "please pass parameter correctly! The Caffe Net Pointer Is NULL!" << endl;
		fgetc(stdin);
		return;
	}
	if (width > 640 || height > 640)
	{
		cout << "The Image Width OR Height Is Too Large!" << endl;
		fgetc(stdin);
		return;
	}

	Net<float> *pNet = (Net<float> *)pCaffeNet;
	unsigned char *pForegroundBuffer = (unsigned char *)pForeground;
	unsigned char *pQualityImageBuffer = (unsigned char *)pQuality;
	RowColPairStruct *pRowColPairStruct = (RowColPairStruct *)pRowColPair;


	//pGAFIS_ExtractBack GAFIS_ExtractBack = (pGAFIS_ExtractBack)dlsym(pHInstance, "GAFIS_ExtractBack");

	GAFIS_ExtractBack(srcBmp, width, height, fgp, pForegroundBuffer);

	cv::Mat srcBmpMat(width, height, CV_8UC1, srcBmp);
	//cv::imwrite(".\\srcbmp\\" + string(pPersonID) + "_" + to_string(fgp) + "_" + "src" + ".bmp", srcBmpMat);
	//cv::Mat_<float> srcBmpFloatMat = srcBmpMat * 1.0f / 255.0f;
	cv::Mat srcBmpFloatMat(width, height, CV_32FC1);
	srcBmpMat.convertTo(srcBmpFloatMat, CV_32FC1);
	srcBmpFloatMat = srcBmpFloatMat * 1.0f / 255.0f;


	cv::Mat ForegroundBufferMat(width, height, CV_8UC1, pForegroundBuffer);
	//cv::imwrite(".\\bmp\\" + string(pPersonID) + "_" + to_string(fgp) + "_" + "foreground" + ".bmp", ForegroundBufferMat * 255);
	int localRankNum = 0;

	GetCutInfo(ForegroundBufferMat.data, width, height, localSize, stepSize, pRowColPairStruct, &localRankNum);

	Blob<float> *input_blobs = pNet->input_blobs()[0];
	Blob<float> *output_blobs = pNet->output_blobs()[0];
	const float *pCpuData = output_blobs->cpu_data();

	Mat localRankMat(48, 48, CV_32FC1);
	Mat BadRankMat = cv::Mat::zeros(height, width, CV_8UC1);
	Mat MiddleRankMat = cv::Mat::zeros(height, width, CV_8UC1);
	Mat GoodRankMat = cv::Mat::zeros(height, width, CV_8UC1);
	Mat TotalRankMat = cv::Mat::zeros(height, width, CV_8UC1);
	cv::Mat_<float> QualityBmpMat(width, height, (float *)pHeatMapFloat + (fgp - 1) * 640 * 640);

	int rowMinPosition = 0, colMinPosition = 0;
	float heatMap = 0.0f, score = 0.0f;

	for (int localRankCount = 0; localRankCount < localRankNum; localRankCount++)
	{
		rowMinPosition = pRowColPairStruct[localRankCount].RowID - y_correct;
		colMinPosition = pRowColPairStruct[localRankCount].ColID - x_correct;

		if (rowMinPosition >= 0 && rowMinPosition + localSize < height && colMinPosition >= 0 && colMinPosition + localSize < width)
		{

			localRankMat = srcBmpFloatMat(Range(pRowColPairStruct[localRankCount].RowID, pRowColPairStruct[localRankCount].RowID + localSize), Range(pRowColPairStruct[localRankCount].ColID, pRowColPairStruct[localRankCount].ColID + localSize)).clone();

			input_blobs->set_cpu_data((float *)localRankMat.data);
			pNet->Forward();
			int  maxChannel = 0;
			for (int channelNum = 1; channelNum < output_blobs->channels(); channelNum++)
			{
				if (pCpuData[maxChannel] < pCpuData[channelNum])
					maxChannel = channelNum;
			}
			pRowColPairStruct[localRankCount].Rank = output_blobs->channels() - maxChannel;
			pRowColPairStruct[localRankCount].boolForecast = true;
			if (pRowColPairStruct[localRankCount].Rank == 1)
			{
				BadRankMat(Range(pRowColPairStruct[localRankCount].RowID, pRowColPairStruct[localRankCount].RowID + localSize), Range(pRowColPairStruct[localRankCount].ColID, pRowColPairStruct[localRankCount].ColID + localSize)) += 1;
				heatMap = cv::mean(QualityBmpMat(Range(rowMinPosition, rowMinPosition + localSize), Range(colMinPosition, colMinPosition + localSize)))[0];
				score += 1 * heatMap;
			}
			else if (pRowColPairStruct[localRankCount].Rank == 2)
			{
				MiddleRankMat(Range(pRowColPairStruct[localRankCount].RowID, pRowColPairStruct[localRankCount].RowID + localSize), Range(pRowColPairStruct[localRankCount].ColID, pRowColPairStruct[localRankCount].ColID + localSize)) += 1;
				heatMap = cv::mean(QualityBmpMat(Range(rowMinPosition, rowMinPosition + localSize), Range(colMinPosition, colMinPosition + localSize)))[0];
				score += 2 * heatMap;
			}
			else if (pRowColPairStruct[localRankCount].Rank == 3)
			{
				GoodRankMat(Range(pRowColPairStruct[localRankCount].RowID, pRowColPairStruct[localRankCount].RowID + localSize), Range(pRowColPairStruct[localRankCount].ColID, pRowColPairStruct[localRankCount].ColID + localSize)) += 1;
				heatMap = cv::mean(QualityBmpMat(Range(rowMinPosition, rowMinPosition + localSize), Range(colMinPosition, colMinPosition + localSize)))[0];
				score += 3 * heatMap;
			}
		}
	}
	//计算矫正后图像剩余大小，计算分数考虑前景面积所占比例的因素
	int y_BegRow = 0, y_EndRow = 0;
	int x_BegCol = 0, x_EndCol = 0;
	if (y_correct > 0)
	{
		y_BegRow = y_correct;
		y_EndRow = height;
	}
	else
	{
		y_BegRow = 0;
		y_EndRow = height + y_correct;
	}
	if (x_correct > 0)
	{
		x_BegCol = x_correct;
		x_EndCol = width;
	}
	else
	{
		x_BegCol = 0;
		x_EndCol = width + x_correct;
	}
	//float meanHeatMap = 0.0f;
	float meanHeatMap = cv::mean(QualityBmpMat(Range(y_BegRow, y_EndRow), Range(x_BegCol, x_EndCol)))[0];
	int TotalCut = ((y_EndRow - y_BegRow - localSize) / stepSize + 1 + 1) * ((x_EndCol - x_BegCol - localSize) / stepSize + 1 + 1); //适当增大切割数 
	if (abs(meanHeatMap) > 1e-6 && TotalCut > 0)
	{
		*QualityScore = score / (3 * TotalCut * meanHeatMap);
		if (*QualityScore > 1.0f)
			*QualityScore = 1.0f;
	}
	else
	{
		*QualityScore = 0.0f;
	}

	TotalRankMat = BadRankMat + MiddleRankMat + GoodRankMat;
	Mat QualityMat = cv::Mat::zeros(height, width, CV_8UC3);
	for (int row = 0; row < height; row++)
	{
		for (int col = 0; col < width; col++)
		{
			if (BadRankMat.at<uchar>(row, col) > MiddleRankMat.at<uchar>(row, col) && BadRankMat.at<uchar>(row, col) > GoodRankMat.at<uchar>(row, col) && TotalRankMat.at<uchar>(row, col) > 0)
			{
				QualityMat.at<Vec3b>(row, col)[0] = 0;
				QualityMat.at<Vec3b>(row, col)[1] = 0;
				QualityMat.at<Vec3b>(row, col)[2] = 255;

			}
			else if ((BadRankMat.at<uchar>(row, col) <= MiddleRankMat.at<uchar>(row, col) || BadRankMat.at<uchar>(row, col) <= GoodRankMat.at<uchar>(row, col)) && TotalRankMat.at<uchar>(row, col) > 0)
			{
				QualityMat.at<Vec3b>(row, col)[0] = 255;
				QualityMat.at<Vec3b>(row, col)[1] = 255;
				QualityMat.at<Vec3b>(row, col)[2] = 255;
			}
			else{}
		}
	}
	for (int localRankCount = 0; localRankCount < localRankNum; localRankCount++)
	{

		if (pRowColPairStruct[localRankCount].boolForecast == true)
		{
			//rowMinPosition = pRowColPairStruct[localRankCount].RowID - y_correct;
			//colMinPosition = pRowColPairStruct[localRankCount].ColID - x_correct;
			rectangle(QualityMat, Point(pRowColPairStruct[localRankCount].ColID, pRowColPairStruct[localRankCount].RowID), Point(pRowColPairStruct[localRankCount].ColID + localSize, pRowColPairStruct[localRankCount].RowID + localSize), Scalar(0, 0, 0), 1);
		}

	}
	memcpy(pQualityImageBuffer, QualityMat.data, width * height * 3);
}



/*
* （矫正条件下，用热图数据，算分数）（画完整前景质量图, 原位置不加矫正显示）
*/
void  GetQualityScoreModeThree(void *pForeground, void *pRowColPair, void *pHeatMapFloat, void *pCaffeNet, void *pHInstance, void *pQuality, unsigned char *srcBmp, int width, int height, int fgp, int Core_X, int Core_Y, int localSize, int stepSize, float *QualityScore)
{
	int x_correct = Core_X - 320;
	int y_correct = Core_Y - 256;

	if (pCaffeNet == NULL)
	{
		cout << "please pass parameter correctly! The Caffe Net Pointer Is NULL!" << endl;
		fgetc(stdin);
		return;
	}
	if (width > 640 || height > 640)
	{
		cout << "The Image Width OR Height Is Too Large!" << endl;
		fgetc(stdin);
		return;
	}

	Net<float> *pNet = (Net<float> *)pCaffeNet;
	unsigned char *pForegroundBuffer = (unsigned char *)pForeground;
	unsigned char *pQualityImageBuffer = (unsigned char *)pQuality;
	RowColPairStruct *pRowColPairStruct = (RowColPairStruct *)pRowColPair;


	//pGAFIS_ExtractBack GAFIS_ExtractBack = (pGAFIS_ExtractBack)dlsym(pHInstance, "GAFIS_ExtractBack");

	GAFIS_ExtractBack(srcBmp, width, height, fgp, pForegroundBuffer);

	cv::Mat srcBmpMat(width, height, CV_8UC1, srcBmp);
	//cv::imwrite(".\\srcbmp\\" + string(pPersonID) + "_" + to_string(fgp) + "_" + "src" + ".bmp", srcBmpMat);
	//cv::Mat_<float> srcBmpFloatMat = srcBmpMat * 1.0f / 255.0f;
	cv::Mat srcBmpFloatMat(width, height, CV_32FC1);
	srcBmpMat.convertTo(srcBmpFloatMat, CV_32FC1);
	srcBmpFloatMat = srcBmpFloatMat * 1.0f / 255.0f;


	cv::Mat ForegroundBufferMat(width, height, CV_8UC1, pForegroundBuffer);
	//cv::imwrite(".\\bmp\\" + string(pPersonID) + "_" + to_string(fgp) + "_" + "foreground" + ".bmp", ForegroundBufferMat * 255);
	int localRankNum = 0;

	GetCutInfo(ForegroundBufferMat.data, width, height, localSize, stepSize, pRowColPairStruct, &localRankNum);

	Blob<float> *input_blobs = pNet->input_blobs()[0];
	Blob<float> *output_blobs = pNet->output_blobs()[0];
		
	Mat localRankMat(48, 48, CV_32FC1);
	Mat BadRankMat = cv::Mat::zeros(height, width, CV_8UC1);
	Mat MiddleRankMat = cv::Mat::zeros(height, width, CV_8UC1);
	Mat GoodRankMat = cv::Mat::zeros(height, width, CV_8UC1);
	Mat TotalRankMat = cv::Mat::zeros(height, width, CV_8UC1);
	cv::Mat_<float> QualityBmpMat(width, height, (float *)pHeatMapFloat + (fgp - 1) * 640 * 640);

	int rowMinPosition = 0, colMinPosition = 0;
	float heatMap = 0.0f, score = 0.0f;
	
	for (int localRankCount = 0; localRankCount < localRankNum; localRankCount++)
	{
		rowMinPosition = pRowColPairStruct[localRankCount].RowID - y_correct;
		colMinPosition = pRowColPairStruct[localRankCount].ColID - x_correct;

		localRankMat = srcBmpFloatMat(Range(pRowColPairStruct[localRankCount].RowID, pRowColPairStruct[localRankCount].RowID + localSize), Range(pRowColPairStruct[localRankCount].ColID, pRowColPairStruct[localRankCount].ColID + localSize)).clone();
		
 		//printf("%ld\n",reinterpret_cast<long int>((float *)localRankMat.data));
		//printf("%p\n",localRankMat.data);
		input_blobs->set_cpu_data((float *)localRankMat.data);
		pNet->Forward();
       
		const float *pCpuData = output_blobs->cpu_data();
		int  maxChannel = 0;
		for (int channelNum = 1; channelNum < output_blobs->channels(); channelNum++)
		{	
			if (pCpuData[maxChannel] < pCpuData[channelNum])
				maxChannel = channelNum;
		}
	
		pRowColPairStruct[localRankCount].Rank = output_blobs->channels() - maxChannel;
		pRowColPairStruct[localRankCount].boolForecast = true;

		if (rowMinPosition >= 0 && rowMinPosition + localSize < height && colMinPosition >= 0 && colMinPosition + localSize < width)
		{
			if (pRowColPairStruct[localRankCount].Rank == 1)
			{
				heatMap = cv::mean(QualityBmpMat(Range(rowMinPosition, rowMinPosition + localSize), Range(colMinPosition, colMinPosition + localSize)))[0];
				score += 1 * heatMap;
			}
			else if (pRowColPairStruct[localRankCount].Rank == 2)
			{
				heatMap = cv::mean(QualityBmpMat(Range(rowMinPosition, rowMinPosition + localSize), Range(colMinPosition, colMinPosition + localSize)))[0];
				score += 2 * heatMap;
			}
			else if (pRowColPairStruct[localRankCount].Rank == 3)
			{
				heatMap = cv::mean(QualityBmpMat(Range(rowMinPosition, rowMinPosition + localSize), Range(colMinPosition, colMinPosition + localSize)))[0];
				score += 3 * heatMap;
			}
		}
		if (pRowColPairStruct[localRankCount].Rank == 1)
		{
			BadRankMat(Range(pRowColPairStruct[localRankCount].RowID, pRowColPairStruct[localRankCount].RowID + localSize), Range(pRowColPairStruct[localRankCount].ColID, pRowColPairStruct[localRankCount].ColID + localSize)) += 1;
		}
		else if (pRowColPairStruct[localRankCount].Rank == 2)
		{
			MiddleRankMat(Range(pRowColPairStruct[localRankCount].RowID, pRowColPairStruct[localRankCount].RowID + localSize), Range(pRowColPairStruct[localRankCount].ColID, pRowColPairStruct[localRankCount].ColID + localSize)) += 1;
		}
		else if (pRowColPairStruct[localRankCount].Rank == 3)
		{
			GoodRankMat(Range(pRowColPairStruct[localRankCount].RowID, pRowColPairStruct[localRankCount].RowID + localSize), Range(pRowColPairStruct[localRankCount].ColID, pRowColPairStruct[localRankCount].ColID + localSize)) += 1;
		}
	}
	//计算矫正后图像剩余大小，计算分数考虑前景面积所占比例的因素
	int y_BegRow = 0, y_EndRow = 0;
	int x_BegCol = 0, x_EndCol = 0;
	if (y_correct > 0)
	{
		y_BegRow = y_correct;
		y_EndRow = height;
	}
	else
	{
		y_BegRow = 0;
		y_EndRow = height + y_correct;
	}
	if (x_correct > 0)
	{
		x_BegCol = x_correct;
		x_EndCol = width;
	}
	else
	{
		x_BegCol = 0;
		x_EndCol = width + x_correct;
	}
	//float meanHeatMap = 0.0f;
	float meanHeatMap = cv::mean(QualityBmpMat(Range(y_BegRow, y_EndRow), Range(x_BegCol, x_EndCol)))[0];
	int TotalCut = ((y_EndRow - y_BegRow - localSize) / stepSize + 1 + 1) * ((x_EndCol - x_BegCol - localSize) / stepSize + 1 + 1); //适当增大切割数 
	if (abs(meanHeatMap) > 1e-6 && TotalCut > 0)
	{
		*QualityScore = score / (3 * TotalCut * meanHeatMap);
		if (*QualityScore > 1.0f)
			*QualityScore = 1.0f;
	}
	else
	{
		*QualityScore = 0.0f;
	}

	TotalRankMat = BadRankMat + MiddleRankMat + GoodRankMat;
	Mat QualityMat = cv::Mat::zeros(height, width, CV_8UC3);
	for (int row = 0; row < height; row++)
	{
		for (int col = 0; col < width; col++)
		{
			if (BadRankMat.at<uchar>(row, col) > MiddleRankMat.at<uchar>(row, col) && BadRankMat.at<uchar>(row, col) > GoodRankMat.at<uchar>(row, col) && TotalRankMat.at<uchar>(row, col) > 0)
			{
				QualityMat.at<Vec3b>(row, col)[0] = 0;
				QualityMat.at<Vec3b>(row, col)[1] = 0;
				QualityMat.at<Vec3b>(row, col)[2] = 255;

			}
			else if ((BadRankMat.at<uchar>(row, col) <= MiddleRankMat.at<uchar>(row, col) || BadRankMat.at<uchar>(row, col) <= GoodRankMat.at<uchar>(row, col)) && TotalRankMat.at<uchar>(row, col) > 0)
			{
				QualityMat.at<Vec3b>(row, col)[0] = 255;
				QualityMat.at<Vec3b>(row, col)[1] = 255;
				QualityMat.at<Vec3b>(row, col)[2] = 255;
			}
			else{}
		}
	}
	for (int localRankCount = 0; localRankCount < localRankNum; localRankCount++)
	{

		if (pRowColPairStruct[localRankCount].boolForecast == true)
		{
			rectangle(QualityMat, Point(pRowColPairStruct[localRankCount].ColID, pRowColPairStruct[localRankCount].RowID), Point(pRowColPairStruct[localRankCount].ColID + localSize, pRowColPairStruct[localRankCount].RowID + localSize), Scalar(0, 0, 0), 1);
		}
	}
	memcpy(pQualityImageBuffer, QualityMat.data, width * height * 3);
}

void GetQualityScoreModeSix(void *pForeground, void *pRowColPair, void *pHeatMapFloat, void *pCaffeNet, void *pHInstance, void *pQuality, unsigned char *srcBmp, int width, int height, int fgp, int Core_X, int Core_Y, int localSize, int stepSize, float *QualityScore)
{
		int x_correct = Core_X - 320;
		int y_correct = Core_Y - 256;

		if (pCaffeNet == NULL)
		{
			cout << "please pass parameter correctly! The Caffe Net Pointer Is NULL!" << endl;
			fgetc(stdin);
			return;
		}
		if (width > 640 || height > 640)
		{
			cout << "The Image Width OR Height Is Too Large!" << endl;
			fgetc(stdin);
			return;
		}

		Net<float> *pNet = (Net<float> *)pCaffeNet;
		unsigned char *pForegroundBuffer = (unsigned char *)pForeground;
		unsigned char *pQualityImageBuffer = (unsigned char *)pQuality;
		RowColPairStruct *pRowColPairStruct = (RowColPairStruct *)pRowColPair;


		//pGAFIS_ExtractBack GAFIS_ExtractBack = (pGAFIS_ExtractBack)dlsym(pHInstance, "GAFIS_ExtractBack");

		GAFIS_ExtractBack(srcBmp, width, height, fgp, pForegroundBuffer);

		cv::Mat srcBmpMat(width, height, CV_8UC1, srcBmp);
		//cv::imwrite(".\\srcbmp\\" + string(pPersonID) + "_" + to_string(fgp) + "_" + "src" + ".bmp", srcBmpMat);
		//cv::Mat_<float> srcBmpFloatMat = srcBmpMat * 1.0f / 255.0f;
		cv::Mat srcBmpFloatMat(width, height, CV_32FC1);
		srcBmpMat.convertTo(srcBmpFloatMat, CV_32FC1);
		srcBmpFloatMat = srcBmpFloatMat * 1.0f / 255.0f;


		cv::Mat ForegroundBufferMat(width, height, CV_8UC1, pForegroundBuffer);
		//cv::imwrite(".\\bmp\\" + string(pPersonID) + "_" + to_string(fgp) + "_" + "foreground" + ".bmp", ForegroundBufferMat * 255);
		int localRankNum = 0;

		GetCutInfo(ForegroundBufferMat.data, width, height, localSize, stepSize, pRowColPairStruct, &localRankNum);

		Blob<float> *input_blobs = pNet->input_blobs()[0];
		Blob<float> *output_blobs = pNet->output_blobs()[0];
		const float *pCpuData = output_blobs->cpu_data();

		Mat localRankMat(48, 48, CV_32FC1);
		Mat BadRankMat = cv::Mat::zeros(height, width, CV_8UC1);
		Mat MiddleRankMat = cv::Mat::zeros(height, width, CV_8UC1);
		Mat GoodRankMat = cv::Mat::zeros(height, width, CV_8UC1);
		Mat TotalRankMat = cv::Mat::zeros(height, width, CV_8UC1);
		cv::Mat_<float> QualityBmpMat(width, height, (float *)pHeatMapFloat + (fgp - 1) * 640 * 640);

		int rowMinPosition = 0, colMinPosition = 0;
		float heatMap = 0.0f, score = 0.0f;

		for (int localRankCount = 0; localRankCount < localRankNum; localRankCount++)
		{
			rowMinPosition = pRowColPairStruct[localRankCount].RowID - y_correct;
			colMinPosition = pRowColPairStruct[localRankCount].ColID - x_correct;

			localRankMat = srcBmpFloatMat(Range(pRowColPairStruct[localRankCount].RowID, pRowColPairStruct[localRankCount].RowID + localSize), Range(pRowColPairStruct[localRankCount].ColID, pRowColPairStruct[localRankCount].ColID + localSize)).clone();
			input_blobs->set_cpu_data((float *)localRankMat.data);
			pNet->Forward();
			int  maxChannel = 0;
			for (int channelNum = 1; channelNum < output_blobs->channels(); channelNum++)
			{
				if (pCpuData[maxChannel] < pCpuData[channelNum])
					maxChannel = channelNum;
			}
			pRowColPairStruct[localRankCount].Rank = output_blobs->channels() - maxChannel;
			pRowColPairStruct[localRankCount].boolForecast = true;

			if (rowMinPosition >= 0 && rowMinPosition + localSize < height && colMinPosition >= 0 && colMinPosition + localSize < width)
			{
				if (pRowColPairStruct[localRankCount].Rank == 1)
				{
					heatMap = cv::mean(QualityBmpMat(Range(rowMinPosition, rowMinPosition + localSize), Range(colMinPosition, colMinPosition + localSize)))[0];
					score += 1 * heatMap;
				}
				else if (pRowColPairStruct[localRankCount].Rank == 2)
				{
					heatMap = cv::mean(QualityBmpMat(Range(rowMinPosition, rowMinPosition + localSize), Range(colMinPosition, colMinPosition + localSize)))[0];
					score += 2 * heatMap;
				}
				else if (pRowColPairStruct[localRankCount].Rank == 3)
				{
					heatMap = cv::mean(QualityBmpMat(Range(rowMinPosition, rowMinPosition + localSize), Range(colMinPosition, colMinPosition + localSize)))[0];
					score += 3 * heatMap;
				}
			}
			if (pRowColPairStruct[localRankCount].Rank == 1)
			{
				BadRankMat(Range(pRowColPairStruct[localRankCount].RowID, pRowColPairStruct[localRankCount].RowID + localSize), Range(pRowColPairStruct[localRankCount].ColID, pRowColPairStruct[localRankCount].ColID + localSize)) += 1;
			}
			else if (pRowColPairStruct[localRankCount].Rank == 2)
			{
				MiddleRankMat(Range(pRowColPairStruct[localRankCount].RowID, pRowColPairStruct[localRankCount].RowID + localSize), Range(pRowColPairStruct[localRankCount].ColID, pRowColPairStruct[localRankCount].ColID + localSize)) += 1;
			}
			else if (pRowColPairStruct[localRankCount].Rank == 3)
			{
				GoodRankMat(Range(pRowColPairStruct[localRankCount].RowID, pRowColPairStruct[localRankCount].RowID + localSize), Range(pRowColPairStruct[localRankCount].ColID, pRowColPairStruct[localRankCount].ColID + localSize)) += 1;
			}
		}
		//计算矫正后图像剩余大小，计算分数考虑前景面积所占比例的因素
		int y_BegRow = 0, y_EndRow = 0;
		int x_BegCol = 0, x_EndCol = 0;
		if (y_correct > 0)
		{
			y_BegRow = y_correct;
			y_EndRow = height;
		}
		else
		{
			y_BegRow = 0;
			y_EndRow = height + y_correct;
		}
		if (x_correct > 0)
		{
			x_BegCol = x_correct;
			x_EndCol = width;
		}
		else
		{
			x_BegCol = 0;
			x_EndCol = width + x_correct;
		}
		//float meanHeatMap = 0.0f;
		float meanHeatMap = cv::mean(QualityBmpMat(Range(y_BegRow, y_EndRow), Range(x_BegCol, x_EndCol)))[0];
		int TotalCut = ((y_EndRow - y_BegRow - localSize) / stepSize + 1 + 1) * ((x_EndCol - x_BegCol - localSize) / stepSize + 1 + 1); //适当增大切割数
		if (abs(meanHeatMap) > 1e-6 && TotalCut > 0)
		{
			*QualityScore = score / (3 * TotalCut * meanHeatMap);
			if (*QualityScore > 1.0f)
				*QualityScore = 1.0f;
		}
		else
		{
			*QualityScore = 0.0f;
		}
}

void  GetQualityScore(void *pForeground, void *pRowColPair, void *pHeatMapFloat, void *pCaffeNet, void *pHInstance, void *pQuality, unsigned char *srcBmp, int width, int height, int fgp, int Core_X, int Core_Y, int localSize, int stepSize, unsigned char modeNumber, float *QualityScore)
{
	if (modeNumber == 1)
	{
		GetQualityScoreModeOne(pForeground, pRowColPair, pHeatMapFloat, pCaffeNet, pHInstance, pQuality, srcBmp, width, height, fgp, Core_X, Core_Y, localSize, stepSize, QualityScore);
	}
	else if (modeNumber == 2)
	{
		GetQualityScoreModeTwo(pForeground, pRowColPair, pHeatMapFloat, pCaffeNet, pHInstance, pQuality, srcBmp, width, height, fgp, Core_X, Core_Y, localSize, stepSize, QualityScore);
	}
	else if (modeNumber == 3)
	{
		GetQualityScoreModeThree(pForeground, pRowColPair, pHeatMapFloat, pCaffeNet, pHInstance, pQuality, srcBmp, width, height, fgp, Core_X, Core_Y, localSize, stepSize, QualityScore);
	}
	else if(modeNumber == 6)
	{
		GetQualityScoreModeSix(pForeground, pRowColPair, pHeatMapFloat, pCaffeNet, pHInstance, pQuality, srcBmp, width, height, fgp, Core_X, Core_Y, localSize, stepSize, QualityScore);
	}
}

void __attribute__((constructor)) before_main(void)
{

}
void __attribute__((destructor)) after_main(void)
{

}
