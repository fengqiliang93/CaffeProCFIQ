// Copyright (c) 2014 The Nirvana Authors. All rights reserved.

/**
 * 抽取使用的头文件
 * @file	extractor.h
 * @author	Jun Tsai
 * @since 2014.7.19
 */
#ifndef _NIRVANA_EXTRACTOR_H_
#define	_NIRVANA_EXTRACTOR_H_

NIRVANA_BEGIN_API

/**
 * 提取特征
 * @param pImage    图像数据结构
 * @param pMnt      输出的特征数据
 * @param pBin      返回的纹线数据
 * @param belatent  是否为捺印图像，0:捺印，1:现场
 * @return          1：成功;<0，失败，见KERROR_XXX
 */
NIRVANA_EXPORT 
int GAFIS_ExtractMNT(GAFISIMAGESTRUCT *pImage,
                     void *pMnt,
                     GAFISIMAGESTRUCT *pBin,
                     int belatent);
/**
 * 提取特征并给出质量判断
 * @param pImage      输入的图像指针
 * @param pMnt        输出的特征数据
 * @param pBin        输出的纹线图(长度在20K以下)
 * @param QualityTh  外部给定的指纹质量阈值(0...100), 0表示为缺省参数
 * @param errinfo     质量信息,如果没有质量问题,就返回字符串长度为0
 * @return            1 成功 <0 失败，见LIVESCAN_QUALITY_XXXXX
 */
NIRVANA_EXPORT
int GAFIS_ExtractMNT_LV(GAFISIMAGESTRUCT *pImage,
                        void *pMnt,
                        GAFISIMAGESTRUCT *pBin,
                        int QualityTh,
                        char *errinfo);

/**
 * 提取特征并给出质量问题代码
 * @param pImage	   指纹图像指针
 * @param pMnt         返回的特征数据
 * @param pBin         返回的纹线图（长度在20K以下）
 * @param QualityTh    外部给定的指纹质量阈值(0...100), 0表示为缺省参数
 * @param MosaicErrNo  返回的质量问题的代码（MOSAIC_ERR_XXX， -20到-25）,如果没有质量问题,则为0
 * @return             1：成功;<0，失败，见LIVESCAN_QUALITY_xxx
 */
NIRVANA_EXPORT
int GAFIS_ExtractMNT_LV_EX(
                           GAFISIMAGESTRUCT *pImage,		//	指纹图像指针
                           void *pMnt,						//	返回的特征数据
                           GAFISIMAGESTRUCT *pBin,			//	返回的纹线图（长度在20K以下）
                           int QualityTh,					//	外部给定的指纹质量阈值(0...100), 0表示为缺省参数
                           int *MosaicErrNo);				//	返回的质量问题的代码（MOSAIC_ERR_XXX， -20到-25）,如果没有质量问题,则为0

/** @brief 重新提取全部特征*/
#define EXTRACTMODE_NEW		0		//重新提取全部特征
/** @brief 在原有特征基础上更新特征*/
#define EXTRACTMODE_UPDATE	1		//在原有特征基础上更新特征
/** @brief 检查原有特征的错误*/
#define EXTRACTMODE_CHECK	2		//检查原有特征的错误

/** @brief 自动更新特征（在现场处理EXTRACTMODE_UPDATE模式下，目前建议用这个选项）*/
#define EXTRACTMNT_AUTO		0x00000001	//自动更新特征（在现场处理EXTRACTMODE_UPDATE模式下，目前建议用这个选项）
/** @brief 使用新的细节特征*/
#define EXTRACTMNT_MINUTIA_NEW	0x00000002	//使用新的细节特征
/** @brief 补充新的细节特征*/
#define EXTRACTMNT_MINUTIA_ADD	0x00000004	//补充新的细节特征
/** @brief 使用新的形态特征（指纹：纹型、中心、副中心、三角、指纹方向，掌纹：第一屈肌褶纹、腕部三角、掌纹方向、花纹中心、花纹三角）*/
#define EXTRACTMNT_MORPHO_NEW	0x00000008	//使用新的形态特征（指纹：纹型、中心、副中心、三角、指纹方向，掌纹：第一屈肌褶纹、腕部三角、掌纹方向、花纹中心、花纹三角）
/** @brief 补充新的形态特征*/
#define EXTRACTMNT_MORPHO_ADD	0x00000010	//补充新的形态特征
/** @brief 使用新的纹线信息*/
#define EXTRACTMNT_RIDGE_NEW	0x00000020	//使用新的纹线信息
/** @brief 补充新的纹线信息（暂时不用）*/
#define EXTRACTMNT_RIDGE_ADD	0x00000040	//补充新的纹线信息（暂时不用）
/** @brief 使用新的区域信息（模糊区、清晰区域、区域方向等）*/
#define EXTRACTMNT_AREAINFO_NEW	0x00000080	//使用新的区域信息（模糊区、清晰区域、区域方向等）
/** @brief 补充新的区域信息（暂时不用）*/
#define EXTRACTMNT_AREAINFO_ADD	0x00000100	//补充新的区域信息（暂时不用）

/** @brief 细节特征漂移	1*/
#define MNTCHK_MINU_SHIFT	0x00000001	//细节特征漂移	1
/** @brief 细节特征遗漏多	2*/
#define MNTCHK_MINU_OMISSION	0x00000002	//细节特征遗漏多	2
/** @brief 细节特征伪特征多	3*/
#define MNTCHK_MINU_FAKE	0x00000004	//细节特征伪特征多	3
/** @brief 指纹中心遗漏	4*/
#define MNTCHK_CORE_MISS	0x00000008	//指纹中心遗漏	4
/** @brief 指纹中心位置不准确5*/
#define MNTCHK_CORE_POSITION	0x00000010	//指纹中心位置不准确5
/** @brief 指纹中心方向不准确6*/
#define MNTCHK_CORE_DIRECTON	0x00000020	//指纹中心方向不准确6
/** @brief 指纹副中心遗漏	 7*/
#define MNTCHK_VCORE_MISS	0x00000040	//指纹副中心遗漏	 7
/** @brief 指纹副中心位置不准确8*/
#define MNTCHK_VCORE_POSITION	0x00000080	//指纹副中心位置不准确8
/** @brief 指纹三角遗漏	 9*/
#define MNTCHK_DELTA_MISS	0x00000100	//指纹三角遗漏	 9
/** @brief 指纹三角位置不准确10*/
#define MNTCHK_DELTA_POSITION	0x00000200	//指纹三角位置不准确10
/** @brief 特征数据不规范	 11*/
#define MNTCHK_DATA_ERROR	0x00000400	//特征数据不规范	 11
/** @brief 指纹形态特征相对位置关系不正确12*/
#define MNTCHK_MORPHO_RELERR	0x00000800	//指纹形态特征相对位置关系不正确12
/** @brief 捺印指纹纹型错误	13*/
#define MNTCHK_RP_ERROR		0x00001000	//捺印指纹纹型错误	13
/** @brief 捺印指纹仅有普通特征信息	14*/
#define MNTCHK_FEAT_ORDINARY	0x00002000	//捺印指纹仅有普通特征信息	14
/** @brief 缺图像		15*/
#define MNTCHK_IMAGE_MISS	0x00004000	//缺图像		15
/** @brief 重复捺印或者指位不匹配	16*/
#define MNTCHK_IMPRESS_ERROR	0x00008000	//重复捺印或者指位不匹配	16


///@删除非感兴趣区内的现场指纹细节特征,修改指纹方向
typedef struct tagNISTMNTTRANSTRUCT
{
	short int 	nWidth;					///<图像宽度	
	short int	nHeight;				///<图像高度
	short int	nResolution;			///<图像分辨率
	short int	fca;					///<指纹掌纹方向
	unsigned short int	D_fca;			///<指纹掌纹方向范围(0-180),当D_fca=0时,表示不设置指纹方向
	UCHAR	nres[6];					///<保留值
	UCHAR	*mntmask;					///<表示感兴趣区,1..特征有效区域,0...无效区域,图像大小为imgwidth*imgheight
	UCHAR	nres_1[32];					///<保留值
}	NISTMNTTRANSTRUCT;

///@brief 特征提取结构
typedef struct tagXGWMNTEXTRACTSTR
{
	GAFISIMAGESTRUCT	*pImage;		///<待处理的图像数据
	void			*pMnt;				///<输入输出的特征信息
	GAFISIMAGESTRUCT	*pBin;			///<输入输出的纹线数据，现场指纹、掌纹提取有数据，捺印没有
	NISTMNTTRANSTRUCT	*pExtPara;		///<有效区域和指掌纹方向信息等
	unsigned char		ExtractMode;	///<处理模式，EXTRACTMODE_XXX
	unsigned char		belatent;		///<是否为现场数据  1...是， 0...不是
	unsigned char		nRes1[6];		///<保留字节
	unsigned int		MntBeUpdated;	///<处理的特征：EXTRACTMNT_XXX  按位记录
	unsigned int		MntCheckErr;	///<特征检查返回的错误信息， MNTCHK_XXX按位记录
	unsigned char		nRes2[40];		///<保留字节
}	XGWMNTEXTRACTSTR;


//	函数返回值
//		1	......	成功
//		<0	......	失败    KERROR-XXX

/**
 * 活体指纹采集以及质量判断
 * @param MntExtStr	   特征提取结构
 * @return             1：成功;<0，失败，见 KERROR-XXX
 */
NIRVANA_EXPORT
int	GAFIS_ExtractMNT_All(XGWMNTEXTRACTSTR *MntExtStr);


//add by wumin 20151210
//将原特征转换成新特征
NIRVANA_EXPORT
int GAFIS_Generate_MntNewTTFea( FINGERMNTSTRUCT* pMnt, FINGERMNTSTRUCT_NEWTT* pMntTTFea );

//将原特征转换成新特征
NIRVANA_EXPORT
int GAFIS_Generate_LatMntNewTTFea( FINGERLATMNTSTRUCT* pMnt, FINGERLATMNTSTRUCT_NEWTT* pMntTTFea );

//新的特征提取，pMnt指向FINGERLATMNTSTRUCT_NEWTT结构，此函数和函数GAFIS_ExtractMNT对应
NIRVANA_EXPORT
int GAFIS_ExtractMNT_NewTTFea(GAFISIMAGESTRUCT *pImage,
                     void *pMnt,
                     GAFISIMAGESTRUCT *pBin,
                     int belatent);

//新的特征提取，MntExtStr里的pMnt指向FINGERLATMNTSTRUCT_NEWTT结构，此函数和函数GAFIS_ExtractMNT_All对应
NIRVANA_EXPORT 
int GAFIS_ExtractMNT_All_NewTTFea(XGWMNTEXTRACTSTR *MntExtStr);

//end by wumin 20151210


/*
 typedef struct tagLPMNTSTATICS
 {
 UCHAR	beExist;			//	特征信息是否存在		1...存在，0...空数据
 UCHAR	CoreON;				//	是否有中心				1...有， 0..没有
 UCHAR	VCoreON;			//	是否有副中心			1...有， 0..没有			
 UCHAR	LDeltaON;			//	是否有左三角			1...有， 0..没有
 UCHAR	RDeltaON;			//	是否右三角				1...有， 0..没有
 UCHAR	Err_Rp_Morpho;		//	纹型和形态特征是否匹配	1...有错，0...无误
 UCHAR	Err_Morpho_Rel;		//	形态特征相对关系是否正确	1...有错，0...无误	
 UCHAR	beZeroDegreeCore;	//	是否方向向下的中心		1...是， 0..否
 short	nMntCnt;			//	细节特征个数			
 UCHAR	nResp[14];
 } LPMNTSTATICS;
 */


/*********************	 计算捺印指纹（掌纹）特征点线数关系  *********************/
//	函数返回值
//		1	......	成功
//		<0	......	失败    KERROR-XXX

/**
 * 计算捺印指纹（掌纹）特征点线数关系
 * @param BinData	   纹线图
 * @param mntdat	   特征结构
 * @param mntid		   指定特征标号，若mntid<0,则计算所有特征点跟其最近4个相邻点数线关系
 * @param lcount	   返回的数线关系
 * @param sum	       返回的数线的个数
 * @return             1：成功;<0，失败，见 KERROR-XXX
 */
NIRVANA_EXPORT
int GAFIS_Display_LineCount(
                            GAFISIMAGESTRUCT *BinData,		//	纹线图
                            MNTDISPSTRUCT *mntdat,			//	特征结构
                            int mntid,						//	指定特征标号，若mntid<0,则计算所有特征点跟其最近4个相邻点数线关系
                            LINECNT2MNTSTRUCT *lcount,		//	返回的数线关系
                            int *sum);						//	返回的数线的个数
/******************      活体采集相关函数     *******************************/

/*************    活体指纹特征提取 (texfdll.dll)    *******************/
//	函数返回值
//		>=	......	图象质量（0---100）
//		<0	......	失败   错误代码为KERROR_XXX

/**
 * 活体采集特征提取
 * @param charbuff	   图象指针
 * @param mntdat	   返回的特征数据
 * @param imgwidth	   图象宽度
 * @param imgheight	   图象高度
 * @param bIsPlain	   是否平面采集，1...是，0...否
 * @return             >=0，图象质量（0---100）; <0，失败，见 KERROR-XXX
 */
NIRVANA_EXPORT
int GAFIS_LiveScan_ExtractMNT(
                              unsigned char *charbuff,		//	图象指针
                              void *mntdat,					//	返回的特征数据		
                              int imgwidth,					//	图象宽度
                              int imgheight,					//	图象高度
                              int bIsPlain);					//	是否平面采集，1...是，0...否


NIRVANA_END_API

#endif //_NIRVANA_EXTRACTOR_H_


