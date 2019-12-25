/*
 * GetCutInfo.cpp
 *
 *  Created on: Mar 19, 2019
 *      Author: lyon
 */

#include"memory.h"
#include"InsideFunctionDef.h"
void GetCutInfo(unsigned char *pForeground, int width, int height, int localSize, int stepSize, RowColPairStruct * pRowColPairStruct, int *localRankNum)
{
	//map<unsigned char, vector<unsigned char>> cutInfoMap;
	*localRankNum = 0;
	unsigned char boolRowBreak = 0;
	unsigned short int *pRowInterArray = new unsigned short int[width];
	int rowID = 0;
	while (true)
	{
		memset(pRowInterArray, 0, width);
		unsigned char boolColBreak = 0;
		bool boolCut = false;
		int loopCount = 0;
		for (int colID = 0; colID < width; colID++)
		{
			bool boolIterResult = true;
			for (int rowCount = 0; rowCount < localSize; rowCount++)
			{
				if (pForeground[(rowID + rowCount) * width + colID] == 0)
				{
					boolIterResult = false;
					break;
				}
			}
			if (boolIterResult)
			{
				pRowInterArray[loopCount++] = colID;
			}
		}

		int index = 0;
		while (index + localSize <= loopCount)
		{
			if (pRowInterArray[index] + localSize - 1 == pRowInterArray[index + localSize - 1])
			{
				pRowColPairStruct[(*localRankNum)].RowID = rowID;
				pRowColPairStruct[(*localRankNum)].ColID = pRowInterArray[index];
				pRowColPairStruct[*(localRankNum)].boolForecast = false;
				(*localRankNum)++;
				boolCut = true;
				index += stepSize;
			}
			else
			{
				index++;
			}

			if (index + localSize > loopCount)
			{
				if (boolColBreak++)
					break;
				index = loopCount - localSize;
			}
		}

		if (!boolCut)
		{
			rowID++; //±ŸŽÎÎÞÇÐÍŒ£¬ŒäžôŒÓ1
		}
		else
		{
			rowID += stepSize;//±ŸŽÎÓÐÇÐÍŒŒäžôŒÓ stepSize
		}
		if (rowID + localSize > height) //ÊÇ·ñµœÐÐ±ßÔµ
		{
			if (boolRowBreak++)//±ßÔµŽŠÒªÏžÇÐ£¬²»ÄÜÂíÉÏÍË³ö
				break;
			rowID = height - localSize;
		}
	}
	delete pRowInterArray;
}


