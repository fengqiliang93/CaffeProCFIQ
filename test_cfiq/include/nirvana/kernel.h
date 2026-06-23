// Copyright (c) 2014 The Nirvana Authors. All rights reserved.

/*
 * 核心头文件，供其他程序调用.
 * 所有调用算法的程序，只包含此文件即可
 * @file kernel.h
 */

#ifndef _NIRVANA_KERNEL_H_
#define _NIRVANA_KERNEL_H_

//dll export
#ifdef DLL_EXPORT
#ifndef NIRVANA_EXPORT
# if defined(NIRVANA_HAVE_WINDOWS)
#   if defined(STATIC_LINKED)
#     define NIRVANA_EXPORT
#   else
#	  define NIRVANA_EXPORT_ALIGN16 __declspec(align(16)) 
#     define NIRVANA_EXPORT __declspec(dllexport)
#   endif
# else
#   if defined(__GNUC__) && defined(GCC_HASCLASSVISIBILITY)
#     define NIRVANA_EXPORT __attribute__ ((visibility("default")))
#     define NIRVANA_EXPORT_ALIGN16
#   else
#     define NIRVANA_EXPORT
#     define NIRVANA_EXPORT_ALIGN16
#   endif
# endif
#endif
#else
#     define NIRVANA_EXPORT
#     define NIRVANA_EXPORT_ALIGN16
#endif

//nirvana dll api
#ifdef __cplusplus
#define NIRVANA_BEGIN_API extern "C" {
#define NIRVANA_END_API }
#else
#define NIRVANA_BEGIN_API
#define NIRVANA_END_API
#endif


#define	KERROR_UNKNOWN			-1		//	未知错误
#define KERROR_PARAMETER		-2		//	参数错误
#define	KERROR_DATA				-3		//	数据错误
#define	KERROR_MEMORY			-4		//	内存申请错误
#define	KERROR_LICENSE			-5		//	使用权限错误

#define	LIVESCAN_QUALITY_PASS		1	//	活体质量通过
#define LIVESCAN_QUALITY_OTHERERR	-1	//	其他错误
#define LIVESCAN_QUALITY_WARNNING	-2	//	质量警告,允许强行通过
#define LIVESCAN_QUALITY_UNPASS		-3	//	质量不通过,必须重新采集

#define PALM_LVQLEV_AREA		-11		//	捺印有效面积不够大
#define PALM_LVQLEV_SIZE		-12		//	宽度或高度不够
#define PALM_LVQLEV_ROOT		-13		//	指根区域捺印不完整
#define PALM_LVQLEV_WRIST		-14		//	腕部区域捺印不完整
#define PALM_LVQLEV_HOLE		-15		//	掌心区域捺印不完整
#define PALM_LVQLEV_LEAVE		-16		//	发现残留掌纹
#define PALM_LVQLEV_DIRTY		-17		//	采集窗口脏
#define PALM_LVQLEV_WHITE		-18		//	图像偏白
#define PALM_LVQLEV_DARK		-19		//	图像偏黑
#define PALM_LVQLEV_LOWCONTRAST	-20		//	图像反差小


typedef  unsigned char  UCHAR;
typedef	short	int2;
typedef	unsigned short	uint2;
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;


#include "image_def.h"
#include "mnt_def.h"
#include "mnt_checker_def.h"

#include "barcode.h"
#include "extractor.h"
#include "fpt.h"
#include "img_compressor.h"
#include "img_enhancer.h"
#include "img_split.h"
#include "line_count.h"
#include "match.h"
#include "mnt_checker.h"
#endif //_NIRVANA_KERNEL_H_

