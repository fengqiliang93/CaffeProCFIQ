/*
 * ExtractMnt.cpp
 *
 *  Created on: Mar 19, 2019
 *      Author: lyon
 */

#include"ExtractMnt.h"
#include <stdio.h>
#include<memory.h>
#include <stdlib.h>
#include <pthread.h>
//typedef void(*pGAFIS_ExtractMNT_All)(XGWMNTEXTRACTSTR *);
extern "C" int XGW_Extraction_Init();
extern "C" int XGW_Extraction_End();

static pthread_mutex_t gMntExtractorMutex = PTHREAD_MUTEX_INITIALIZER;

void DestructMntExtractor()
{
    pthread_mutex_lock(&gMntExtractorMutex);
    XGW_Extraction_End();
    pthread_mutex_unlock(&gMntExtractorMutex);
}

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
	int resolution = ch2_to_int(pMnt->resolution);
	int nres = ch2_to_int(pMnt->nres);
	if (resolution <= 0 || nres <= 0)
	{
		const char *traceMntEnv = getenv("CFIQ_TRACE_MNT");
		if (traceMntEnv != NULL && strcmp(traceMntEnv, "1") == 0)
		{
			fprintf(stderr,
					"[mnt] invalid-resolution resolution=%d nres=%d rp=%u cx=%u cy=%u coreQ=%u vcoreQ=%u\n",
					resolution,
					nres,
					(unsigned int)pMnt->rp,
					(unsigned int)pMnt->cx,
					(unsigned int)pMnt->cy,
					(unsigned int)pMnt->FeatQlev.CoreQlev,
					(unsigned int)pMnt->FeatQlev.VCoreQlev);
		}
		return 0;
	}
	double scale = (double)resolution / nres;
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
	const char *traceMntEnv = getenv("CFIQ_TRACE_MNT");
	if (traceMntEnv != NULL && strcmp(traceMntEnv, "1") == 0)
	{
		fprintf(stderr,
				"[mnt] resolution=%d nres=%d offset=(%d,%d) rp=%u cx=%u cy=%u ca=%u coreQ=%u vcoreQ=%u row=%d col=%d valid=%d\n",
				resolution,
				nres,
				nOffsetX,
				nOffsetY,
				(unsigned int)pMnt->rp,
				(unsigned int)pMnt->cx,
				(unsigned int)pMnt->cy,
				(unsigned int)pMnt->ca,
				(unsigned int)pMnt->FeatQlev.CoreQlev,
				(unsigned int)pMnt->FeatQlev.VCoreQlev,
				row,
				col,
				isValidCore);
	}
	return isValidCore;
}

int ExtractSingleRaw( unsigned char *pRaw, int width, int height, int Fingertype, FINGERMNTSTRUCT *pMnt )
{
    memset(pMnt, 0, sizeof(FINGERMNTSTRUCT));
    pthread_mutex_lock(&gMntExtractorMutex);
    XGW_Extraction_End();
    int initResult = XGW_Extraction_Init();
    const char *traceMntEnv = getenv("CFIQ_TRACE_MNT");
    if (traceMntEnv != NULL && strcmp(traceMntEnv, "1") == 0)
    {
        fprintf(stderr, "[mnt] extraction-init result=%d\n", initResult);
    }

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
    if (traceMntEnv != NULL && strcmp(traceMntEnv, "1") == 0)
    {
        fprintf(stderr,
                "[mnt] extract-result r=%d mntCheckErr=0x%x mntBeUpdated=0x%x\n",
                r,
                para.MntCheckErr,
                para.MntBeUpdated);
    }
    if(0 > r)
    {
        printf("GAFIS_ExtractMNT_All error r is :%d\n", r);
        delete[] pImgBuffer;
        pthread_mutex_unlock(&gMntExtractorMutex);
        return r;
    }

    // free buffer
    delete[] pImgBuffer;
    pthread_mutex_unlock(&gMntExtractorMutex);
    return 1;
}
