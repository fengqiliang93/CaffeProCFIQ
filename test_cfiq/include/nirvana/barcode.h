// Copyright (c) 2014 The Nirvana Authors. All rights reserved.

/**
 * 条码相关的头文件
 * @file	barcode.h
 * @author	Jun Tsai
 * @since 2014.7.19
 */

#ifndef _NIRVANA_BARCODE_H_
#define _NIRVANA_BARCODE_H_

NIRVANA_BEGIN_API

 /**
 * 条码最小宽度
 * @param code_type 条码类型(“39” 或 “128”或“25” ）
 * @param code_len 条码字符个数
 * @param pdpi 条码打印分辨率
 * @return  大于0 则为条码的最小宽度，0，则为参数错误
 */ 
 NIRVANA_EXPORT double  MiniBarWidth(char *code_type,int code_len,int pdpi);
 
 /**
  * 
  * @param code_type 条码类型，目前仅仅支持128的条码
  * @param bar_code 条码，如果长度不够，则在前面加0
  * @param pdpi 打印的分辨率
  * @param pwidth 输出条码的实际宽度
  * @param pheight 数据条码的实际高度
  * @param barimage 生成的条码图象的指针，函数内部申请的，用完请释放
  * @param imagewidth 生成条码图像的宽(像素)
  * @param imageheight 生成条码的图像高度(像素)
  * @return 1 成功，<0 失败，错误代码为 KERROR_XXX
  */
 NIRVANA_EXPORT int Creat_BarImage(char *code_type,char *bar_code, 
	 int pdpi, double pwidth,double pheight, 
	 unsigned char **barimage,int *imagewidth,int *imageheight);

 /**
  * 条码自动识别
  * @param srcimg 原始图像指针
  * @param width 原始图像宽度
  * @param height 原始图像高度
  * @param bar_type 条码类型 （“39” 或 “128”或“25” ）
  * @param barcode 条码,返回的条码值
  * @param bar_len 条码的长度（预先设置，对25码起作用，缺省为0）
  * @return 
  */
  NIRVANA_EXPORT int GAFIS_BarcodeRecognize(unsigned char *srcimg,int width,int height,
	  char *bar_type,char *barcode,int bar_len);


NIRVANA_END_API

#endif //_NIRVANA_BARCODE_H_

