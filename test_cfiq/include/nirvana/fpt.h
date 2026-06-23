// Copyright (c) 2014 The Nirvana Authors. All rights reserved.

/**
 * fpt转换相关函数
 * @file	fpt.h
 */
#ifndef _NIRVANA_FPT_H_
#define _NIRVANA_FPT_H_

NIRVANA_BEGIN_API



/**
 * 数据交换时对金指格式压缩数据进行加密，在导出前和导入后使用.
 * @param cprdata 压缩数据.
 * @return =1  ......  成功 <0	......	失败,错误代码为KERROR_XXX.
 */
NIRVANA_EXPORT
int FPT_datatranform(GAFISIMAGESTRUCT *cprdata);
	


NIRVANA_END_API

#endif //_NIRVANA_FPT_H_

