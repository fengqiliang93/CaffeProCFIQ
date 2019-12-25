#pragma once

extern "C" void InitializeDll(void **pForeGroundBuffer, void **pRowColPairStruct, void **pHeatMapFloat, void **pCaffeNet, void **pHInstance, void **pQuality, char *pPrototxtPath, char *pCaffeModelPath);
extern "C" void GetQualityScore(void *pForeGroundBuffer, void *pRowColPairStruct, void *pHeatMapFloat, void *pCaffeNet, void *pHInstance, void *pQuality, unsigned char *srcBmp, int width, int height, int fgp, int Core_X, int Core_Y, int localSize, int stepSize, unsigned char modeNumber, float *QualityScore);
extern "C" void DestructDLL(void **pForeGroundBuffer, void **pRowColPairStruct, void **pHeatMapFloat, void **pCaffeNet, void **pHInstance, void **pQuality);
/*
InitializeDll接口函数与DestructDLL接口函数前六个参数传参数方式一样，且顺序一致。下面统一说明：
void **pForeGroundBuffer
void **pRowColPairStruct 
void **pHeatMapFloat
void **pCaffeNet
void **pHInstance
void **pQuality

以上六个参数 需要提前声明指针，并传入指针的地址
如：
	void *pForeGroundBuffer;
	void *pRowColPairStruct; 
	void *pHeatMapFloat;
	void *pCaffeNet;
	void *pHInstance;
	void *pQuality;
	InitializeDll(&pForeGroundBuffer,&pRowColPairStruct, &pHeatMapFloat, &pCaffeNet, &pHInstance, &pQuality, char *pPrototxtPath, char *pCaffeModelPath); //申请分配空间，pPrototxtPath，pCaffeModelPath参数传法在后边
	.......
	GetQualityScore(pForeGroundBuffer, pRowColPairStruct, pHeatMapFloat, pCaffeNet, pHInstance, pQuality,.................);//使用空间
	
		//void *pQuality;				==>返回的质量图（BGR 彩色图，像素部分，像素顺序从左上角开始）
	.......
	DestructDLL(&pForeGroundBuffer,&pRowColPairStruct, &pHeatMapFloat, &pCaffeNet, &pHInstance, &pQuality); //释放空间

注意：传递顺序，必须一致;
*/

/* InitializeDll最后两个参数传递说明：
 * char *pPrototxtPath, char *pCaffeModelPath
 * pPrototxtPath 是 .prototxt 文件的路径，如： ....(自己的路径)/FQNet_model/deploy.prototxt
 * pCaffeModelPath 是 .caffemodel 文件路径， 如： ....（自己的路径）/FQNet_model/_iter_50000.caffemodel
 */

/*
GetQualityScore函数传参说明：
前六个参数传递方式上面已经指明，这里不再赘述。

unsigned char *srcBmp       ==>解压缩后的图像数据（去掉头，只传像素部分, 像素顺序从左上角开始（不是从左下角））
int width					==>图像宽度，传640
int height					==>图像高度，传640
int fgp						==>指位号，1-10
int Core_X					==>中心点的X坐标，即列号
int Core_Y					==>中心点的Y坐标，即行号
int localSize				==>局部小块的大小，目前传48
int stepSize				==>stepSize，局部小块的间隔，一般取16
unsigned char modeNumber    ==>选择质量图的绘图模式，一般传1（辽宁模式）；modeNumber 可取 1，2，3.
float *QualityScore			==>返回的分数

重复声明：
void *pQuality;				==>返回的质量图（BGR 彩色图，像素部分，像素顺序从左上角开始）

郑重声明：
1.多线程程序，每个线程开始打分前都要各调用一次InitializeDll函数
2.中间调用GetQualityScore
	可根据pQuality 获取质量图
	可根据QualityScore获取分数
3.每个线程退出前都各调用一次DestructDLL函数，以释放空间

*/
