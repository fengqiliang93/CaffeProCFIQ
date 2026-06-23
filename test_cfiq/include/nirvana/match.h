// Copyright (c) 2014 The Nirvana Authors. All rights reserved.

/**
 * 比对使用的头文件
 * @file match.h
 */

#ifndef _NIRVANA_MATCH_H_
#define	_NIRVANA_MATCH_H_

NIRVANA_BEGIN_API

/**
 * 匹配关系结构
 */
typedef struct tagMATCHINFO
{
	int				dx,dy,dw;
	int				sum;
	short int		latc[1200];
	short int		tenc[1200];
	int				xl,yl,xt,yt;
#ifdef _VERSION60		
	/** 匹配的尺度缩放系数(纵向,返回) */
	float			scalx;		
	/** 匹配的尺度缩放系数(横向,返回) */
	float			scaly;
#endif
} MATCHINFO;

/**
 * 细节点位置结构
 */
typedef struct tagMINUTIAPOS
{
	int				x;///< x轴坐标
	int				y;///< y轴坐标
	int				z;///< 角度
}	MINUTIAPOS;

/**
 * 匹配数据结构
 */
typedef struct tagMATCHDATASTRUCT
{
	/** 比对数据类型				0...指纹,1...掌纹 */
	int				bePalm;					  
	/**
	 *  匹配特征基数				1... 低,2... 较低, 3...中等(缺省),4...较高, 5...高  	
	 *	说明：  匹配上的特征数目对评分的影响,特别是倒查时候候选结果对此参数比较敏感
	 *			数值越低,有利于匹配特征点少但特征吻合很好的指纹
	 *			数值越高,有利于匹配上的特征较多的指纹,
	 */
	int				MaMth_Minutia;			
	/** 变形程度					1... 小,2... 较小,3...中等(缺省),4...较大,  5...大	 */
	int				MaMth_Distore;				
	/**
	 * 容错能力			1... 低,2... 较低,3...中等(缺省),4...较高, 5...高   
	 * 说明：数值越低对伪点和漏点的容错能力也越差,但分数差异性也越大
	 *		 数值越高对伪点和漏点的容错能力越好,但分数差异也越小
	 */
	int				MaMth_LocStructure;		
	/** 
	 * 屏蔽增强型现场特征			0...否(缺省),1...是
	 * 说明：增强型特征包括模糊区、重点区域、数线信息	
	 */
	int				MaMth_MaskEnhFeat;		
	/** 手纹方向变化范围(暂时不用)0...缺省 */
	int				AngleVar;					
	/** 待查手纹特征的指针数组 */
	void			**srcMntData;				
	/** 待查手纹纹线的指针数组 */
	GAFISIMAGESTRUCT	**srcRidgeData;				
	/** 
	 * 	TT：待查和档案手纹的个数,一般情况下指纹为10,掌纹是2      
	 *	TL: 待查的手纹的个数,一般情况下指纹为10,掌纹是2   (档案手纹个数缺省为1)
	 *	LT: 档案手纹的个数				                    (待查手纹个数缺省为1)	  
	 *	LL: 待查和档案手纹个数都缺省为1
	 */
	int				nMntCnt	;				
	/** 1....TT二次比对时候采用候选内部交叉比对,0...不采用 */
	UCHAR			CrossMatch_TT;				
	/** 数据缩放比例范围(0.5---2.0,Scale1>=Scale0),如果两个值都是0.0,表示原大 */
	UCHAR			nRes1[3];
	/** 数据缩放比例范围(0.5---2.0,Scale1>=Scale0),如果两个值都是0.0,表示原大 */
	double			Scale0;						
	/**  */
	double			Scale1;
	/** 待查数据的指位或掌位,只对倒查起作用 */
	UCHAR			*srcIndex;					
	/** 是否精确进行形态筛选,0...按指纹实际条件使用,1...忽略形态特征变化范围,精确使用 */
	UCHAR			MaMth_MorphAccuUse;			
	/** 待查数据的关键字 */
	char			SrcKey[32];					
	/** 是否进行全面比对,0...否(缺省),1...是,当需要给即时比对提供候选或进行即时比对时,需要选择1 */
	UCHAR			FullMatchOn;				
	/** 是否使用自动提取的内部特征来进行比对, 0(缺省)....根据配置文件设置, 1...使用,2...不使用 */
	UCHAR			MatchUseInnerMnt;			
	/** 保留1个字节 */
	UCHAR			bnRes1[1];				 
	/** srcMntData[i]对应源数据的序号,,序号为0-15.当pnItemIndex为空的时候,缺省认为srcMntData为同组源数据 */
	UCHAR			*pnItemIndex;			 
	/** 保留字节 */
	UCHAR			nRes2[27-sizeof(UCHAR *)];					
	/** 测试数据 */
	UCHAR			TestOn;						
}	MATCHDATASTRUCT;

#define	GSCH_DESTMNTCNT	22

/**
 * 指向数据的指针结构,此指针可能是指向数据的指针(pdata),也可能是指向数据的数组(ppdata)
 */
typedef	union	tagGSCH_COLPT
{
	/** 指向数据的指针,这个时候rowWidth是数据本身的长度 */
	UCHAR			*pData; 
	/** 指向数据指针数组,这个时候的rowWidth是指针的长度 */
	UCHAR			**ppData;
} GSCH_COLPT;

/**
 * 定义列数据格式,大小是12或16bytes
 * @code 
 * GSCH_COLDATA	cd;
 *
 * p = cd.pData+k*nRowWidth+nOffsetInRow is the actual pointer
 * @endcode
 */
typedef	struct	tagGSCH_COLDATA
{
  /** 指向数据的指针定义,此指针可能是指向数据的指针(pdata),也可能是指向数据的数组(ppdata) */
	GSCH_COLPT		stPt;
  /** 在遍历数据时候的行的宽度,如果是指向数据那么,宽度是数据本身,如果是指向数据数组,宽度是指针的宽度 */
	int				nRowWidth;
  /** 标示是使用ppdata还是pData */
	int				bUse_ppData;
} GSCH_COLDATA;	//


/**
 * 此结构是比对的核心数据结构,512bytes.
 */
typedef	struct	tagGSCH_DESTBLOCKDATA
{
  /** 特征数据 */
	GSCH_COLDATA	stMnt[GSCH_DESTMNTCNT];
  /** 纹线数据,可以为null,192bytes */
	GSCH_COLDATA	stRidge[GSCH_DESTMNTCNT];	
  /** ?,可以为空,16bytes */
	GSCH_COLDATA	stKey;
  /** 对应的指纹卡片,卡号,16bytes */
	GSCH_COLDATA	stSID;	
  /** 定义对应的特征数据是否需要参入比对 */
	GSCH_COLDATA	stIsRowUsed;
  /** 定义是否使用指位信息 */
	GSCH_COLDATA	stFgPos;
  /** 卡中指纹的数目 */
	int				nMntCnt;
  /** 特征的起始位置 */
	int				nStartIdx;
  /** 特征的终止位置 */
	int				nEndIdx;
  /** 每个特征数据对应的指位信息 */
	UCHAR			nFingIndex[GSCH_DESTMNTCNT];
} GSCH_DESTBLOCKDATA;


/**
 * 目标数据的结构
 */
typedef	struct	tagGSCH_DESTDATA
{
	/** 块数据 */
	GSCH_DESTBLOCKDATA	*pstBlk;
	/** 块个数 */
	int				nBlkCnt;
} GSCH_DESTDATA;

/**
 * 候选结果结构, 16 bytes
 * the following structure is used for all mu's 
 * and it will be transformed to GAQUERYCANDSTRUCT when sending result to
 * server. because GAQUERYCANDSTRUCT is too large.
 */
typedef	struct	tagGA_SIMPCAND
{
	UCHAR			nDestDBID[2];
	UCHAR			nDestTID[2];
	/** 候选结果分数 */
	UCHAR			nScore[4];	 
	/** 候选结果SID,6 bytes */
	UCHAR			nSID[6];	 
	/** 指位 */
	UCHAR			nFg;		 
	/** 6.1新增 */
	UCHAR			nSrcKeyIndex;			
#ifdef _NIST2007
	/** 匹配的特征数 */
	uint2			Num_Matching_Minutia;	
	/** 候选的特征数 */
	uint2			Num_Candidate_Minutia;		
	/** 候选的指纹质量,0...100,0表示未知 */
	UCHAR			Candidate_Quality;			
	/** 空余位? */
	UCHAR			nres[3];
#endif
} GA_SIMPCAND;	



/**
 * 比对进行初始化
 * @param mhandle 比对初始化之后的结果存储指针
 * @param matchtype 比对类型 0...查前科,1...倒查,2...正查,3...串查
 * @param BeTwoStepMatch 二次比对标记,0...一次比对,1...2次比对
 * @param MatchDataStr 比对手纹数据以及比对参数
 * @param mdatacnt 比对手纹的个数,通常是1,多指(掌)关联查询时大于1
 * @return >=0	成功、返回比对句柄
 *         <0		失败,错误代码为KERROR_XXX
 */
NIRVANA_EXPORT  int GAFIS_MatchNew_init(void **mhandle,int matchtype,int BeTwoStepMatch,MATCHDATASTRUCT  *MatchDataStr,int mdatacnt);

//add by wumin 20150408
//用来设置粗筛选参数
//posNum是粗匹配要匹配的指位个数，th是粗匹配阈值，默认为g_PosNum = 7, g_MatchTh = 35;
NIRVANA_EXPORT int GAFIS_SetNewTTPosNumAndTh(int posNum, int th);
NIRVANA_EXPORT double Match_TTLP(unsigned char* pFea1, unsigned char* pFea2);
//end by wumin 20150408

/**
 * 执行比对任务
 *
 * @param mhandle 比对初始化之后的结果存储指针
 * @param filedata 档案数据,比对的数据结构见 GSCH_DESTDATA 结构
 * @param mcand 返回的比对结果(内存函数外部申请)
 * @return >0	成功,候选个数
 *         <0	失败,错误代码为KERROR_XXX
 */
NIRVANA_EXPORT  int GAFIS_MatchNew(void *mhandle,GSCH_DESTDATA *filedata,GA_SIMPCAND *mcand);



/**
 * 比对结束,销毁一些内存
 * @param mhandle 比对初始化之后的结果存储指针
 * @return >0 成功
 *         <0	失败,错误代码为KERROR_XXX
 */
NIRVANA_EXPORT  int GAFIS_MatchNew_end(void *mhandle);

/**
 * 捺印指纹前科的形态筛选
 * @param mhandle 比对初始化之后的结果存储指针
 * @param filefeat 目标指纹的特征,详见 FINGERFEATURE 结构
 * @return 1 通过形态筛选,需要进一步细节特征匹配
 *         0 通不过形态筛选,非配对指纹
 *		   <0 失败,错误代码为KERROR_XXX
 */
NIRVANA_EXPORT  int GAFIS_Finger_TTSearchNew_Filter(void *mhandle,FINGERFEATURE **filefeat);

/**
 * 判断查询任务是否需要二次比对
 * @param matchtype 比对类型 0...查前科,1...倒查,2...正查,3...串查
 * @param MatchDataStr 比对手纹数据以及比对参数
 * @param mdatacnt 比对手纹的个数,通常是1,多指(掌)关联查询时大于1
 * @return 1 需要
 *         0 不需要,一次比对直接得到最终结果
 *		   -1 错误参数
 */
NIRVANA_EXPORT  int GAFIS_MatchNew_beNeedTwoStep(int matchtype,MATCHDATASTRUCT  *MatchDataStr,int mdatacnt);

/**
 * 得到一对一的匹配信息
 * @param matchtype 比对类型 0...查前科,1...倒查,2...正查,3...串查
 * @param MatchDataStr 比对手纹数据以及比对参数
 * @param destmnt 目标指纹数据
 * @param ret_info 输出匹配关系数据,详见 MATCHINFO 结构.
 * @return 1 成功
 *		   -1 错误或没有匹配对儿
 */
NIRVANA_EXPORT  int GAFIS_MatchNew_one2one(int matchtype, MATCHDATASTRUCT  *MatchDataStr,void *destmnt,MATCHINFO *ret_info);

/**
 * 用滚动指纹的纹型数据更正平面指纹数据
 * @param plainmnt 平面指纹数据,详见 FINGERMNTSTRUCT 结构
 * @param rollmnt 滚动指纹数据,详见 FINGERMNTSTRUCT 结构
 * @return 1 成功
 *		   -1 失败,错误代码为KERROR_XXX
 */

NIRVANA_EXPORT  int GAFIS_Rectify_PlainMnt(void *plainmnt,void *rollmnt);

/**
 * 指纹验证(1对1)进行初始化
 * @param mhandle 指纹验证初始化之后的结果存储指针
 * @param srcmnt 待验证指纹数据,数据结构见 FINGERMNTSTRUCT 结构
 * @param angle 预留标志位,暂不使用.
 * @return >=0	成功、返回比对句柄
 *         <0		失败,错误代码为KERROR_XXX
 */
NIRVANA_EXPORT int GAFIS_Finger_VerifyNew_init(void **mhandle,void *srcmnt,int angle);

/**
 * 执行指纹验证任务
 * @param mhandle 指纹验证初始化之后的结果存储指针
 * @param tgtmnt 目标指纹数据,数据结构见 FINGERMNTSTRUCT 结构
 * @return >0	成功,匹配分数
 *         <0	失败,错误代码为KERROR_XXX
 */
NIRVANA_EXPORT int GAFIS_Finger_VerifyNew(void *mhandle,void *tgtmnt);

/**
 * 执行指纹验证任务
 * @param mhandle 指纹验证初始化之后的结果存储指针
 * @param mntdat2 目标指纹数据,数据结构见 FINGERMNTSTRUCT 结构
 * @return 0	成功,来自不同手指
 *         1	成功,来自同一手指
 *         <0	失败,错误代码为KERROR_XXX
 */
NIRVANA_EXPORT int GAFIS_Finger_VerifyNew_Same(void *mhandle,void *mntdat2);

/**
 * 指纹验证结束,销毁一些内存
 * @param mhandle 指纹验证初始化之后的结果存储指针
 * @return >0 成功
 *         <0	失败,错误代码为KERROR_XXX
 */
NIRVANA_EXPORT void GAFIS_Finger_VerifyNew_end(void *mhandle);

/**
 * 掌纹匹配1对1
 * @param fea1 特征1
 * @param fea2 特征2
 * @param score 匹配分数
 * return > 0 匹配得分为正
 * return = 0 得分为0
 * return < 0 得分为负
 */
NIRVANA_EXPORT int PalmTT_Test( void *fea1, void *fea2, double *score);

NIRVANA_END_API

#endif	//_NIRVANA_MATCH_H_

