#include<iostream>
#include<string>

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

void GetQualityScoreModeFive(void *pForeground, void *pRowColPair, void *pHeatMapFloat, void *pCaffeNet, void *pHInstance, void *pQuality, unsigned char *srcBmp, int width, int height, int fgp, int Core_X, int Core_Y, int localSize, int stepSize, float *QualityScore)
{
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

	if (!ExtractForeground(pHInstance, srcBmp, width, height, fgp, pForegroundBuffer))
		return;

	cv::Mat srcBmpMat(width, height, CV_8UC1, srcBmp);
	cv::Mat srcBmpFloatMat(width, height, CV_32FC1);
	srcBmpMat.convertTo(srcBmpFloatMat, CV_32FC1);
	srcBmpFloatMat = srcBmpFloatMat * 1.0f / 255.0f;

	cv::Mat ForegroundBufferMat(width, height, CV_8UC1, pForegroundBuffer);
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

	float score = 0.0f;

	for (int localRankCount = 0; localRankCount < localRankNum; localRankCount++)
	{
		localRankMat = srcBmpFloatMat(Range(pRowColPairStruct[localRankCount].RowID, pRowColPairStruct[localRankCount].RowID + localSize), Range(pRowColPairStruct[localRankCount].ColID, pRowColPairStruct[localRankCount].ColID + localSize)).clone();
		input_blobs->set_cpu_data((float *)localRankMat.data);
		pNet->Forward();

		int maxChannel = 0;
		for (int channelNum = 1; channelNum < output_blobs->channels(); channelNum++)
		{
			if (pCpuData[maxChannel] < pCpuData[channelNum])
				maxChannel = channelNum;
		}
		pRowColPairStruct[localRankCount].Rank = output_blobs->channels() - maxChannel;
		pRowColPairStruct[localRankCount].boolForecast = true;

		if (pRowColPairStruct[localRankCount].Rank == 1)
		{
			score += CFIQRankWeight(pRowColPairStruct[localRankCount].Rank);
			BadRankMat(Range(pRowColPairStruct[localRankCount].RowID, pRowColPairStruct[localRankCount].RowID + localSize), Range(pRowColPairStruct[localRankCount].ColID, pRowColPairStruct[localRankCount].ColID + localSize)) += 1;
		}
		else if (pRowColPairStruct[localRankCount].Rank == 2)
		{
			score += CFIQRankWeight(pRowColPairStruct[localRankCount].Rank);
			MiddleRankMat(Range(pRowColPairStruct[localRankCount].RowID, pRowColPairStruct[localRankCount].RowID + localSize), Range(pRowColPairStruct[localRankCount].ColID, pRowColPairStruct[localRankCount].ColID + localSize)) += 1;
		}
		else if (pRowColPairStruct[localRankCount].Rank == 3)
		{
			score += CFIQRankWeight(pRowColPairStruct[localRankCount].Rank);
			GoodRankMat(Range(pRowColPairStruct[localRankCount].RowID, pRowColPairStruct[localRankCount].RowID + localSize), Range(pRowColPairStruct[localRankCount].ColID, pRowColPairStruct[localRankCount].ColID + localSize)) += 1;
		}
	}

	int TotalCut = ((height - localSize) / stepSize + 1) * ((width - localSize) / stepSize + 1);
	TotalCut = TotalCut > localRankNum ? TotalCut : localRankNum;
	if (TotalCut > 0)
	{
		*QualityScore = score * 1.0f / (CFIQ_MAX_RANK_WEIGHT * TotalCut);
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
