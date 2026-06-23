#pragma once
#include"StructDef.h"
void GetCutInfo(unsigned char *pForeground, int width, int height, int localSize, int stepSize, RowColPairStruct * pRowColPairStruct, int *localRankNum);
void GetLocalBlockAndNormalize(RowColPairStruct * pRowColPairStruct, int size, unsigned char *pSrcImg, int width, int height, int localSize, float * pLocalBlockFloatBuffer);
//void Normalize(unsigned char *pSrcLocalBock, int size);