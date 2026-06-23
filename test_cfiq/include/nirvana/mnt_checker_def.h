// Copyright (c) 2014 The Nirvana Authors. All rights reserved.

/*
 * 特征文件展示相关结构定义的头文件.
 * @file mnt_checker_def.h
 */

#ifndef	_NIRVANA_MNT_CHECKER_DEF_H_
#define _NIRVANA_MNT_CHECKER_DEF_H_

NIRVANA_BEGIN_API

  //#define GAIFS_SWEATPORE_USED

#define	KEYIDLEN				32
#define	USERIDLEN				16

  //typedef unsigned short int	uint2;
  //typedef unsigned char		UCHAR;
  //typedef short int			int2;
  //typedef unsigned  int		uint4;

  /* value for MNTDATSTRUCT.rp */
#define	MNTRP_UNDET		0	/* 不确定 */
#define	MNTRP_ARCH		1	/* 弓型  */
#define	MNTRP_LEFTLOOP	2	/* 左箕 */
#define	MNTRP_RIGHTLOOP	3	/* 右箕 */
#define	MNTRP_WHORL		4	/* 斗  */

  /* value for whorl type */
#define	MNTWHORL_UNDET	0
#define	MNTWHORL_RIGHT	1
#define	MNTWHORL_LEFT	2

  typedef struct tagAFISPOINTSTRUCT {
    uint2 x;
    uint2 y;
  } AFISPOINTSTRUCT; // 4 bytes long.

  typedef struct tagAFISMNTPOINTSTRUCT {
    uint2 x; //	特征点横向坐标，范围从0到 nImgWidth-1
    uint2 y; //	特征点纵向坐标，范围从0到 nImgHeight-1
    int2 z; //	细节特征方向, [-90, 270)
    UCHAR nFlag; //	MNTPTFLAG_XXX  使用特征质量标记
    UCHAR nReliability; //	特征可靠度，0...不可信，1...可信
    UCHAR nPosOnFinger; //	掌纹特征位置 按位计算，或的关系
    //	0x1.....指根区，0x2...内侧区 0x4...外侧区
    UCHAR nRes[7];
  } AFISMNTPOINTSTRUCT; // 16 bytes

#define	MNTPTFLAG_USEREL	0x1		//	使用特征质量标记
  //	指纹
#define	DELTACLASS_CORE		0
#define	DELTACLASS_VICECORE		0

#define	DELTACLASS_LEFT		0
#define	DELTACLASS_RIGHT	1

#define	DELTACLASS_UNKNOWN		0
#define	DELTACLASS_WRIST		1
#define	DELTACLASS_FINGROOT		2
#define	DELTACLASS_PATTERN		3	// pattern

  // structure for center
  typedef struct tagAFISCOREDELTASTRUCT {
    uint2 x;
    uint2 y;
    int2 z; //	特征方向，范围从-90到 270，999表示未知方向
    UCHAR nRadius; //	位置半径
    UCHAR nzVarRange; //	方向范围
    UCHAR nReliability; //	可信度，1...估计，2...虚拟
    UCHAR nPosOnPalm; //	特征所在区域，0：未知区域，1：指根区 ,2：内侧区，3：外侧区，其他：跨区域用（掌纹）
    UCHAR nFingerIdx; //  指根三角索引号（掌纹用）
    UCHAR nClass; //	三角是 DELTACLASS_XXX, （副）中心是 CORECLASS_XXX
    UCHAR bIsExist; //	0...不存在，1...存在
    UCHAR bEdited; //	是否编辑过
    UCHAR nRes[2];
  } AFISCOREDELTASTRUCT; // 16 bytes

  // structure for circle

  typedef struct tagAFISCIRCLESTRUCT {
    uint2 x;
    uint2 y;
    UCHAR nRelibility;
    UCHAR nRadius;
    UCHAR nRes[2];
  } AFISCIRCLESTRUCT; // 8 bytes

  typedef struct tagAFISRECTSTRUCT {
    uint2 l; // left
    uint2 t; // top
    uint2 r; // right
    uint2 b; // bottom
    UCHAR nPos; // which part, MNTPART_XX  
    UCHAR bnRes[3];
  } AFISRECTSTRUCT; // size is 12 bytes

  //  MODIFY AND NEW MNTPART_XXX  
#define	MNTPART_PALM_FP	1
#define	MNTPART_PALM_IP	2
#define	MNTPART_PALM_OP	3

  //	指根区的小分区
#define	MNTPART_PALM_FP1	11
#define	MNTPART_PALM_FP2	12
#define	MNTPART_PALM_FP3	13
#define	MNTPART_PALM_FP4	14
  //	内侧区的小分区
#define	MNTPART_PALM_IPUP	21
#define	MNTPART_PALM_IPMD	22
#define	MNTPART_PALM_IPLW	23
  //	外侧区的小分区
#define	MNTPART_PALM_OPUP	31
#define	MNTPART_PALM_OPMD	32
#define	MNTPART_PALM_OPLW	33


#define	MNTPART_FINGER_UPPER	1
#define	MNTPART_FINGER_LOWER	2
#define	MNTPART_FINGER_LEFT		3
#define	MNTPART_FINGER_RIGHT	4

  typedef struct tagLINECNT2MNTSTRUCT {
    uint2 nStartMntIdx; // first mnt index, 2 bytes
    uint2 nEndMntIdx; // second mnt index	2 bytes
    uint2 nLineCnt; // 2 bytes
    UCHAR nRes[2]; // reserved 2 bytes
  } LINECNT2MNTSTRUCT; // total 8 bytes

  typedef struct tagGAFIS_PALMSPECMNTSTRUCT {
    /////////////////		掌纹专有特征	////////////////////////////////
    UCHAR nPalmIndex; // MODIFY  掌位，0：未知，1：右掌，2：左掌
    UCHAR bnRes[3]; // MODIFY reserved, to here is 4 bytes long.
    uint2 nBaseLinePos; // 
    int2 wca; //  NEW 第一曲肌褶纹方向（起点为外侧点）
    // to here is 8 bytes long.
    UCHAR bnRes0[8];
    AFISPOINTSTRUCT stWIPt; // 	第一曲肌褶纹内侧点
    AFISPOINTSTRUCT stWOPt; //  第一曲肌褶纹外侧点
    UCHAR nPatternDeltaCnt; //	花纹三角个数
    UCHAR nPatternCoreCnt; //	花纹中心个数
    UCHAR bnRes_1[6]; // to here is 32 bytes
    AFISCOREDELTASTRUCT WristDelta; //	腕部三角
    AFISCOREDELTASTRUCT PatternDelta[20]; //	花纹三角
    AFISCOREDELTASTRUCT PatternCore[20]; //	花纹中心
    UCHAR bnRes_3[336];
  } GAFIS_PALMSPECMNTSTRUCT; // structure is 1024 bytes

  typedef struct tagGAFIS_FINGERSPECMNTSTRUCT {
    //////////////////		指纹专有特征   /////////////////////////////
    UCHAR rp; //	纹型 0，1，2，3，4
    UCHAR vrp; //  副纹型 0，1，2，3，4
    UCHAR FingerIdx; //	捺印指纹指位
    UCHAR FingerCode[10]; //	现场候选指位
    UCHAR RpCode[4]; //	现场候选纹型
    UCHAR fgrp[10][4]; //	现场连指纹型
    UCHAR bEditedRP; //	纹型是否修改过
    UCHAR bnRes[6]; //  MODIFY to here is 64 bytes
    AFISCOREDELTASTRUCT upcore; //	指纹中心特征
    AFISCOREDELTASTRUCT lowcore; //	指纹副中心特征
    AFISCOREDELTASTRUCT ldelta; //	指纹左三角特征
    AFISCOREDELTASTRUCT rdelta; //	指纹右三角特征
  } GAFIS_FINGERSPECMNTSTRUCT; // size of this structure is 64+64=128 bytes


  /**
   * Added by beagle on Dec. 12, 2008
   * 增加汗眼特征数据信息
   */

  /*
     typedef struct tagGAFIS_PorePointData
     {
     GAFIS_POREPOINT	*pstPore;
     int		nPoreCount;
     int		bCanBeFree;
     }GAFIS_POREPOINTDATA;
     */

  typedef struct tagGAFIS_PFCOMMONMNTSTRUCT {
    //////////////////		指纹掌纹共有特征   /////////////////////////////
    UCHAR MntVersion; //	特征提取版本
    UCHAR qlev; //	捺印指纹掌纹图象整体质量
    int2 fca; //	指纹掌纹方向
    uint2 D_fca; //	指纹掌纹方向范围

    UCHAR Distore; //  变形参数，0...未知，1...变形小，2...变形大

    UCHAR bUseQlev; //	细节特征是否评判质量 0...无，1...有					
    uint2 nMntCnt; //	细节特征数目
    uint2 nExactAreaCnt; //	现场稳定区域个数
    uint2 n2ptlc; //	现场特征数线个数
    uint2 nMntRegionCnt; //	NEW 现场特征位置信息框个数
    AFISMNTPOINTSTRUCT mnt[1500]; //	细节特征数据
    AFISCIRCLESTRUCT exactarea[32]; //	现场稳定区域数据	
    LINECNT2MNTSTRUCT m_st2ptlc[32]; //	现场特征数线信息
    AFISRECTSTRUCT stMntRegion[8]; //  NEW 8 rects, a minutia can belong one or more of them
    //	GAFIS_POREPOINTDATA	stPorePoint;	// 汗眼数据信息
    UCHAR bnRes[976]; // - sizeof(GAFIS_POREPOINTDATA)];
  } GAFIS_PFCOMMONMNTSTRUCT; // size is 25KB=1024*25 bytes

  typedef struct tagGAFIS_MNTINNERDATA {
    void *pData;
    int nMntDataLen;
    int bCanBeFree;
  } GAFIS_MNTINNERDATA;

  //A类：RpQlev>80 ImgQlev>85，B类：RpQlev>70 ImgQlev>75，C类：ImgQlev 50分以上，D类：ImgQlev 50分以下

  typedef struct tagFINGERFEATQLEV {
    unsigned char bUseQlev; //	是否使用特征质量判断，	1...使用，	0...不使用
    unsigned char bIsPlain; //	是否平面指纹,			1...是，	0...不是
    unsigned char ImgQlev; //	指纹图像质量，主要反映纹线清晰程度，0－100
    unsigned char RpQlev; //	形态特征总体质量， 建议80分以下要干预
    unsigned char CoreQlev; //  中心可靠程度，0－100以及255，255代表主纹型下该特征不存在，0代表该特征未被提取（副中心，三角类似）
    unsigned char VCoreQlev; //	副中心可靠程度
    unsigned char LDeltaQlev; //	左三角可靠程度
    unsigned char RDeltaQlev; //  右三角可靠程度
  } FINGERFEATQLEV;

  typedef struct tagMNTDISPSTRUCT {
    //////////////////		基本数据       /////////////////////////////
    uint2 nSize; //	结构长度  ？？ 是否需要
    uint2 nWidth; //	图象宽度
    uint2 nHeight; //	高度
    uint2 nResolution; //	分辨率
    UCHAR bIsPalm; //  0...指纹，1...掌纹
    UCHAR bIsLatent; //  0...捺印，1...现场
    uint2 nSignature; // signature of the minutiae.
    UCHAR bEditedMinutia; //	捺印细节特征是否修改正确,1...修改正确，0...未修改或未知
    UCHAR nMntFormat; //	特征格式
    UCHAR bnRes_1[2];
    //	以下字段是否需要修改
    UCHAR Bar[KEYIDLEN]; //	条码号
    UCHAR Type;
    UCHAR LibType;
    UCHAR bnRes_2[6];
    int LibID;
    UCHAR LastEditTime[8];
    UCHAR PersonID[KEYIDLEN];
    UCHAR MatchBar[KEYIDLEN];
    UCHAR BreakTime[8];
    UCHAR BreakUserID[USERIDLEN];
    UCHAR CaseID[KEYIDLEN];
    UCHAR CrimeBarCode[KEYIDLEN]; //	查中捺印指纹条码号
    int CrimeTenLibID; //	查中捺印指纹库序号	
    UCHAR CrimeFingerNum; //	查中捺印指纹指位	
    UCHAR Breakdbid;
    UCHAR BreakedFlag;
    UCHAR bnRes_3[5];
    int GroupNo;
    //////////////////////////////////////
    int2 nMaxMnt; // 最大特征数
    int2 nMaxEA; // 最大稳定区域个数
    int2 nMax2PtLC; // 最大特征数线个数
    int2 nMaxMntRegion; // NEW 最大现场特征位置信息框个数
    UCHAR bnRes_4[8]; // MODIFY

    GAFIS_PALMSPECMNTSTRUCT stPm;
    GAFIS_FINGERSPECMNTSTRUCT stFg;
    GAFIS_PFCOMMONMNTSTRUCT stCm; // common mnt
    UCHAR stdReserve[128]; // 内部数据，不允许更改
    GAFIS_MNTINNERDATA stInnerData;

    FINGERFEATQLEV FeatQlev; //	8字节

#ifdef GAIFS_SWEATPORE_USED
    GAFIS_POREPOINT stPorePoint[2000];
    int nPoreCnt;
#else
    UCHAR bnRes_5[1520]; // reserved.
#endif
  } MNTDISPSTRUCT; // total size is very large, 28KB=28*1024.

#define	MNTDIFF_FEAT_NOUSE	0
#define MNTDIFF_FEAT_RIGHT	1
#define MNTDIFF_FEAT_OMIT	2
#define MNTDIFF_FEAT_ERROR	3
#define MNTDIFF_FEAT_FAKE	4
  //  错误类型
  //  0...不存在该类特征, 1...存在并且正确，2...该类特征遗漏，3...该类特征错误

  typedef struct tagMNTDIFF {
    unsigned char rp; //	纹型	MNTDIFF_FEAT_***
    unsigned char upcore; //	上中心	MNTDIFF_FEAT_***
    unsigned char lowcore; //	副中心	MNTDIFF_FEAT_***
    unsigned char ldelta; //	左三角	MNTDIFF_FEAT_***
    unsigned char rdelta; //	右三角	MNTDIFF_FEAT_***
    unsigned char nres1[11];
    short int cm; //	标准特征的细节特征数目
    short int omitmnt; //	遗漏特征数目
    short int errmmt; //	错误(偏)特征数目
    short int fakemmt; //	伪特征数目
    unsigned char nres2[8];
  } MNTDIFF;


NIRVANA_END_API

#endif //_NIRVANA_MNT_CHECKER_DEF_H_
