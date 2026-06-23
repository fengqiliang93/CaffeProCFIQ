// Copyright (c) 2014 The Nirvana Authors. All rights reserved.

/*
 * 图像格式定义文件
 * @file image_def.h
 */
#include <stdio.h>

#ifndef _NIRVANA_IMAGE_DEF_H_
#define _NIRVANA_IMAGE_DEF_H_

//#include "afisdef.h"

/**
 * 图像数据头文件结构
 */
typedef struct tagGAFISIMAGEHEADSTRUCT {
    UCHAR nSize[4]; // size of this structure, 4 bytes int
    UCHAR nWidth[2]; // width of the image, 2 bytes int
    UCHAR nHeight[2]; // height of the image, 2 bytes int
    UCHAR nBits; // bit per pixel, 8, 16, 24, no pallete, 1 byte
    UCHAR bIsCompressed; // whether is compressed, 1 byte
    UCHAR nCompressMethod; // compressed method, if compressed, 1 byte
    UCHAR nCaptureMethod; // image capture method GA_IMGCAPTYPE_XXX, 1 byte
    UCHAR nImgSize[4]; // size of image, does not include head size, 4 bytes int
    UCHAR nResolution[2]; // resolution, 2 bytes int
    UCHAR bIsPlain; // plain type or non plain type
    UCHAR nImageType; // GAIMG_IMAGETYPE_XXX
    UCHAR nFingerIndex; // used only by exf.
    UCHAR bnRes[3]; // 3 bytes reserved
    UCHAR bnRes2[8]; // reserved
    char szName[32]; // image name
} GAFISIMAGEHEADSTRUCT; // total length of this structure is 64 bytes

#define	GAIMG_IMAGETYPE_UNKNOWN		0x0
#define	GAIMG_IMAGETYPE_FINGER		0x1
#define	GAIMG_IMAGETYPE_PALM		0x2
#define	GAIMG_IMAGETYPE_FACE		0x3
#define	GAIMG_IMAGETYPE_CARDIMG		0x4

#define	GA_IMGCAPTYPE_UNKNOWN		0x0
#define	GA_IMGCAPTYPE_SCANCARD		0x1
#define	GA_IMGCAPTYPE_LIVESCAN		0x2
#define	GA_IMGCAPTYPE_CPRGEN		0x3


/** 
 * 图像定义，此结构的前面一部分为头文件说明，后面跟随实际数据
 * <code> 
 *  GAFISIMAGESTRUCT *pImg;
 *  UCHAR *pImgData;
 *  pImgData = (UCHAR *)pImg->bnData;
 * </code>
 */
typedef struct tagGAFISIMAGESTRUCT {
  /** 图像头定义 */
  GAFISIMAGEHEADSTRUCT stHead; // image head structure
  /** 实际数据 */
  UCHAR bnData[8]; // image followed
} GAFISIMAGESTRUCT; // size of this structure depends on the image size(32-2GB)

//GAFISIMAGESTRUCT *pImg;
//UCHAR *pImgData;
//pImgData = (UCHAR *)pImg->bnData;

#endif //_NIRVANA_IMAGE_DEF_H_