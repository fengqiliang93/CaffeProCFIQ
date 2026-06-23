// Copyright (c) 2014 The Nirvana Authors. All rights reserved.

/**
 * 纹线查看工具类
 * @file line_count.h
 */

#ifndef _NIRVANA_LINE_COUNT_H_
#define _NIRVANA_LINE_COUNT_H_

NIRVANA_BEGIN_API
  
typedef struct 
{
	int	StartX,StartY;
	int EndX,EndY;
	int sum;
	int PosX[256],PosY[256],PosZ[256];
}  LINECOUNTSTRUCT;

NIRVANA_EXPORT int GAFIS_LineCount_Init(unsigned char *srcimg,int width,int height);
NIRVANA_EXPORT void GAFIS_LineCount_Finished();
NIRVANA_EXPORT int GAFIS_LineCount(unsigned char *srcimg,int width,int height,LINECOUNTSTRUCT *linec);

NIRVANA_END_API

#endif //_NIRVANA_LINE_COUNT_H_