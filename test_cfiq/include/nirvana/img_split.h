// Copyright (c) 2014 The Nirvana Authors. All rights reserved.

/**
 * 图像分割头文件
 * @file img_split.h
 */
#ifndef _NIRVANA_IMG_SPLIT_H_
#define _NIRVANA_IMG_SPLIT_H_

NIRVANA_BEGIN_API

typedef struct {
		int vx0;		//	预设的采集框中心位置(纵向) 
		int hy0;		//	预设的采集框中心位置(横向) 
		int vx;			//	自动寻找的采集框中心位置(纵向) 
		int hy;			//	自动寻找的采集框中心位置(横向) 
		int noflag;		//	缺指标记	1...缺指， 0...正常 
} FINGERPOS;

///////////////      滚动指纹自动切割(整数倍缩小的图像）     ////////////////////
NIRVANA_EXPORT int GAFIS_SpliteImage(unsigned char *scaleimg,FINGERPOS *fgpos,int fgsum,int height,int width,int sc);
/* 
	函数返回值：1		......	成功
				<0		......	失败,错误代码为KERROR_XXX
	参数说明：	scaleimg......	当前按sc比例缩小过的图象指针
				height	......	scaleimg图象高度
				width	......	scaleimg图象宽度
				fgpos	......  自动定位指纹参数数组，主要位置参数已根据sc相应调整。
				fgsum	......	指纹的个数，fgpos数组的实际使用长度
				sc 		......	缩小比例
***/


///////////////      滚动指纹自动切割(任意缩小的图像）     ////////////////////
NIRVANA_EXPORT int GAFIS_SpliteImage_Ex(unsigned char *timg,
								FINGERPOS *fgpos,
								int fgsum,
								int theight,int twidth,
								double sc);


typedef struct {
	int x;				//	（输入输出）对象的左上角位置（相对图象）
	int y;
	int beExist;		//	（输出）存在标记，0...不存在，1...存在
	int nAngle;			//	（输出）旋转角度  垂直向下是0度，右边是正角度，左边是负角度
	int nWidth;			//	（输入输出）对象宽度
	int nHeight;		//	（输入输出）对象长度
	int nObjectType;	//	（输入输出）对象类型，对这个参数必须事先赋值，输入参数中表示输入的数据的类型，输出参数中表示希望返回的对象类型
} REGIONRECT;

//////////////////      平面指纹和掌纹自动切割和定位（方向）    ////////////////////
NIRVANA_EXPORT int  GAFIS_SpliteImageNew(unsigned char *srcimg,int width,int height,REGIONRECT *srcobj,REGIONRECT *destobj,int destcnt);
/* 
	函数返回值：1		......	成功
				<0		......	失败,错误代码为KERROR_XXX
	参数说明：	srcimg......	图象指针
				height	......	图象高度
				width	......	图象宽度
				srcobj	......  初始对象的指针（单个）
				destobj	......	返回的对象的数组指针
				destcnt	......	希望返回的对象的个数
*/

//////////////////           十指卡自动切割和定位          ////////////////////
NIRVANA_EXPORT int GAFIS_SpliteImageNew_Auto(unsigned char *srcimg,int width,int height,int card_type,REGIONRECT *destobj,int destcnt);
/* 
	函数返回值：1		......	成功
				<0		......	失败,错误代码为KERROR_XXX
	参数说明：	srcimg......	图象指针
				height	......	图象高度
				width	......	图象宽度
				card_type	......  0...标准卡正面（指纹捺印），1...标准卡方面（掌纹捺印）
				destobj	......	返回的对象的数组指针
				destcnt	......	希望返回的对象的个数
*/


NIRVANA_END_API

#endif //_NIRVANA_IMG_SPLIT_H_