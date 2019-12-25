// Copyright (c) 2014 The Nirvana Authors. All rights reserved.

/*
 * 针对特征数据的检测
 * @file mnt_checker.h
 */

#ifndef _NIRVANA_MNT_CHECKER_H_
#define _NIRVANA_MNT_CHECKER_H_
#include "mnt_checker_def.h"

NIRVANA_BEGIN_API

#define	MNTVALID_OK			1
#define	MNTVALID_WARNING	-1
#define	MNTVALID_SUGGEST	-2


//现场错误信息代码
#define	MNTERRNO_NONAMEERR		-1
#define	MNTERRNO_MNTCOUNT		-2
#define	MNTERRNO_POSITION		-3
#define MNTERRNO_RPANDFEATURE	-4
#define MNTERRNO_RPMATCH		-5
#define MNTERRNO_MISSFEATURE	-6
#define MNTERRNO_EXACTAREA		-7
#define MNTERRNO_NORP			-8

#define MNTERRNO_FEATURERANGE1	-20
#define	MNTERRNO_FEATURERANGE2	-21
#define	MNTERRNO_MNTPOSITION	-22
#define	MNTERRNO_MNTDISPERSE	-23


/////////// /       检查特征人工标点是否准确规范    //////////////
NIRVANA_EXPORT int	GAFIS_MntDispMnt_beValid(MNTDISPSTRUCT *pmnt,		//	待检查的特征数据
									 char *errinfo);			//	返回的中文提示信息
/*	
	函数返回值：MNTVALID_OK...合理，
				MNTVALID_WARNING...警告信息
				MNTVALID_SUGGEST...提示信息
*/

////////////        检查特征人工标点是否准确规范    //////////////
NIRVANA_EXPORT int  GAFIS_MntDispMnt_beValid_EX(MNTDISPSTRUCT *pmnt,	//	待检查的特征数据
										char *errinfo1,			//	返回的中文提示信息
										char *errinfo2);		//	返回的英文提示信息
/*	
	函数返回值：MNTVALID_OK...合理，
				MNTVALID_WARNING...警告信息
				MNTVALID_SUGGEST...提示信息
*/
////////////        检查特征人工标点是否准确规范    //////////////
NIRVANA_EXPORT int GAFIS_MntDispMnt_beValid_New(MNTDISPSTRUCT *pmnt,		//	待检查的特征数据
										int *MntErrNo);				//	返回的错误警告或者提示代码 MNTERRNO_XXX
/*	
函数返回值：MNTVALID_OK...合理，
MNTVALID_WARNING...警告信息
MNTVALID_SUGGEST...提示信息
*/

NIRVANA_EXPORT void	GAFIS_MntDispMnt_Initial(MNTDISPSTRUCT *pmnt,int bIsPalm,int bIsLatent,int nWidth,int nHeight,int nResolution);
NIRVANA_EXPORT void	GAFIS_Finger_TenprintFeatureStdToMntDisp(FINGERFEATURE *mnt, MNTDISPSTRUCT *pmnt);
NIRVANA_EXPORT int	GAFIS_Finger_TenprintMntCombine(FINGERMNTSTRUCT *mntnew, GAFISIMAGESTRUCT *ridgemap,FINGERMNTSTRUCT *mntold);
NIRVANA_EXPORT int	GAFIS_Finger_TenprintMntCombine_ex(FINGERMNTSTRUCT *mntstd, MNTDISPSTRUCT *mntdisp);
NIRVANA_EXPORT void GAFIS_TPMnt30ToTenprintMntStd(void *mnt30,void *mntdat);
NIRVANA_EXPORT void GAFIS_LPMnt30ToLatentMntStd(void *mnt30,void *latmnt);
NIRVANA_EXPORT int GAFIS_CprStdToTpCpr30(unsigned char *VBitCode,int codelen);
NIRVANA_EXPORT
void GAFIS_LpMnt_Statics_Check(FINGERLATMNTSTRUCT *pMnt, LPMNTSTATICS *pSta);
//  返回值： 1...成功， -1...失败（数据长度不够或不是GAFIS标准数据）
// VBitCode...压缩数据指针（不包括头结构）
// codelen...压缩数据长度


NIRVANA_END_API

#endif //_NIRVANA_MNT_CHECKER_H_

