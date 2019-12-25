// Copyright (c) 2014 The Nirvana Authors. All rights reserved.

/**
 * 图像增强类
 * @file img_enhancer.h
 */
#ifndef _NIRVANA_IMG_ENHANCER_H_
#define _NIRVANA_IMG_ENHANCER_H_

NIRVANA_BEGIN_API

//////  初始化增强 (每次改变图象或改变增强方法之前使用） ///////
NIRVANA_EXPORT int GAFIS_Enhance_Init(int opcode,unsigned char *srcimg,int height,int width,unsigned char *tgtimg);
/*
	函数返回值：1		......	成功
				<0		......	失败,错误代码为KERROR_XXX

	参数说明：	opcode	......	增强方法  
								0 ... 锐化
								1 ... 均衡化
				srcimg	......	原图象指针
				height	......	图象高度
				width	......	图象宽度
				tgtimg	......  增强图象指针
*/

/////////    依据给定的对比度和亮度参数进行图象增强    /////////
NIRVANA_EXPORT int GAFIS_Enhance(unsigned char *srcimg,int height,int width,float contrast,float brightness,unsigned char *tgtimg);
/* 
	函数返回值：1		......	成功
				<0		......	失败,错误代码为KERROR_XXX

	参数说明：	srcimg	......	原图象指针
				height	......	图象高度
				width	......	图象宽度
				contract......  对比度（0---1.0）
				bright	......	亮度  （0---1.0）
				tgtimg	......	增强图象指针
*/

/////////              结束图象增强               /////////
//    增强图像改变或者增强方法改变时候都需要调用该函数   //
NIRVANA_EXPORT void GAFIS_Enhance_Finished();


typedef struct tagIMGENHANCESTR
{
	UCHAR	OpCode;					//	0...锐化，1...现场均衡化，2...捺印均衡化
    	unsigned char ModuleWidth1; //均衡化模板窗口（0，>=5），缺省值是9， 设置0时候自动设置为9
	unsigned char nRes[6];
	int		ImgWidth,ImgHeight;		//	图像长宽
	int		WinWidth,WinHeight;		//	增强区域的长宽（如果为0的话，代表对全图增强）
	int		WinOffsetX,WinOffsetY;	//	增强区域的左上角位置（x是竖直方向，y是水平方向）
	int		nBrightNess,nConstrast;	//	亮度（0－255,缺省设为128）,对比度（0－255,缺省设为128）
	UCHAR	*InImg,*OutImg;			//	原图像和增强图像
}  IMGENHANCESTR;

////////  新的图像增强函数(支持局部增强，可以替代原有的增强函数）  //////
NIRVANA_EXPORT int IMG_EnhanceNew(IMGENHANCESTR *pEnhanceData);
/*
	函数返回值：1		......	成功
				<0		......	失败,错误代码为KERROR_XXX
*/


//图像增强方法
#define IMGENHANCE_METHOD_LOCAL		0				//	局部增强
#define IMGENHANCE_METHOD_EQUAL		1				//	均衡化
#define IMGENHANCE_METHOD_HIST		2				//	直方图拉伸
#define IMGENHANCE_METHOD_GABOR1	3				//	GABOR1变换，允许在部分区域上人工描线辅助下进行图像增强
#define IMGENHANCE_METHOD_GABOR2	4				//	GABOR2变换，按照人工给定的纹线方向进行图像增强

//图像描述信息
#define IMGENHANCE_AREA_MASK		0				//	非增强区域
#define IMGENHANCE_AREA_WORK		1				//	需增强的工作区域，自动计算增强图像
#define IMGENHANCE_AREA_RIDGE		2				//	人工描线区域，用于描述所在点（包括周围）的纹线方向来辅助进行图像增强，该区域在工作区域内,   在IMGENHANCE_METHOD_GABOR1使用

typedef struct tagIMGENHANCESTR70
{
	unsigned char	OpCode;					//	图像增强方法，由IMGENHANCE_METHOD_XXX定义
	unsigned char	nEnhancePara1;			//	图像增强参数1
	unsigned char	nEnhancePara2;			//	图像增强参数2
	unsigned char	nEnhancePara3;			//	图像增强参数3
	int				nWidth,nHeight;			//	图像宽度和高度
	char			nRes1[12];				
	unsigned char	*InImg;					//	输入的图像指针
	unsigned char	*WorkImg;				//	图像描述信息	由IMGENHANCE_AREA_XXX定义， WorkImg可以为空，表示对全图自动增强
	unsigned char	*OutImg;				//	输出的图像指针
	char			nRes2[32];
}  IMGENHANCESTR70;

/***  图像增强参数在不同增强方法下含义以及定义范围和缺省值	***
方法：IMGENHANCE_METHOD_LOCAL
参数1：亮度，				0-255（缺省值 128）
参数2：对比度，				0-255（缺省值 128）
方法：IMGENHANCE_METHOD_EQUAL
参数1：亮度，				0-255（缺省值 128）
参数2：对比度，				0-255（缺省值 128）
参数3：均衡化模板窗口半宽,,	2-8	  (缺省值是5)，设置为0时候使用缺省值
方法：IMGENHANCE_METHOD_HIST
参数1：亮度，				0-255（缺省值 128）
参数2：对比度，				0-255（缺省值 128）
方法：IMGENHANCE_METHOD_GABOR1
参数1：纹线宽度，			4-16 （缺省值 10）设置为0时候使用缺省值
参数2：Gabor算法强度因子	0-255（缺省值 128）
方法：IMGENHANCE_METHOD_GABOR2
参数1：纹线宽度，			4-16 （缺省值 10）设置为0时候使用缺省值
参数2：Gabor算法强度因子	0-255（缺省值 128）
参数3：指定纹线方向,		0-179 (缺省值是0)
***************************************************************/

////////  新的图像增强函数(支持指定任意形状区域增强，支持在人工描线辅助增强指纹图像，可以替代原有所有的增强函数）  //////
NIRVANA_EXPORT int IMG_Enhance_All(IMGENHANCESTR70 *pEnhanceData);
/*
函数返回值：1		......	成功
<0		......	失败,错误代码为KERROR_XXX
*/



NIRVANA_END_API

#endif //_NIRVANA_IMG_ENHANCER_H_
