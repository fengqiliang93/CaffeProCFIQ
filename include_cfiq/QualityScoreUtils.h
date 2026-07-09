#pragma once

static const int CFIQ_MAX_RANK_WEIGHT = 5;

inline int CFIQRankWeight(int rank)
{
    switch (rank)
    {
    case 1:
        return 1;
    case 2:
        return 3;
    case 3:
        return 5;
    default:
        return 0;
    }
}

inline void CFIQUseForegroundCenterWhenCoreMissing(const unsigned char *foreground,
                                                   int width,
                                                   int height,
                                                   int *coreX,
                                                   int *coreY)
{
    if (foreground == 0 || coreX == 0 || coreY == 0)
        return;
    if (*coreX != 0 || *coreY != 0)
        return;

    long long sumRow = 0;
    long long sumCol = 0;
    long long foregroundCount = 0;

    for (int row = 0; row < height; ++row)
    {
        const unsigned char *rowPtr = foreground + row * width;
        for (int col = 0; col < width; ++col)
        {
            if (rowPtr[col] == 0)
                continue;
            sumRow += row;
            sumCol += col;
            ++foregroundCount;
        }
    }

    if (foregroundCount == 0)
    {
        *coreX = width / 2;
        *coreY = height / 2;
        return;
    }

    *coreX = static_cast<int>((sumCol + foregroundCount / 2) / foregroundCount);
    *coreY = static_cast<int>((sumRow + foregroundCount / 2) / foregroundCount);
}
