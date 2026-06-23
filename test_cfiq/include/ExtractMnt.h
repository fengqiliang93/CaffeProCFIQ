/*
 * ExtractMnt.h
 *
 *  Created on: Mar 19, 2019
 *      Author: lyon
 */
#ifndef EXTRACTMNT_H_
#define EXTRACTMNT_H_
#include"kernel.h"
void DestructMntExtractor();
int ExtractSingleRaw( unsigned char *pRaw, int width, int height, int Fingertype, FINGERMNTSTRUCT *pMnt );
int GetCorePosition(FINGERMNTSTRUCT* pMnt, int* cRow, int* cCol);
#endif /* EXTRACTMNT_H_ */
