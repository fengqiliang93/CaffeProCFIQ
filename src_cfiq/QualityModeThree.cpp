/*
 * CFIQ.cpp
 *
 *  Created on: Mar 19, 2019
 *      Author: lyon
 */
#include<iostream>
#include<string>
#include<algorithm>
#include<cstdlib>
#include<cstring>
#include<vector>

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include"ForegroundExtractor.h"
#include"StructDef.h"
#include"InsideFunctionDef.h"
#include"QualityScoreUtils.h"
#include"caffe/net.hpp"
#include"CFIQ.h"
using namespace cv;
using namespace caffe;
using namespace std;

static bool TraceQualityModeThree()
{
	const char *traceQualityEnv = getenv("CFIQ_TRACE_QUALITY3");
	return traceQualityEnv != NULL && strcmp(traceQualityEnv, "1") == 0;
}

static void CopyPatchToBufferModeThree(const cv::Mat &srcBmpFloatMat,
                                       int row,
                                       int col,
                                       int localSize,
                                       float *dst)
{
	for (int r = 0; r < localSize; ++r)
	{
		const float *srcRow = srcBmpFloatMat.ptr<float>(row + r) + col;
		memcpy(dst + r * localSize, srcRow, sizeof(float) * localSize);
	}
}

static void DrawPatchGridModeThree(cv::Mat &qualityMat,
                                   const RowColPairStruct *rowColPairs,
                                   int localRankNum,
                                   int localSize)
{
	const int width = qualityMat.cols;
	const int height = qualityMat.rows;

	for (int localRankCount = 0; localRankCount < localRankNum; localRankCount++)
	{
		if (!rowColPairs[localRankCount].boolForecast)
			continue;

		const int col = static_cast<int>(rowColPairs[localRankCount].ColID);
		const int row = static_cast<int>(rowColPairs[localRankCount].RowID);
		const int x0 = std::max(0, col);
		const int y0 = std::max(0, row);
		const int x1 = std::min(width - 1, col + localSize);
		const int y1 = std::min(height - 1, row + localSize);
		if (x0 > x1 || y0 > y1)
			continue;

		memset(qualityMat.ptr<uchar>(y0) + x0 * 3, 0, (x1 - x0 + 1) * 3);
		memset(qualityMat.ptr<uchar>(y1) + x0 * 3, 0, (x1 - x0 + 1) * 3);
		for (int y = y0; y <= y1; y++)
		{
			uchar *rowPtr = qualityMat.ptr<uchar>(y);
			memset(rowPtr + x0 * 3, 0, 3);
			memset(rowPtr + x1 * 3, 0, 3);
		}
	}
}

/*
* （矫正条件下，用热图数据，算分数）（画完整前景质量图, 原位置不加矫正显示）
*/
void  GetQualityScoreModeThree(void *pForeground, void *pRowColPair, void *pHeatMapFloat, void *pCaffeNet, void *pHInstance, void *pQuality, unsigned char *srcBmp, int width, int height, int fgp, int Core_X, int Core_Y, int localSize, int stepSize, float *QualityScore)
{
	const bool traceQuality = TraceQualityModeThree();

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
	*QualityScore = 0.0f;
	memset(pQualityImageBuffer, 0, width * height * 3);
	memset(pRowColPairStruct, 0, sizeof(RowColPairStruct) * width * height);


	if (!ExtractForeground(pHInstance, srcBmp, width, height, fgp, pForegroundBuffer))
		return;
	CFIQUseForegroundCenterWhenCoreMissing(pForegroundBuffer, width, height, &Core_X, &Core_Y);
	int x_correct = Core_X - 320;
	int y_correct = Core_Y - 256;
	
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
	if (traceQuality)
	{
		fprintf(stderr, "[quality-three] core=(%d,%d) corrected=(%d,%d) localRankNum=%d\n", Core_X, Core_Y, x_correct, y_correct, localRankNum);
	}

	Blob<float> *input_blobs = pNet->input_blobs()[0];
	Blob<float> *output_blobs = pNet->output_blobs()[0];
		
	std::vector<float> localRankBuffer(localSize * localSize);
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

		CopyPatchToBufferModeThree(srcBmpFloatMat,
		                           pRowColPairStruct[localRankCount].RowID,
		                           pRowColPairStruct[localRankCount].ColID,
		                           localSize,
		                           &localRankBuffer[0]);
		input_blobs->set_cpu_data(&localRankBuffer[0]);
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
				score += CFIQRankWeight(pRowColPairStruct[localRankCount].Rank) * heatMap;
			}
			else if (pRowColPairStruct[localRankCount].Rank == 2)
			{
				heatMap = cv::mean(QualityBmpMat(Range(rowMinPosition, rowMinPosition + localSize), Range(colMinPosition, colMinPosition + localSize)))[0];
				score += CFIQRankWeight(pRowColPairStruct[localRankCount].Rank) * heatMap;
			}
			else if (pRowColPairStruct[localRankCount].Rank == 3)
			{
				heatMap = cv::mean(QualityBmpMat(Range(rowMinPosition, rowMinPosition + localSize), Range(colMinPosition, colMinPosition + localSize)))[0];
				score += CFIQRankWeight(pRowColPairStruct[localRankCount].Rank) * heatMap;
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
	TotalCut = std::max(TotalCut, localRankNum);
	if (abs(meanHeatMap) > 1e-6 && TotalCut > 0)
	{
		*QualityScore = score / (CFIQ_MAX_RANK_WEIGHT * TotalCut * meanHeatMap);
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
	DrawPatchGridModeThree(QualityMat, pRowColPairStruct, localRankNum, localSize);
	memcpy(pQualityImageBuffer, QualityMat.data, width * height * 3);
}
