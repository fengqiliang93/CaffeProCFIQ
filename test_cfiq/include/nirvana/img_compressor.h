// Copyright (c) 2014 The Nirvana Authors. All rights reserved.

/*
 * 图像压缩相关
 * @file img_compressor.h
 */
#ifndef _NIRVANA_IMG_COMPRESSOR_H_
#define	_NIRVANA_IMG_COMPRESSOR_H_

NIRVANA_BEGIN_API


//手纹图像压缩
NIRVANA_EXPORT int GAFIS_CompressIMG(
	GAFISIMAGESTRUCT *pImage,		//	图象指针（单枚）
	GAFISIMAGESTRUCT *cpr,		//	返回的压缩图象指针（单枚）
	int				cprmethod,		//	压缩方法，目前固定值为0, 1为EZW压缩方法适合低倍率高保真的压缩
	int				compress_ratio );		//	压缩倍率2...30
/*
函数返回值：	1	......	成功
				<0	......	失败,错误代码为KERROR_XXX
*/

/////////////////	    	手纹图象解压    	///////////////////
NIRVANA_EXPORT int GAFIS_DecompressIMG(
	GAFISIMAGESTRUCT *cpr,			//	返回的压缩图象指针（单枚）
	GAFISIMAGESTRUCT *pImage);		//	图象指针（单枚）
/*
函数返回值：	1	......	成功
				<0	......	失败,错误代码为KERROR_XXX
*/


NIRVANA_EXPORT int GAFIS_CompressData(
	GAFISIMAGESTRUCT *img,			//	原始图象指针
	GAFISIMAGESTRUCT *cpr)	;		//	压缩图象指针
/*
函数返回值：	1	......	成功
				<0	......	失败,错误代码为KERROR_XXX
*/

NIRVANA_EXPORT int GAFIS_DeCompressData(GAFISIMAGESTRUCT *cpr,GAFISIMAGESTRUCT *img);
/*
函数返回值：	1	......	成功
				<0	......	失败,错误代码为KERROR_XXX
*/

NIRVANA_END_API

#endif	//_NIRVANA_IMAGE_COMPRESSOR_H_

