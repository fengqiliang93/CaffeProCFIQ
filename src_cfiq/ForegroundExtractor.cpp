#include "ForegroundExtractor.h"

#include <cstdio>
#include <cstring>
#include <dlfcn.h>

typedef int (*pGAFIS_ExtractBack)(unsigned char *bufferPix,
                                  int width,
                                  int height,
                                  int fgp,
                                  unsigned char *foreGroundBuffer);

bool ExtractForeground(void *pHInstance,
                       unsigned char *bufferPix,
                       int width,
                       int height,
                       int fgp,
                       unsigned char *foreGroundBuffer)
{
    dlerror();
    pGAFIS_ExtractBack GAFIS_ExtractBack =
        (pGAFIS_ExtractBack)dlsym(pHInstance, "GAFIS_ExtractBack");
    char *error = dlerror();
    if (error != NULL || GAFIS_ExtractBack == NULL)
    {
        printf("Load GAFIS_ExtractBack Error:(%s)\n", error == NULL ? "symbol not found" : error);
        fgetc(stdin);
        return false;
    }

    memset(foreGroundBuffer, 0, width * height);
    const int ret = GAFIS_ExtractBack(bufferPix, width, height, fgp, foreGroundBuffer);
    if (ret < 0)
    {
        fprintf(stderr, "GAFIS_ExtractBack failed: %d\n", ret);
        return false;
    }
    return true;
}
