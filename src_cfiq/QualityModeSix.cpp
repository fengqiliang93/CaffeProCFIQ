/*
 * CFIQ.cpp
 *
 *  Created on: Mar 19, 2019
 *      Author: lyon
 */
#include<iostream>
#include<string>
#include<cstdlib>
#include<cstring>
#include<cstdio>
#include<vector>
#include<chrono>

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include"ForegroundExtractor.h"
#include"StructDef.h"
#include"InsideFunctionDef.h"
#include"caffe/net.hpp"
#include"CFIQ.h"
using namespace cv;
using namespace caffe;
using namespace std;

static bool TraceQualityModeSix()
{
	const char *traceQualityEnv = getenv("CFIQ_TRACE_QUALITY");
	return traceQualityEnv != NULL && strcmp(traceQualityEnv, "1") == 0;
}

static bool ProfileQualityModeSix()
{
	const char *profileQualityEnv = getenv("CFIQ_PROFILE_QUALITY6");
	return profileQualityEnv != NULL && strcmp(profileQualityEnv, "1") == 0;
}

static double QualityModeSixElapsedMs(const std::chrono::steady_clock::time_point &begin,
                                      const std::chrono::steady_clock::time_point &end)
{
	return std::chrono::duration<double, std::milli>(end - begin).count();
}

static void CopyPatchToBuffer(const cv::Mat &srcBmpFloatMat,
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

void GetQualityScoreModeSix(void *pForeground, void *pRowColPair, void *pHeatMapFloat, void *pCaffeNet, void *pHInstance, void *pQuality, unsigned char *srcBmp, int width, int height, int fgp, int Core_X, int Core_Y, int localSize, int stepSize, float *QualityScore)
{
		const bool traceQuality = TraceQualityModeSix();
		const bool profileQuality = ProfileQualityModeSix();
		double msMemset = 0.0;
		double msExtractForeground = 0.0;
		double msConvert = 0.0;
		double msGetCutInfo = 0.0;
		double msInitMats = 0.0;
		double msCopyPatch = 0.0;
		double msForward = 0.0;
		double msHeatMapMean = 0.0;
		double msRankMat = 0.0;
		double msFinalMean = 0.0;
		const std::chrono::steady_clock::time_point profileBegin =
		    std::chrono::steady_clock::now();
		if (traceQuality)
		{
			fprintf(stderr, "[quality-six] begin fgp=%d core=(%d,%d) local=%d step=%d\n", fgp, Core_X, Core_Y, localSize, stepSize);
		}
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
		RowColPairStruct *pRowColPairStruct = (RowColPairStruct *)pRowColPair;
		std::chrono::steady_clock::time_point profileStepBegin =
		    std::chrono::steady_clock::now();
		memset(pRowColPairStruct, 0, sizeof(RowColPairStruct) * width * height);
		if (profileQuality)
		{
			msMemset += QualityModeSixElapsedMs(profileStepBegin, std::chrono::steady_clock::now());
		}


		if (traceQuality)
		{
			fprintf(stderr, "[quality-six] extract-foreground\n");
		}
		profileStepBegin = std::chrono::steady_clock::now();
		if (!ExtractForeground(pHInstance, srcBmp, width, height, fgp, pForegroundBuffer))
			return;
		if (profileQuality)
		{
			msExtractForeground += QualityModeSixElapsedMs(profileStepBegin, std::chrono::steady_clock::now());
		}
		if (traceQuality)
		{
			fprintf(stderr, "[quality-six] foreground-ready\n");
		}
		
		profileStepBegin = std::chrono::steady_clock::now();
		cv::Mat srcBmpMat(width, height, CV_8UC1, srcBmp);
		//cv::imwrite(".\\srcbmp\\" + string(pPersonID) + "_" + to_string(fgp) + "_" + "src" + ".bmp", srcBmpMat);
		//cv::Mat_<float> srcBmpFloatMat = srcBmpMat * 1.0f / 255.0f;
		cv::Mat srcBmpFloatMat(width, height, CV_32FC1);
		srcBmpMat.convertTo(srcBmpFloatMat, CV_32FC1);
		srcBmpFloatMat = srcBmpFloatMat * 1.0f / 255.0f;
		if (profileQuality)
		{
			msConvert += QualityModeSixElapsedMs(profileStepBegin, std::chrono::steady_clock::now());
		}


		cv::Mat ForegroundBufferMat(width, height, CV_8UC1, pForegroundBuffer);
		//cv::imwrite(".\\bmp\\" + string(pPersonID) + "_" + to_string(fgp) + "_" + "foreground" + ".bmp", ForegroundBufferMat * 255);
		int localRankNum = 0;

		profileStepBegin = std::chrono::steady_clock::now();
		GetCutInfo(ForegroundBufferMat.data, width, height, localSize, stepSize, pRowColPairStruct, &localRankNum);
		if (profileQuality)
		{
			msGetCutInfo += QualityModeSixElapsedMs(profileStepBegin, std::chrono::steady_clock::now());
		}
		if (traceQuality)
		{
			fprintf(stderr, "[quality-six] cut-info localRankNum=%d\n", localRankNum);
		}

		Blob<float> *input_blobs = pNet->input_blobs()[0];
		Blob<float> *output_blobs = pNet->output_blobs()[0];
		if (traceQuality)
		{
			fprintf(stderr, "[quality-six] blob-shape input_count=%d output_channels=%d\n", input_blobs->count(), output_blobs->channels());
		}

		const int localArea = localSize * localSize;
		profileStepBegin = std::chrono::steady_clock::now();
		std::vector<float> localRankBuffer(localArea);
		cv::Mat_<float> QualityBmpMat(width, height, (float *)pHeatMapFloat + (fgp - 1) * 640 * 640);
		if (profileQuality)
		{
			msInitMats += QualityModeSixElapsedMs(profileStepBegin, std::chrono::steady_clock::now());
		}

		int rowMinPosition = 0, colMinPosition = 0;
		float heatMap = 0.0f, score = 0.0f;

		for (int localRankCount = 0; localRankCount < localRankNum; localRankCount++)
		{
			rowMinPosition = pRowColPairStruct[localRankCount].RowID - y_correct;
			colMinPosition = pRowColPairStruct[localRankCount].ColID - x_correct;
			profileStepBegin = std::chrono::steady_clock::now();
			CopyPatchToBuffer(srcBmpFloatMat,
			                  pRowColPairStruct[localRankCount].RowID,
			                  pRowColPairStruct[localRankCount].ColID,
			                  localSize,
			                  &localRankBuffer[0]);
			if (profileQuality)
			{
				msCopyPatch += QualityModeSixElapsedMs(profileStepBegin, std::chrono::steady_clock::now());
			}
			input_blobs->set_cpu_data(&localRankBuffer[0]);
			if (traceQuality && (localRankCount == 0 || (localRankCount + 1) % 25 == 0 || localRankCount + 1 == localRankNum))
			{
				fprintf(stderr, "[quality-six] forward-begin %d/%d row=%d col=%d\n", localRankCount + 1, localRankNum, pRowColPairStruct[localRankCount].RowID, pRowColPairStruct[localRankCount].ColID);
			}
			profileStepBegin = std::chrono::steady_clock::now();
			pNet->Forward();
			if (profileQuality)
			{
				msForward += QualityModeSixElapsedMs(profileStepBegin, std::chrono::steady_clock::now());
			}
			if (traceQuality && (localRankCount == 0 || (localRankCount + 1) % 25 == 0 || localRankCount + 1 == localRankNum))
			{
				fprintf(stderr, "[quality-six] forward-end %d/%d\n", localRankCount + 1, localRankNum);
			}

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
					profileStepBegin = std::chrono::steady_clock::now();
					heatMap = cv::mean(QualityBmpMat(Range(rowMinPosition, rowMinPosition + localSize), Range(colMinPosition, colMinPosition + localSize)))[0];
					if (profileQuality)
					{
						msHeatMapMean += QualityModeSixElapsedMs(profileStepBegin, std::chrono::steady_clock::now());
					}
					score += 1 * heatMap;
				}
				else if (pRowColPairStruct[localRankCount].Rank == 2)
				{
					profileStepBegin = std::chrono::steady_clock::now();
					heatMap = cv::mean(QualityBmpMat(Range(rowMinPosition, rowMinPosition + localSize), Range(colMinPosition, colMinPosition + localSize)))[0];
					if (profileQuality)
					{
						msHeatMapMean += QualityModeSixElapsedMs(profileStepBegin, std::chrono::steady_clock::now());
					}
					score += 2 * heatMap;
				}
				else if (pRowColPairStruct[localRankCount].Rank == 3)
				{
					profileStepBegin = std::chrono::steady_clock::now();
					heatMap = cv::mean(QualityBmpMat(Range(rowMinPosition, rowMinPosition + localSize), Range(colMinPosition, colMinPosition + localSize)))[0];
					if (profileQuality)
					{
						msHeatMapMean += QualityModeSixElapsedMs(profileStepBegin, std::chrono::steady_clock::now());
					}
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
		profileStepBegin = std::chrono::steady_clock::now();
		float meanHeatMap = cv::mean(QualityBmpMat(Range(y_BegRow, y_EndRow), Range(x_BegCol, x_EndCol)))[0];
		if (profileQuality)
		{
			msFinalMean += QualityModeSixElapsedMs(profileStepBegin, std::chrono::steady_clock::now());
		}
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
		if (traceQuality)
		{
			fprintf(stderr, "[quality-six] done score=%.9g totalCut=%d meanHeatMap=%.9g\n", *QualityScore, TotalCut, meanHeatMap);
		}
		if (profileQuality)
		{
			const double msTotal = QualityModeSixElapsedMs(profileBegin, std::chrono::steady_clock::now());
			fprintf(stderr,
			        "[quality6-profile] total_ms=%.6f localRankNum=%d memset=%.6f extract=%.6f convert=%.6f cut=%.6f init_mats=%.6f copy_patch=%.6f forward=%.6f heatmap_mean=%.6f rank_mat=%.6f final_mean=%.6f\n",
			        msTotal,
			        localRankNum,
			        msMemset,
			        msExtractForeground,
			        msConvert,
			        msGetCutInfo,
			        msInitMats,
			        msCopyPatch,
			        msForward,
			        msHeatMapMean,
			        msRankMat,
			        msFinalMean);
		}
}
