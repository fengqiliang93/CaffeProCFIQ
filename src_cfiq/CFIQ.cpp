#include "CFIQ.h"

void GetQualityScoreModeOne(void *pForeground,
                            void *pRowColPair,
                            void *pHeatMapFloat,
                            void *pCaffeNet,
                            void *pHInstance,
                            void *pQuality,
                            unsigned char *srcBmp,
                            int width,
                            int height,
                            int fgp,
                            int Core_X,
                            int Core_Y,
                            int localSize,
                            int stepSize,
                            float *QualityScore);
void GetQualityScoreModeTwo(void *pForeground,
                            void *pRowColPair,
                            void *pHeatMapFloat,
                            void *pCaffeNet,
                            void *pHInstance,
                            void *pQuality,
                            unsigned char *srcBmp,
                            int width,
                            int height,
                            int fgp,
                            int Core_X,
                            int Core_Y,
                            int localSize,
                            int stepSize,
                            float *QualityScore);
void GetQualityScoreModeThree(void *pForeground,
                              void *pRowColPair,
                              void *pHeatMapFloat,
                              void *pCaffeNet,
                              void *pHInstance,
                              void *pQuality,
                              unsigned char *srcBmp,
                              int width,
                              int height,
                              int fgp,
                              int Core_X,
                              int Core_Y,
                              int localSize,
                              int stepSize,
                              float *QualityScore);
void GetQualityScoreModeFour(void *pForeground,
                             void *pRowColPair,
                             void *pHeatMapFloat,
                             void *pCaffeNet,
                             void *pHInstance,
                             void *pQuality,
                             unsigned char *srcBmp,
                             int width,
                             int height,
                             int fgp,
                             int Core_X,
                             int Core_Y,
                             int localSize,
                             int stepSize,
                             float *QualityScore);
void GetQualityScoreModeFive(void *pForeground,
                             void *pRowColPair,
                             void *pHeatMapFloat,
                             void *pCaffeNet,
                             void *pHInstance,
                             void *pQuality,
                             unsigned char *srcBmp,
                             int width,
                             int height,
                             int fgp,
                             int Core_X,
                             int Core_Y,
                             int localSize,
                             int stepSize,
                             float *QualityScore);
void GetQualityScoreModeSix(void *pForeground,
                            void *pRowColPair,
                            void *pHeatMapFloat,
                            void *pCaffeNet,
                            void *pHInstance,
                            void *pQuality,
                            unsigned char *srcBmp,
                            int width,
                            int height,
                            int fgp,
                            int Core_X,
                            int Core_Y,
                            int localSize,
                            int stepSize,
                            float *QualityScore);

void GetQualityScore(void *pForeground,
                     void *pRowColPair,
                     void *pHeatMapFloat,
                     void *pCaffeNet,
                     void *pHInstance,
                     void *pQuality,
                     unsigned char *srcBmp,
                     int width,
                     int height,
                     int fgp,
                     int Core_X,
                     int Core_Y,
                     int localSize,
                     int stepSize,
                     unsigned char modeNumber,
                     float *QualityScore)
{
    if (modeNumber == 1)
    {
        GetQualityScoreModeOne(pForeground, pRowColPair, pHeatMapFloat, pCaffeNet, pHInstance, pQuality, srcBmp, width, height, fgp, Core_X, Core_Y, localSize, stepSize, QualityScore);
    }
    else if (modeNumber == 2)
    {
        GetQualityScoreModeTwo(pForeground, pRowColPair, pHeatMapFloat, pCaffeNet, pHInstance, pQuality, srcBmp, width, height, fgp, Core_X, Core_Y, localSize, stepSize, QualityScore);
    }
    else if (modeNumber == 3)
    {
        GetQualityScoreModeThree(pForeground, pRowColPair, pHeatMapFloat, pCaffeNet, pHInstance, pQuality, srcBmp, width, height, fgp, Core_X, Core_Y, localSize, stepSize, QualityScore);
    }
    else if (modeNumber == 4)
    {
        GetQualityScoreModeFour(pForeground, pRowColPair, pHeatMapFloat, pCaffeNet, pHInstance, pQuality, srcBmp, width, height, fgp, Core_X, Core_Y, localSize, stepSize, QualityScore);
    }
    else if (modeNumber == 5)
    {
        GetQualityScoreModeFive(pForeground, pRowColPair, pHeatMapFloat, pCaffeNet, pHInstance, pQuality, srcBmp, width, height, fgp, Core_X, Core_Y, localSize, stepSize, QualityScore);
    }
    else if (modeNumber == 6)
    {
        GetQualityScoreModeSix(pForeground, pRowColPair, pHeatMapFloat, pCaffeNet, pHInstance, pQuality, srcBmp, width, height, fgp, Core_X, Core_Y, localSize, stepSize, QualityScore);
    }
}

void __attribute__((constructor)) before_main(void)
{
}

void __attribute__((destructor)) after_main(void)
{
}
