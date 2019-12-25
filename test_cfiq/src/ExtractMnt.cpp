/*
 * ExtractMnt.cpp
 *
 *  Created on: Mar 19, 2019
 *      Author: lyon
 */

#include"ExtractMnt.h"
#include <stdio.h>
#include<memory.h>
//typedef void(*pGAFIS_ExtractMNT_All)(XGWMNTEXTRACTSTR *);
int ch_to_int(unsigned char *ch)
{
    int k;
    unsigned int kk;

    kk=ch[0];
    kk=(kk<<8)+ch[1];
    kk=(kk<<8)+ch[2];
    kk=(kk<<8)+ch[3];
    k=kk;
    return(k);
}


int ch2_to_int(unsigned char *ch)
{
    int k;
    short int kk;

    kk=ch[0];
    kk=(kk<<8)+ch[1];
    k=kk;
    return(k);
}

void int_to_ch(int k, unsigned char *ch)
{
    ch[0]=k>>24;
    ch[1]=(k>>16)&(0xff);
    ch[2]=(k>>8) & (0xff);
    ch[3]=k & (0xff);
}

void int_to_ch2(int k, unsigned char *ch)
{
    ch[0]=(k>>8) & (0xff);
    ch[1]=k & (0xff);
}

uint2 Char2To_uint2(const UCHAR *ch)
{
    return	(uint2)(ch[0]*256+ch[1]);
}

int GetCorePosition(FINGERMNTSTRUCT* pMnt, int* cRow, int* cCol)
{
	double scale = (double)ch2_to_int(pMnt->resolution) / ch2_to_int(pMnt->nres);
	int nOffsetX = ch2_to_int(pMnt->offsetX);
	int nOffsetY = ch2_to_int(pMnt->offsetY);
	nOffsetX = nOffsetX*scale + 0.5;
	nOffsetY = nOffsetY*scale + 0.5;
	int row = (int)(pMnt->cx*scale);
	int col = (int)(pMnt->cy*scale);
	int isValidCore = 0;
	if (0 < row && 0 < col)
	{
		*cRow = row + nOffsetX;
		*cCol = col + nOffsetY;
		isValidCore = 1;
	}
	return isValidCore;
}

int ExtractSingleRaw( unsigned char *pRaw, int width, int height, int Fingertype, FINGERMNTSTRUCT *pMnt )
{
    // 1. prapare the parameters
    XGWMNTEXTRACTSTR para;
    para.pMnt = (void*)pMnt;
    para.pBin = 0;
    para.pExtPara = 0;
    para.ExtractMode = 0;
    para.belatent = 0;
    para.MntBeUpdated = 0;
    para.MntCheckErr = 0;
    memset( para.nRes1, 0, 6);
    memset( para.nRes2, 0, 40);
    //image
    char *pImgBuffer = new char [ sizeof(GAFISIMAGEHEADSTRUCT) + width * height ];
    para.pImage = (GAFISIMAGESTRUCT *) pImgBuffer;
    memcpy( para.pImage->bnData, pRaw, width * height );
    int_to_ch( sizeof(GAFISIMAGEHEADSTRUCT), para.pImage->stHead.nSize );
    int_to_ch2( width, para.pImage->stHead.nWidth );
    int_to_ch2( height, para.pImage->stHead.nHeight );
    para.pImage->stHead.nBits = 8;
    para.pImage->stHead.bIsCompressed = 0;
    para.pImage->stHead.nCompressMethod = 0;
    para.pImage->stHead.nCaptureMethod = 3;
    int_to_ch( width * height, para.pImage->stHead.nImgSize);
    int_to_ch2( 500, para.pImage->stHead.nResolution );
    para.pImage->stHead.bIsPlain = 0;
    para.pImage->stHead.nImageType = 1;
    para.pImage->stHead.nFingerIndex = Fingertype;
    memset( para.pImage->stHead.bnRes, 0, 3);
    memset( para.pImage->stHead.bnRes2, 0, 8);
    strcpy( para.pImage->stHead.szName, "FingerRHMCpr" );


    // ExtractFeaTrue
    int r = GAFIS_ExtractMNT_All(&para);
    if(0 > r)
    {
        printf("GAFIS_ExtractMNT_All error r is :%d\n", r);
        delete[] pImgBuffer;
        return r;
    }

    // free buffer
    delete[] pImgBuffer;
    return 1;
}


