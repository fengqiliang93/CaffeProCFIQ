// Copyright (c) 2014 The Nirvana Authors. All rights reserved.

/**
 * 特征相关结构定义
 * @file mnt_def.h
 */

#ifndef _NIRVANA_MNT_DEF_H_
#define _NIRVANA_MNT_DEF_H_

#include <stdio.h>
#include <stdlib.h>
//#include "afisdef.h"
//#include "common.h"

//#define _NISTSTRUCT

#ifdef _NISTSTRUCT
#define	FTPMNTSIZE	248
#define FLPMNTSIZE	80
#else
#define	FTPMNTSIZE	180				//不能动
#define FLPMNTSIZE	80				//不能动
#endif

#define	PTPMNTSIZE	1200
#define	PLPMNTSIZE	512


#define	BLKMNTSIZE	16

#define	LINECOUNTSIZE	24
#define	MNTPOSSIZE		8

#define PALMDELTASIZE		20
#define	PALMPATCORESIZE		16

#define MAXSGPOINTCNT_FINGER	5
#define MAXSGPOINTCNT_PALM		20




#define MAXMINUTIAQLEV		100
#define MAXMINUTIAQLEV2		50


#define WINOFBLOCKDRT	30			//	块窗口大小,被缩放系数2.5整除

#ifdef _NISTSTRUCT
#define MAX_BLOCKDRT	255
#else
#define MAX_BLOCKDRT	16			//	分块方向的最大块数
#endif

#define	MNTPART_FINGER_UPPER	1
#define	MNTPART_FINGER_LOWER	2
#define	MNTPART_FINGER_LEFT		3
#define	MNTPART_FINGER_RIGHT	4

#define	MNTPART_FINGER_UPPER_BITS	1
#define	MNTPART_FINGER_LOWER_BITS	2
#define	MNTPART_FINGER_LEFT_BITS	4
#define	MNTPART_FINGER_RIGHT_BITS	8

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


#define	MNTPART_PALM_FP_BITS	0x10
#define	MNTPART_PALM_IP_BITS	0x100
#define	MNTPART_PALM_OP_BITS	0x1000

//	指根区的小分区
#define	MNTPART_PALM_FP1_BITS	0x1
#define	MNTPART_PALM_FP2_BITS	0x2
#define	MNTPART_PALM_FP3_BITS	0x4
#define	MNTPART_PALM_FP4_BITS	0x8
//	内侧区的小分区
#define	MNTPART_PALM_IPUP_BITS	0x20
#define	MNTPART_PALM_IPMD_BITS	0x40
#define	MNTPART_PALM_IPLW_BITS	0x80
//	外侧区的小分区
#define	MNTPART_PALM_OPUP_BITS	0x200
#define	MNTPART_PALM_OPMD_BITS	0x400
#define	MNTPART_PALM_OPLW_BITS	0x800
/*
#define	MNTPART_PALM_FP_BITS	1
#define	MNTPART_PALM_IP_BITS	2
#define	MNTPART_PALM_OP_BITS	4

#define	MNTPART_PALM_SUBAREA1_BITS	16
#define	MNTPART_PALM_SUBAREA2_BITS	32
#define	MNTPART_PALM_SUBAREA3_BITS	64
#define	MNTPART_PALM_SUBAREA4_BITS	128
 */
#define	DELTACLASS_UNKNOWN		0
#define	DELTACLASS_WRIST		1
#define	DELTACLASS_FINGROOT		2
#define	DELTACLASS_PATTERN		3	// pattern

//脊线类型
#define	RIDGE_NOUSE				0	//	不使用细节特征类型
#define RIDGE_UNKNOWN			1	//	使用特征类型,脊线类型未知
#define RIDGE_BLACK				2	//	脊线为黑线
#define RIDGE_WHITE				3	//	脊线为白线
//细节特征点类型
#define	MTYPE_UNKNOWN			0	//	未知
#define	MTYPE_ENDPOINT			1	//	端点
#define	MTYPE_BIFPOINT			2	//	分叉点

typedef struct {
    unsigned char pcx[2];
    unsigned char pcy[2];
    unsigned char pca; //	0...180,(cz+90)/2,cz为实际的方向(-90,270)
    unsigned char pcr; //	位置半径
    unsigned char pda; //	方向范围
    unsigned char res;
} PALMMORHOFEATURE;

typedef struct tagFEATQLEV {
    //A类：RpQlev>80 ImgQlev>85，B类：RpQlev>70 ImgQlev>75，C类：ImgQlev 50分以上，D类：ImgQlev 50分以下
    unsigned char bUseQlev; //	是否使用特征质量判断，	1...使用，	0...不使用
    unsigned char bIsPlain; //	是否平面指纹,			1...是，	0...不是
    unsigned char ImgQlev; //	指纹图像质量，主要反映纹线清晰程度，0－100
    unsigned char RpQlev; //	形态特征总体质量， 建议80分以下要干预
    unsigned char CoreQlev; //  中心可靠程度，0－100以及255，255代表主纹型下该特征不存在，0代表该特征未被提取（副中心，三角类似）
    unsigned char VCoreQlev; //	副中心可靠程度
    unsigned char LDeltaQlev; //	左三角可靠程度
    unsigned char RDeltaQlev; //  右三角可靠程度
} FEATQLEV; //同mnt2disp.h中FINGERFEATQLEV

typedef struct tagSGPOINT {
    unsigned char x;
    unsigned char y;
    unsigned char z; //	奇异点方向
    unsigned char TypeQlev; //	最高位代表类型，0...中心型，1...三角型,  低7位代表质量0－100
} SGPOINT;

typedef struct tagBLOCKDRT {
    unsigned char x;
    unsigned char y;
    unsigned char z; //	方向－90－90,高位为1表示质量可靠
} BLOCKDRT; //	32*32块的方向

#define _TT_ACCELERATE 1

typedef struct tagDELTADRT {
    UCHAR x;
    UCHAR y; //	三角的位置
    UCHAR z[3]; //	三角三个方向，朝上的为第一个，左边第二，朝右第三，垂直向下为0度，逆时针方向，-90-269
} DELTADRT;

#define  MAXSIZEOFCSL	9

typedef struct tagCORESTREAMLINE {
    UCHAR x;
    UCHAR y;
    UCHAR z;
    UCHAR dw;
    UCHAR off[MAXSIZEOFCSL]; //	纹线方向与中心方向夹角为（i+1）*dw时的距离（沿中心方向的延长线），0表示结束
} CORESTREAMLINE;

typedef struct {
    unsigned char bePalm; //	
    unsigned char beLatent; //	
    unsigned char resolution[2]; //	采集密度，缺省值为500	//
    unsigned char nWidth[2]; //	图象宽度				//
    unsigned char nHeight[2]; //	图象高度				//
    unsigned char offsetX[2]; //	特征提取偏移位置（相对坐标原点,nres下的偏移量）
    unsigned char offsetY[2];
    unsigned char nFingerIndex; //	指位1...10
    unsigned char nres[2]; //	存储密度，缺省值为200   SCALE=resolution/nres
    unsigned char MntVersion; //	特征提取方法(以前版本为0)	
    unsigned char qlev; //	指纹质量：0 到 100 	
    unsigned char rp; /*  指纹纹型
										0.....残缺或不确定或有歧义
										1.....弓形
										2.....左箕
										3.....右箕
										4.....斗
									*/
    unsigned char vrp; //	副纹型，定义同纹型

    unsigned char fca; //	指纹方向，定义同中心方向
    unsigned char D_fca; //	指纹方向角度变化范围,定义同中心方向范围，实际值2*D_fca
    //	有指纹方向时，D_fca值在8和45之间
    //	当D_fca==0代表不用指纹方向

    unsigned char cx; //	cx : 中心实际位置为(SCALE*cx,SCALE*cy) 
    unsigned char cy;
    unsigned char ca; //	ca : 中心实际方向为 2*ca-90, 在[-90,90]区间内
    //	垂直向下为 0，水平向右为 90，水平向左为-90
    unsigned char D_ca;
    unsigned char D_cr;

    unsigned char ex; //	下中心实际位置为(SCALE*ex,SCALE*ey) 
    unsigned char ey;
    unsigned char D_er;
    unsigned char ldx; //	左三角实际位置为(SCALE*ldx,SCALE*ldy) 
    unsigned char ldy;
    unsigned char D_lr;

    unsigned char rdx; //	右三角实际位置为(SCALE*rdx,SCALE*rdy) 
    unsigned char rdy;
    unsigned char D_rr;

    unsigned char cm; //	细节特征数	0---180 */		
    //以上36字节
    unsigned char xx[FTPMNTSIZE]; //	细节特征实际位置为(SCALE*xx,SCALE*yy) */	//
    unsigned char yy[FTPMNTSIZE]; //				
    unsigned char zz[FTPMNTSIZE]; //  细节特征实际方向为 2*zz-90, 在[-90,270)区间内//
    //	垂直向下为 0，水平向右为 90，水平向左为-90

    unsigned char mqlevON; //	使用细节特征评判 1...使用，0...不使用
#ifdef _NISTSTRUCT
    unsigned char mqlev[FTPMNTSIZE / 8]; //	细节特征质量，按位设置，1代表可靠，0代表不可靠
#else
    unsigned char mqlev[24]; //	细节特征质量，按位设置，1代表可靠，0代表不可靠
#endif
    unsigned char mdate[2]; //	生成或变更时间，转换成短整型 YY*32*32+MM*32+DD
    unsigned char nSignature[2]; //	图象标签，用来判定图象特征一致性
    unsigned char bEditedMinutia; //	捺印细节特征是否修改正确
    unsigned char bEditedMorpho; //	捺印形态特征是否修改正确
    FEATQLEV FeatQlev;
    //819+24或819+FTPMNTSIZE/8
#ifdef _NISTSTRUCT
    unsigned char SgCnt; //奇异点个数
    SGPOINT stSgPoint[MAXSGPOINTCNT_FINGER];
    unsigned char mtypeON; //	脊线类型		RIDGE_XXX				
    unsigned char mtype[FTPMNTSIZE / 4]; //	细节特征类型	MTYPE_XXX
    unsigned char BlkCnt;
    BLOCKDRT stBlockDrt[MAX_BLOCKDRT];
    unsigned char MntFill1[2048 - 1 - FTPMNTSIZE / 4 - FTPMNTSIZE / 8 - 43 - 3 * FTPMNTSIZE - sizeof (FEATQLEV) - MAXSGPOINTCNT_FINGER * 4 - 1 - MAX_BLOCKDRT * 3 - 1];
#else
#ifdef NIST_TEST
    unsigned char SgCnt; //奇异点个数
    SGPOINT stSgPoint[MAXSGPOINTCNT_FINGER];
    unsigned char MntFill[640 - 67 - 3 * FTPMNTSIZE - sizeof (FEATQLEV) - MAXSGPOINTCNT_FINGER * 4 - 1]; //	剩余4字节
#else
#ifdef _TT_ACCELERATE
    DELTADRT stLDeltaDrt;
    DELTADRT stRDeltaDrt;
    CORESTREAMLINE stCoreStreamLine;
    unsigned char bUseCoreAreaQlev; //	是否使用中心周围的质量（为TT快速筛选），存储在细节特征空余的地方
    unsigned char MntFill[640 - 68 - 3 * FTPMNTSIZE - sizeof (FEATQLEV) - 2 * sizeof (DELTADRT) - sizeof (CORESTREAMLINE)]; //	剩余1字节	
#else
    unsigned char MntFill[640 - 67 - 3 * FTPMNTSIZE - sizeof (FEATQLEV)]; //	剩余25字节
#endif
#endif
#endif
} FINGERMNTSTRUCT;

typedef struct {
    unsigned char bePalm; //	
    unsigned char beLatent; //	
    unsigned char resolution[2]; //	采集密度，缺省值为500	//
    unsigned char nWidth[2]; //	图象宽度				//
    unsigned char nHeight[2]; //	图象高度				//
    unsigned char offsetX[2]; //	特征提取偏移位置（相对坐标原点,nres下的偏移量）
    unsigned char offsetY[2];
    unsigned char nFingerIndex; //	1...10
    unsigned char nres[2]; //	存储密度，缺省值为200   SCALE=resolution/nres
    unsigned char MntVersion; //	特征提取方法(以前版本为0)	
    unsigned char qlev; //	指纹质量：0 到 100 	
    unsigned char rp; /*  指纹纹型
										0.....残缺或不确定或有歧义
										1.....弓形
										2.....左箕
										3.....右箕
										4.....斗
									*/
    unsigned char vrp; //	副纹型，定义同纹型

    char fca; //	指纹方向，定义同中心方向
    unsigned char D_fca; //	指纹方向角度变化范围,定义同中心方向范围，
    //	有指纹方向时，D_fca值在8和45之间
    //	当D_fca==0代表不用指纹方向

    unsigned char cx; //	cx : 中心实际位置为(SCALE*cx,SCALE*cy) 
    unsigned char cy;
    unsigned char ca; //	ca : 中心实际方向为 2*ca-90, 在[-90,90]区间内
    //	垂直向下为 0，水平向右为 90，水平向左为-90
    unsigned char D_ca;
    unsigned char D_cr;

    unsigned char ex; //	下中心实际位置为(SCALE*ex,SCALE*ey) 
    unsigned char ey;
    unsigned char D_er;
    unsigned char ldx; //	左三角实际位置为(SCALE*ldx,SCALE*ldy) 
    unsigned char ldy;
    unsigned char D_lr;

    unsigned char rdx; //	右三角实际位置为(SCALE*rdx,SCALE*rdy) 
    unsigned char rdy;
    unsigned char D_rr;
    unsigned char bEditedMinutia; //	捺印细节特征是否修改正确
    unsigned char bEditedMorpho; //	捺印形态特征是否修改正确(按位来表示,1..纹型,2..中心,4..副中心,8..左三角,16..右三角)
    FEATQLEV FeatQlev;
    char MntFill[11 - sizeof (FEATQLEV)];
} FINGERFEATURE;

typedef struct {
    unsigned char bePalm; //	
    unsigned char beLatent; //	
    unsigned char resolution[2]; //	采集密度，缺省值为500	
    unsigned char nWidth[2]; //	图象宽度						
    unsigned char nHeight[2]; //	图象高度						
    unsigned char offsetX[2]; //	特征提取偏移位置（相对坐标原点,nres下的偏移量）
    unsigned char offsetY[2];
    unsigned char nres[2]; //	存储密度，缺省值为200   SCALE=dpi/resolution
    unsigned char MntVersion; //	特征提取方法(以前版本为0)

    unsigned char FingerCode[2]; //	候选指位（按位计算）
    unsigned char RpCode; //	候选纹型（按位计算）
    unsigned char fgrp[10]; //	联指纹型（按位计算）					
    unsigned char Distore; /*  变形参数
										0.......未知
										1.......变形小
										2.......变形大
									*/

    unsigned char fca; //指纹方向，定义同中心方向
    unsigned char D_fca; //指纹方向角度变化范围
    unsigned char cfl; /*	左三角置信度
										1...估计三角
										2...虚拟三角
										0...无三角
									*/
    unsigned char cfr; /*	右三角置信度
										1...估计三角
										2...虚拟三角
										0...无三角
									*/
    unsigned char cx;
    unsigned char cy;
    unsigned char ca;
    unsigned char D_ca; //　2*D_ca 为估计中心角度变化范围　 
    unsigned char D_cr; //	SCALE*D_cr 为估计中心区域半径		
    unsigned char ex;
    unsigned char ey;
    unsigned char D_er; //	SCALE*D_er 为估计下中心区域半径		
    unsigned char ldx;
    unsigned char ldy;
    unsigned char D_lr; //	SCALE*D_lr 为估计左三角区域半径		
    unsigned char rdx;
    unsigned char rdy;
    unsigned char D_rr; //	SCALE*D_rr 为估计右三角区域半径		
    unsigned char cm; //
    unsigned char xx[FLPMNTSIZE]; //
    unsigned char yy[FLPMNTSIZE]; //
    unsigned char zz[FLPMNTSIZE];
    unsigned char mpos[FLPMNTSIZE]; //	特征位置，按位设置
    //			mpos|0...上部，1...下部，2...左部，3...右部
    unsigned char mqlevON; //	使用细节特征评判 1...使用，0...不使用
    unsigned char mqlev[FLPMNTSIZE / 8]; //	细节特征质量，按位设置，1代表可靠，0代表不可靠

    /*	确定区域个数(bm)
            确定区域定义为：指纹纹线清晰，标点特征可靠，
            没有遗漏特征和伪特征
            确定区域包含老版本中空白区，即无细节特征的确定区域
     */
    unsigned char bm;
    unsigned char bx[BLKMNTSIZE];
    unsigned char by[BLKMNTSIZE];
    unsigned char br[BLKMNTSIZE];
    /*　结束	*/
    unsigned char lm; //数线点对个数				//
    unsigned char lnum0[LINECOUNTSIZE]; //数线点对的特征标号		//
    unsigned char lnum1[LINECOUNTSIZE]; //数线点对的特征标号		//
    unsigned char lcount[LINECOUNTSIZE]; //线数						//
    //  特征区域设置框信息
    unsigned char mpm;
    unsigned char mptop[MNTPOSSIZE];
    unsigned char mpbottom[MNTPOSSIZE];
    unsigned char mpleft[MNTPOSSIZE];
    unsigned char mpright[MNTPOSSIZE];
    unsigned char mptype[MNTPOSSIZE]; //	设置框代表的区域编码
    //	0...上部，1...下部，2...左部，3...右部
    unsigned char mdate[2]; //	生成或变更时间，转换成短整型 YY*32*32+MM*32+DD
    unsigned char nSignature[2]; //	图象标签，用来判定图象特征一致性
    unsigned char ImgQlev; //	现场指纹质量
#ifdef _NISTSTRUCT
    unsigned char SgCnt; //奇异点个数
    SGPOINT stSgPoint[MAXSGPOINTCNT_FINGER];
    unsigned char mtypeON; //	脊线类型		RIDGE_XXX				
    unsigned char mtype[FLPMNTSIZE / 4]; //	细节特征类型	MTYPE_XXX
    unsigned char BlkCnt;
    BLOCKDRT stBlockDrt[MAX_BLOCKDRT];
    char LatMntFill[2048 - 68 - 3 * LINECOUNTSIZE - 4 * FLPMNTSIZE - 3 * BLKMNTSIZE - 5 * MNTPOSSIZE - FLPMNTSIZE / 4 - MAXSGPOINTCNT_FINGER * 4 - 1 - MAX_BLOCKDRT * 3 - 1]; //60--->65
#else
#ifdef NIST_TEST
    unsigned char SgCnt; //奇异点个数
    SGPOINT stSgPoint[MAXSGPOINTCNT_FINGER];
    unsigned char mtypeON; //	脊线类型		RIDGE_XXX				
    unsigned char mtype[FLPMNTSIZE / 4]; //	细节特征类型	MTYPE_XXX
    unsigned char BlkCnt;
    BLOCKDRT stBlockDrt[MAX_BLOCKDRT];
    unsigned char MntFill[640 - 67 - 3 * LINECOUNTSIZE - 4 * FLPMNTSIZE - 3 * BLKMNTSIZE - 5 * MNTPOSSIZE - MAXSGPOINTCNT_FINGER * 4 - 1 - FLPMNTSIZE / 4 - 1 - MAX_BLOCKDRT * 3 - 1]; //	剩余2字节
#else
    unsigned char LatMntFill[640 - 67 - 3 * LINECOUNTSIZE - 4 * FLPMNTSIZE - 3 * BLKMNTSIZE - 5 * MNTPOSSIZE]; //640－67－72－320－48－40＝93
#endif
#endif

} FINGERLATMNTSTRUCT;

//add by wumin 20151211
#define MAXTTFEASIZE 3320
typedef struct {
   FINGERMNTSTRUCT MNT;
   unsigned char pTTFea[MAXTTFEASIZE];
}FINGERMNTSTRUCT_NEWTT;

typedef struct {
   FINGERLATMNTSTRUCT MNT;
   unsigned char pTTFea[MAXTTFEASIZE];
}FINGERLATMNTSTRUCT_NEWTT;
//end by wumin 20151211

typedef struct {
    unsigned char bePalm; //	
    unsigned char beLatent; //	
    unsigned char resolution[2]; //	采集密度，缺省值为500	//
    unsigned char nWidth[2]; //	图象宽度				//
    unsigned char nHeight[2]; //	图象高度				//
    unsigned char offsetX[2]; //	特征提取偏移位置（相对原图坐标原点）
    unsigned char offsetY[2];
    unsigned char nPalmIndex; //	掌位，0：未知，1：右掌，2：左掌
    unsigned char nres[2]; //	存储密度，缺省值为200   SCALE=resolution/nres
    unsigned char MntVersion; //	特征提取方法(以前版本为0)	
    unsigned char qlev; //	掌纹质量：0 到 100 	
    unsigned char basepos[2]; //	基线位置
    unsigned char wiptx[2]; //	第一曲肌褶纹内侧位置
    unsigned char wipty[2];
    unsigned char woptx[2]; // 	第一曲肌褶纹外侧位置
    unsigned char wopty[2];

    unsigned char fca; //	掌纹方向，定义同中心方向
    unsigned char D_fca; //	掌纹方向角度变化范围,定义同中心方向范围，
    //	有掌纹方向时，D_fca值在8和45之间
    //	当D_fca==0代表不用掌纹方向
    /*
            unsigned char	FPTrp;			//	指根区纹型信息0：未知,1：存在,2：不存在
            unsigned char	IPTrp;			//	内侧区纹型信息0：未知,1：存在,2：不存在
            unsigned char	OPTrp;			//	外侧区纹型信息0：未知,1：存在,2：不存在   //30字节
     */
    unsigned char nres1[3]; //	删掉分区纹型信息
    unsigned char nDeltaCnt; //	三角个数

    unsigned char dx[PALMDELTASIZE][2];
    unsigned char dy[PALMDELTASIZE][2];
    unsigned char dz[PALMDELTASIZE]; //	0...180,(cz+90)/2,cz为实际的方向(-90,270)
    unsigned char dr[PALMDELTASIZE]; //	位置半径
    unsigned char D_dz[PALMDELTASIZE]; //	方向范围
    unsigned char dtype[PALMDELTASIZE]; //	三角类型（腕部、指根或花纹）
    //	unsigned char	dfpt[PALMDELTASIZE];		//	指根三角（按位表示，从右到左0...食指，3...小指）
    unsigned char nres2[PALMDELTASIZE]; //	删掉指根三角信息

    unsigned char nCoreCnt; //	花纹中心个数

    unsigned char cx[PALMPATCORESIZE][2];
    unsigned char cy[PALMPATCORESIZE][2];
    unsigned char cz[PALMPATCORESIZE]; //	0...180,(cz+90)/2,cz为实际的方向(-90,270)
    unsigned char cr[PALMPATCORESIZE]; //	位置半径
    unsigned char D_cz[PALMPATCORESIZE]; //	方向范围

    unsigned char cm[2]; //	细节特征数	0---150 */						//
    unsigned char xx[PTPMNTSIZE][2]; //	细节特征实际位置为(SCALE*xx,SCALE*yy) */	//
    unsigned char yy[PTPMNTSIZE][2]; //				
    unsigned char zz[PTPMNTSIZE]; //  细节特征实际方向为 2*zz-90, 在[-90,270)区间内//
    //	垂直向下为 0，水平向右为 90，水平向左为-90
    unsigned char mqlevON; //	使用细节特征评判 1...使用，0...不使用
    unsigned char mqlev[(PTPMNTSIZE + 4) / 8]; //	细节特征质量，按位设置，1代表可靠，0代表不可靠
    unsigned char mdate[2]; //	生成或变更时间，转换成短整型 YY*32*32+MM*32+DD
    unsigned char nSignature[2]; //	图象标签，用来判定图象特征一致性
    unsigned char bEditedMinutia; //	捺印细节特征是否修改正确
    unsigned char bEditedMorpho; //	捺印形态特征是否修改正确
    unsigned char MntFill[8192 - 43 - 9 * PALMDELTASIZE - PALMPATCORESIZE * 7 - 5 * PTPMNTSIZE - (PTPMNTSIZE + 4) / 8]; //  35--->39
} PALMMNTSTRUCT;

typedef struct {
    unsigned char bePalm; //	
    unsigned char beLatent; //	
    unsigned char resolution[2]; //	采集密度，缺省值为500	//
    unsigned char nWidth[2]; //	图象宽度				//
    unsigned char nHeight[2]; //	图象高度				//
    unsigned char offsetX[2]; //	特征提取偏移位置（相对原图坐标原点）
    unsigned char offsetY[2];
    unsigned char nPalmIndex; //	掌位，0：未知，1：右掌，2：左掌
    unsigned char nres[2]; //	存储密度，缺省值为200   SCALE=resolution/nres
    unsigned char MntVersion; //	特征提取方法(以前版本为0)	
    unsigned char Distore; //  变形参数

    unsigned char basepos[2]; //	基线位置
    unsigned char woptx[2]; // 	第一曲肌褶纹外侧位置
    unsigned char wopty[2];
    unsigned char woptz; //	第一曲肌褶纹的方向（指向手心方向）

    unsigned char fca; //	掌纹方向，定义同中心方向
    unsigned char D_fca; //	掌纹方向角度变化范围,定义同中心方向范围，
    //	有掌纹方向时，D_fca值在8和45之间（实际值为15--90）
    //	当D_fca==0代表不用掌纹方向
    /*
            unsigned char	FPTrp;			//	指根区纹型信息0：未知,1：存在,2：不存在
            unsigned char	IPTrp;			//	内侧区纹型信息0：未知,1：存在,2：不存在
            unsigned char	OPTrp;			//	外侧区纹型信息0：未知,1：存在,2：不存在   //30字节
     */
    unsigned char nres1[3];
    unsigned char nDeltaCnt; //	三角个数

    unsigned char dx[PALMDELTASIZE][2];
    unsigned char dy[PALMDELTASIZE][2];
    unsigned char dz[PALMDELTASIZE]; //	0...180,(cz+90)/2,cz为实际的方向(-90,270)
    unsigned char dr[PALMDELTASIZE]; //	位置半径
    unsigned char D_dz[PALMDELTASIZE]; //	方向范围
    unsigned char dtype[PALMDELTASIZE]; //	三角类型（腕部、指根或花纹）
    //	unsigned char	dfpt[PALMDELTASIZE];		//	指根三角（按位表示，从右到左0...食指，3...小指）
    unsigned char nres2[20];

    unsigned char nCoreCnt; //	花纹中心个数

    unsigned char cx[PALMPATCORESIZE][2];
    unsigned char cy[PALMPATCORESIZE][2];
    unsigned char cz[PALMPATCORESIZE]; //	0...180,(cz+90)/2,cz为实际的方向(-90,270)
    unsigned char cr[PALMPATCORESIZE]; //	位置半径
    unsigned char D_cz[PALMPATCORESIZE]; //	方向范围
    unsigned char cm[2]; //	细节特征数	0---150 */						//
    unsigned char xx[PLPMNTSIZE][2]; //	细节特征实际位置为(SCALE*xx,SCALE*yy) */	//
    unsigned char yy[PLPMNTSIZE][2]; //				
    unsigned char zz[PLPMNTSIZE]; //  细节特征实际方向为 2*zz-90, 在[-90,270)区间内//
    //	垂直向下为 0，水平向右为 90，水平向左为-90
    unsigned char mpos[PLPMNTSIZE]; //	按位表示 指根区--128，内侧区--64 外侧区--32
    unsigned char mqlevON; //	使用细节特征评判 1...使用，0...不使用
    unsigned char mqlev[(PLPMNTSIZE + 4) / 8]; //	细节特征质量，按位设置，1代表可靠，0代表不可靠
    unsigned char bm;
    unsigned char bx[BLKMNTSIZE][2];
    unsigned char by[BLKMNTSIZE][2];
    unsigned char br[BLKMNTSIZE];
    /*　结束	*/
    unsigned char lm; //数线点对个数				//
    unsigned char lnum0[LINECOUNTSIZE][2]; //数线点对的特征标号		//
    unsigned char lnum1[LINECOUNTSIZE][2]; //数线点对的特征标号		//
    unsigned char lcount[LINECOUNTSIZE]; //线数						//
    //  特征区域设置框信息
    unsigned char mpm;
    unsigned char mptop[MNTPOSSIZE][2];
    unsigned char mpbottom[MNTPOSSIZE][2];
    unsigned char mpleft[MNTPOSSIZE][2];
    unsigned char mpright[MNTPOSSIZE][2];
    unsigned char mptype[MNTPOSSIZE]; //	设置框代表的区域编码
    //	0...上部，1...下部，2...左部，3...右部
    unsigned char mdate[2]; //	生成或变更时间，转换成短整型 YY*32*32+MM*32+DD
    unsigned char nSignature[2]; //	图象标签，用来判定图象特征一致性
    unsigned char MntFill[5120 - 41 - PALMDELTASIZE * 9 - PALMPATCORESIZE * 7 - 6 * PLPMNTSIZE - (PLPMNTSIZE + 4) / 8 - 5 * BLKMNTSIZE - 5 * LINECOUNTSIZE - 9 * MNTPOSSIZE]; // 35->39
} PALMLATMNTSTRUCT;


#ifndef _NIST2007

typedef struct {
    unsigned char resolution[2]; //	采集密度，缺省值为500	//
    unsigned char nWidth[2]; //	图象宽度				//
    unsigned char nHeight[2]; //	图象高度				//
    unsigned char offsetX[2]; //	特征提取偏移位置（相对原图坐标原点）
    unsigned char offsetY[2];
    unsigned char nFingerIndex; //	指位1...10
    unsigned char nres[2]; //	存储密度，缺省值为200   SCALE=resolution/nres
    unsigned char MntVersion; //	特征提取方法(以前版本为0)	
    unsigned char qlev; //	指纹质量：0 到 100 	
    unsigned char rp; /*  指纹纹型
										0.....残缺或不确定或有歧义
										1.....弓形
										2.....左箕
										3.....右箕
										4.....斗
									*/
    unsigned char vrp; //	副纹型，定义同纹型

    unsigned char fca; //	指纹方向，定义同中心方向
    unsigned char D_fca; //	指纹方向角度变化范围,定义同中心方向范围，实际值2*D_fca
    //	有指纹方向时，D_fca值在8和45之间
    //	当D_fca==0代表不用指纹方向

    unsigned char cx; //	cx : 中心实际位置为(SCALE*cx,SCALE*cy) 
    unsigned char cy;
    unsigned char ca; //	ca : 中心实际方向为 2*ca-90, 在[-90,90]区间内
    //	垂直向下为 0，水平向右为 90，水平向左为-90
    unsigned char D_ca;
    unsigned char D_cr;

    unsigned char ex; //	下中心实际位置为(SCALE*ex,SCALE*ey) 
    unsigned char ey;
    unsigned char D_er;
    unsigned char ldx; //	左三角实际位置为(SCALE*ldx,SCALE*ldy) 
    unsigned char ldy;
    unsigned char D_lr;

    unsigned char rdx; //	右三角实际位置为(SCALE*rdx,SCALE*rdy) 
    unsigned char rdy;
    unsigned char D_rr;

    unsigned char cm; //	细节特征数	0---150 */						//
    unsigned char xx[FTPMNTSIZE]; //	细节特征实际位置为(SCALE*xx,SCALE*yy) */	//
    unsigned char yy[FTPMNTSIZE]; //				
    unsigned char zz[FTPMNTSIZE]; //  细节特征实际方向为 2*zz-90, 在[-90,270)区间内//
    //	垂直向下为 0，水平向右为 90，水平向左为-90

    unsigned char mqlevON; //	使用细节特征评判 1...使用，0...不使用
    unsigned char mqlev[24]; //	细节特征质量，按位设置，1代表可靠，0代表不可靠
    unsigned char mdate[2]; //	生成或变更时间，转换成短整型 YY*32*32+MM*32+DD
    unsigned char nSignature[2]; //	图象标签，用来判定图象特征一致性
    char MntFill[640 - 63 - 3 * FTPMNTSIZE]; //	59-->63
} FINGERMNTSTRUCT50;

typedef struct {
    unsigned char resolution[2]; //	采集密度，缺省值为500	//
    unsigned char nWidth[2]; //	图象宽度				//
    unsigned char nHeight[2]; //	图象高度				//
    unsigned char offsetX[2]; //	特征提取偏移位置（相对原图坐标原点）
    unsigned char offsetY[2];
    unsigned char nFingerIndex; //	1...10
    unsigned char nres[2]; //	存储密度，缺省值为200   SCALE=resolution/nres
    unsigned char MntVersion; //	特征提取方法(以前版本为0)	
    unsigned char qlev; //	指纹质量：0 到 100 	
    unsigned char rp; /*  指纹纹型
										0.....残缺或不确定或有歧义
										1.....弓形
										2.....左箕
										3.....右箕
										4.....斗
									*/
    unsigned char vrp; //	副纹型，定义同纹型

    char fca; //	指纹方向，定义同中心方向
    unsigned char D_fca; //	指纹方向角度变化范围,定义同中心方向范围，
    //	有指纹方向时，D_fca值在8和45之间
    //	当D_fca==0代表不用指纹方向

    unsigned char cx; //	cx : 中心实际位置为(SCALE*cx,SCALE*cy) 
    unsigned char cy;
    unsigned char ca; //	ca : 中心实际方向为 2*ca-90, 在[-90,90]区间内
    //	垂直向下为 0，水平向右为 90，水平向左为-90
    unsigned char D_ca;
    unsigned char D_cr;

    unsigned char ex; //	下中心实际位置为(SCALE*ex,SCALE*ey) 
    unsigned char ey;
    unsigned char D_er;
    unsigned char ldx; //	左三角实际位置为(SCALE*ldx,SCALE*ldy) 
    unsigned char ldy;
    unsigned char D_lr;

    unsigned char rdx; //	右三角实际位置为(SCALE*rdx,SCALE*rdy) 
    unsigned char rdy;
    unsigned char D_rr;

    char MntFill[15];
} FINGERFEATURE50;

typedef struct {
    unsigned char resolution[2]; //	采集密度，缺省值为500	
    unsigned char nWidth[2]; //	图象宽度						
    unsigned char nHeight[2]; //	图象高度						
    unsigned char offsetX[2]; //	特征提取偏移位置（相对原图坐标原点）
    unsigned char offsetY[2];
    unsigned char nres[2]; //	存储密度，缺省值为200   SCALE=dpi/resolution
    unsigned char MntVersion; //	特征提取方法(以前版本为0)	

    unsigned char FingerCode[2]; //	候选指位（按位计算）					
    unsigned char RpCode; //	候选纹型（按位计算）					
    unsigned char fgrp[10]; //	联指纹型（按位计算）					
    unsigned char Distore; /*  变形参数
										0.......未知
										1.......变形小
										2.......变形大
									*/

    unsigned char fca; //指纹方向，定义同中心方向
    unsigned char D_fca; //指纹方向角度变化范围
    unsigned char cfl; /*	左三角置信度
										1...估计三角
										2...虚拟三角
										0...无三角
									*/
    unsigned char cfr; /*	右三角置信度
										1...估计三角
										2...虚拟三角
										0...无三角
									*/
    unsigned char cx;
    unsigned char cy;
    unsigned char ca;
    unsigned char D_ca; //　2*D_ca 为估计中心角度变化范围　 
    unsigned char D_cr; //	SCALE*D_cr 为估计中心区域半径		
    unsigned char ex;
    unsigned char ey;
    unsigned char D_er; //	SCALE*D_er 为估计下中心区域半径		
    unsigned char ldx;
    unsigned char ldy;
    unsigned char D_lr; //	SCALE*D_lr 为估计左三角区域半径		
    unsigned char rdx;
    unsigned char rdy;
    unsigned char D_rr; //	SCALE*D_rr 为估计右三角区域半径		
    unsigned char cm; //
    unsigned char xx[FLPMNTSIZE]; //
    unsigned char yy[FLPMNTSIZE]; //
    unsigned char zz[FLPMNTSIZE];
    unsigned char mpos[FLPMNTSIZE]; //	特征位置，按位设置
    //			mpos|0...上部，1...下部，2...左部，3...右部
    unsigned char mqlevON; //	使用细节特征评判 1...使用，0...不使用
    unsigned char mqlev[FLPMNTSIZE / 8]; //	细节特征质量，按位设置，1代表可靠，0代表不可靠

    /*	确定区域个数(bm)
            确定区域定义为：指纹纹线清晰，标点特征可靠，
            没有遗漏特征和伪特征
            确定区域包含老版本中空白区，即无细节特征的确定区域
     */
    unsigned char bm;
    unsigned char bx[BLKMNTSIZE];
    unsigned char by[BLKMNTSIZE];
    unsigned char br[BLKMNTSIZE];
    /*　结束	*/
    unsigned char lm; //数线点对个数				//
    unsigned char lnum0[LINECOUNTSIZE]; //数线点对的特征标号		//
    unsigned char lnum1[LINECOUNTSIZE]; //数线点对的特征标号		//
    unsigned char lcount[LINECOUNTSIZE]; //线数						//
    //  特征区域设置框信息
    unsigned char mpm;
    unsigned char mptop[MNTPOSSIZE];
    unsigned char mpbottom[MNTPOSSIZE];
    unsigned char mpleft[MNTPOSSIZE];
    unsigned char mpright[MNTPOSSIZE];
    unsigned char mptype[MNTPOSSIZE]; //	设置框代表的区域编码
    //	0...上部，1...下部，2...左部，3...右部
    unsigned char ridgemap[2048];
    unsigned char mdate[2]; //	生成或变更时间，转换成短整型 YY*32*32+MM*32+DD
    unsigned char nSignature[2]; //	图象标签，用来判定图象特征一致性
    char LatMntFill[2880 - 2048 - 64 - 3 * LINECOUNTSIZE - 4 * FLPMNTSIZE - 3 * BLKMNTSIZE - 5 * MNTPOSSIZE]; //60--->64
} FINGERLATMNTSTRUCT50;

typedef struct {
    unsigned char resolution[2]; //	采集密度，缺省值为500	//
    unsigned char nWidth[2]; //	图象宽度				//
    unsigned char nHeight[2]; //	图象高度				//
    unsigned char offsetX[2]; //	特征提取偏移位置（相对原图坐标原点）
    unsigned char offsetY[2];
    unsigned char nPalmIndex; //	掌位，0：未知，1：右掌，2：左掌
    unsigned char nres[2]; //	存储密度，缺省值为200   SCALE=resolution/nres
    unsigned char MntVersion; //	特征提取方法(以前版本为0)	
    unsigned char qlev; //	掌纹质量：0 到 100 	
    unsigned char basepos[2]; //	基线位置
    unsigned char wiptx[2]; //	第一曲肌褶纹内侧位置
    unsigned char wipty[2];
    unsigned char woptx[2]; // 	第一曲肌褶纹外侧位置
    unsigned char wopty[2];

    unsigned char fca; //	掌纹方向，定义同中心方向
    unsigned char D_fca; //	掌纹方向角度变化范围,定义同中心方向范围，
    //	有掌纹方向时，D_fca值在8和45之间
    //	当D_fca==0代表不用掌纹方向
    unsigned char FPTrp; //	指根区纹型信息0：未知,1：存在,2：不存在
    unsigned char IPTrp; //	内侧区纹型信息0：未知,1：存在,2：不存在
    unsigned char OPTrp; //	外侧区纹型信息0：未知,1：存在,2：不存在   //30字节

    unsigned char nDeltaCnt; //	三角个数

    unsigned char dx[PALMDELTASIZE][2];
    unsigned char dy[PALMDELTASIZE][2];
    unsigned char dz[PALMDELTASIZE]; //	0...180,(cz+90)/2,cz为实际的方向(-90,270)
    unsigned char dr[PALMDELTASIZE]; //	位置半径
    unsigned char D_dz[PALMDELTASIZE]; //	方向范围
    unsigned char dtype[PALMDELTASIZE]; //	三角类型（腕部、指根或花纹）
    unsigned char dfpt[PALMDELTASIZE]; //	指根三角（按位表示，从右到左0...食指，3...小指）

    unsigned char nCoreCnt; //	花纹中心个数

    unsigned char cx[PALMPATCORESIZE][2];
    unsigned char cy[PALMPATCORESIZE][2];
    unsigned char cz[PALMPATCORESIZE]; //	0...180,(cz+90)/2,cz为实际的方向(-90,270)
    unsigned char cr[PALMPATCORESIZE]; //	位置半径
    unsigned char D_cz[PALMPATCORESIZE]; //	方向范围

    unsigned char cm[2]; //	细节特征数	0---150 */						//
    unsigned char xx[PTPMNTSIZE][2]; //	细节特征实际位置为(SCALE*xx,SCALE*yy) */	//
    unsigned char yy[PTPMNTSIZE][2]; //				
    unsigned char zz[PTPMNTSIZE]; //  细节特征实际方向为 2*zz-90, 在[-90,270)区间内//
    //	垂直向下为 0，水平向右为 90，水平向左为-90
    unsigned char mqlevON; //	使用细节特征评判 1...使用，0...不使用
    unsigned char mqlev[(PTPMNTSIZE + 4) / 8]; //	细节特征质量，按位设置，1代表可靠，0代表不可靠
    unsigned char mdate[2]; //	生成或变更时间，转换成短整型 YY*32*32+MM*32+DD
    unsigned char nSignature[2]; //	图象标签，用来判定图象特征一致性
    char MntFill[8192 - 39 - 9 * PALMDELTASIZE - PALMPATCORESIZE * 7 - 5 * PTPMNTSIZE - (PTPMNTSIZE + 4) / 8]; //  35--->39
} PALMMNTSTRUCT50;

typedef struct {
    unsigned char resolution[2]; //	采集密度，缺省值为500	//
    unsigned char nWidth[2]; //	图象宽度				//
    unsigned char nHeight[2]; //	图象高度				//
    unsigned char offsetX[2]; //	特征提取偏移位置（相对原图坐标原点）
    unsigned char offsetY[2];
    unsigned char nPalmIndex; //	掌位，0：未知，1：右掌，2：左掌
    unsigned char nres[2]; //	存储密度，缺省值为200   SCALE=resolution/nres
    unsigned char MntVersion; //	特征提取方法(以前版本为0)	
    unsigned char Distore; //  变形参数

    unsigned char basepos[2]; //	基线位置
    unsigned char woptx[2]; // 	第一曲肌褶纹外侧位置
    unsigned char wopty[2];
    unsigned char woptz; //	第一曲肌褶纹的方向（指向手心方向）

    unsigned char fca; //	掌纹方向，定义同中心方向
    unsigned char D_fca; //	掌纹方向角度变化范围,定义同中心方向范围，
    //	有掌纹方向时，D_fca值在8和45之间（实际值为15--90）
    //	当D_fca==0代表不用掌纹方向
    unsigned char FPTrp; //	指根区纹型信息0：未知,1：存在,2：不存在
    unsigned char IPTrp; //	内侧区纹型信息0：未知,1：存在,2：不存在
    unsigned char OPTrp; //	外侧区纹型信息0：未知,1：存在,2：不存在   //30字节

    unsigned char nDeltaCnt; //	三角个数

    unsigned char dx[PALMDELTASIZE][2];
    unsigned char dy[PALMDELTASIZE][2];
    unsigned char dz[PALMDELTASIZE]; //	0...180,(cz+90)/2,cz为实际的方向(-90,270)
    unsigned char dr[PALMDELTASIZE]; //	位置半径
    unsigned char D_dz[PALMDELTASIZE]; //	方向范围
    unsigned char dtype[PALMDELTASIZE]; //	三角类型（腕部、指根或花纹）
    unsigned char dfpt[PALMDELTASIZE]; //	指根三角（按位表示，从右到左0...食指，3...小指）

    unsigned char nCoreCnt; //	花纹中心个数

    unsigned char cx[PALMPATCORESIZE][2];
    unsigned char cy[PALMPATCORESIZE][2];
    unsigned char cz[PALMPATCORESIZE]; //	0...180,(cz+90)/2,cz为实际的方向(-90,270)
    unsigned char cr[PALMPATCORESIZE]; //	位置半径
    unsigned char D_cz[PALMPATCORESIZE]; //	方向范围
    unsigned char cm[2]; //	细节特征数	0---150 */						//
    unsigned char xx[PLPMNTSIZE][2]; //	细节特征实际位置为(SCALE*xx,SCALE*yy) */	//
    unsigned char yy[PLPMNTSIZE][2]; //				
    unsigned char zz[PLPMNTSIZE]; //  细节特征实际方向为 2*zz-90, 在[-90,270)区间内//
    //	垂直向下为 0，水平向右为 90，水平向左为-90
    unsigned char mpos[PLPMNTSIZE]; //	按位表示 指根区--128，内侧区--64 外侧区--32
    unsigned char mqlevON; //	使用细节特征评判 1...使用，0...不使用
    unsigned char mqlev[(PLPMNTSIZE + 4) / 8]; //	细节特征质量，按位设置，1代表可靠，0代表不可靠
    unsigned char bm;
    unsigned char bx[BLKMNTSIZE][2];
    unsigned char by[BLKMNTSIZE][2];
    unsigned char br[BLKMNTSIZE];
    /*　结束	*/
    unsigned char lm; //数线点对个数				//
    unsigned char lnum0[LINECOUNTSIZE][2]; //数线点对的特征标号		//
    unsigned char lnum1[LINECOUNTSIZE][2]; //数线点对的特征标号		//
    unsigned char lcount[LINECOUNTSIZE]; //线数						//
    //  特征区域设置框信息
    unsigned char mpm;
    unsigned char mptop[MNTPOSSIZE][2];
    unsigned char mpbottom[MNTPOSSIZE][2];
    unsigned char mpleft[MNTPOSSIZE][2];
    unsigned char mpright[MNTPOSSIZE][2];
    unsigned char mptype[MNTPOSSIZE]; //	设置框代表的区域编码
    //	0...上部，1...下部，2...左部，3...右部
    unsigned char ridgemap[15360];
    unsigned char mdate[2]; //	生成或变更时间，转换成短整型 YY*32*32+MM*32+DD
    unsigned char nSignature[2]; //	图象标签，用来判定图象特征一致性
    char MntFill[20480 - 15360 - 39 - PALMDELTASIZE * 9 - PALMPATCORESIZE * 7 - 6 * PLPMNTSIZE - (PLPMNTSIZE + 4) / 8 - 5 * BLKMNTSIZE - 5 * LINECOUNTSIZE - 9 * MNTPOSSIZE]; // 35->39
} PALMLATMNTSTRUCT50;


#define MARKOFINNERMNT		254	//	内部特征的标志
#define INNERMNT_MNTCNT		0	//	内部特征的总数
#define INNERMNT_SGPOINT	1	//	奇异点
#define INNERMNT_MINUTIA1	2	//	可靠细节特征点，参与匹配
#define	INNERMNT_MINUTIA0	3	//	疑似特征点，影响匹配得分
#define INNERMNT_CFAREA		4	//	确定区域	
#define INNERMNT_MNTEVALUE  5	//  特征评价信息
#define INNERMNT_MINUTIAADJ	6	//	需要调整方向的特征

#define MNTCONF_UNSET		0	// 未设置
#define MNTCONF_TRUE		1	// 确定特征
#define MNTCONF_MAYBE		2	// 疑似特征
#define MNTCONF_FALSE		3	// 错误特征


#define MINUTIAERR_NORMAL	0	// 细节特征正常		
#define MINUTIAERR_SHIFT	1	// 细节特征平移
#define MINUTIAERR_DIRECT	2	// 细节特征方向错
#define MINUTIAERR_FAKE		4	// 细节特征伪
#define MINUTIAERR_OMISSION	8	// 细节特征遗漏

typedef struct tagAFISCIRCLE {
    int cx;
    int cy;
    int radius;
} AFISCIRCLE;

typedef struct tagMNTEVALUESTR {
    UCHAR Conf_CorePos; // MNTCONF_XXX
    UCHAR Conf_CoreDrt; // MNTCONF_XXX
    UCHAR Conf_VCorePos; // MNTCONF_XXX
    UCHAR Conf_LDeltaPos; // MNTCONF_XXX
    UCHAR Conf_RDeltaPos; // MNTCONF_XXX
    UCHAR MinutiaErr; // MINUTIAERR_XXX位运算
    char ShiftX; // 竖直平移值（尺度变换过），原坐标值减这个值为实际值
    char ShiftY; // 横向平移值（尺度变换过），原坐标值减这个值为实际值
} MNTEVALUESTR;

typedef struct tagLPINNERMNTSTR {
    UCHAR nMntCnt_Add1; //	补充的可靠细节特征数
    UCHAR nMntCnt_Add0; //	补充的疑似细节特征数
    UCHAR nMntCnt_Adj; //	需调整方向的细节特征个数
    UCHAR SgCnt; //	奇异点个数
    UCHAR nCAreaCnt; //	确定区域个数
    UCHAR nRes[3];
    MNTEVALUESTR stFgMntValue; //	特征自动评价信息
    UCHAR xx1[FLPMNTSIZE]; //	补充的可靠细节特征信息
    UCHAR yy1[FLPMNTSIZE]; //	
    UCHAR zz1[FLPMNTSIZE]; //	
    UCHAR xx0[FLPMNTSIZE]; //	补充的疑似细节特征信息
    UCHAR yy0[FLPMNTSIZE]; //	
    UCHAR zz0[FLPMNTSIZE]; //	
    UCHAR Adj_Indx[FLPMNTSIZE]; //	需调整方向的细节特征下标
    UCHAR Adj_zz[FLPMNTSIZE]; //	需调整方向的细节特征的方向
    AFISCIRCLE CAreaArray[BLKMNTSIZE]; //	确定区域信息
    SGPOINT SgPoint[MAXSGPOINTCNT_FINGER]; //	奇异点信息
    UCHAR nres[48];
} LPINNERMNTSTR;

typedef struct tagLPMNTSTATICS {
    UCHAR beExist; //	特征信息是否存在		1...存在，0...空特征
    UCHAR CoreON; //	是否有中心				1...有， 0..没有
    UCHAR VCoreON; //	是否有副中心			1...有， 0..没有			
    UCHAR LDeltaON; //	是否有左三角			1...有， 0..没有
    UCHAR RDeltaON; //	是否右三角				1...有， 0..没有
    UCHAR Err_Rp_Morpho; //	纹型和形态特征是否匹配	1...有错，0...无误
    UCHAR Err_Morpho_Rel; //	形态特征相对关系是否正确	1...有错，0...无误	
    UCHAR beZeroDegreeCore; //	是否方向向下的中心		1...是， 0..否
    short nMntCnt; //	细节特征个数			1...有， 0..没有
    UCHAR nResp[14];
} LPMNTSTATICS;


#endif  //_NIST2007

#endif //_NIRVANA_MNT_DEF_H_
