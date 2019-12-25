#include"MakeBmp.h"
//#include"Memory.h"
void MakeGrayBmp(unsigned char *ImageDataBuffer, int width, int height, bool normalOrder, string FullFilePath)
{
	BitMapFileHeader *pBitMapFileHeader = new BitMapFileHeader;
	BitMapInfoHeader *pBitMapInfoHeader = new BitMapInfoHeader;
	RgbQuad *pRgbQuad = new RgbQuad[256];

	pBitMapFileHeader->bfType = 0x4d42;
	pBitMapFileHeader->bfSize = 54 + 1024 + width * height;
	pBitMapFileHeader->bfReserved1 = 0;
	pBitMapFileHeader->bfReserved2 = 0;
	pBitMapFileHeader->bfOffBits = 54 + 1024;


	pBitMapInfoHeader->biSize = 40;
	pBitMapInfoHeader->biWidth = width;
	if (normalOrder)
	{
		pBitMapInfoHeader->biHeight = 0 - height;
	}
	else
	{
		pBitMapInfoHeader->biHeight = height;
	}
	pBitMapInfoHeader->biPlanes = 1;
	pBitMapInfoHeader->biBitCount = 8;
	pBitMapInfoHeader->biCompression = 0;
	pBitMapInfoHeader->biSizeImage = width * height;
	pBitMapInfoHeader->biXPelsPerMeter = 0;
	pBitMapInfoHeader->biYPelsPerMeter = 0;
	pBitMapInfoHeader->biClrUsed = 0;
	pBitMapInfoHeader->biClrImportant = 0;

	for (int i = 0; i < 256; i++)
	{
		pRgbQuad[i].rgbBlue = i;
		pRgbQuad[i].rgbGreen = i;
		pRgbQuad[i].rgbRed = i;
		pRgbQuad[i].rgbReserved = 0;
	}

	fstream OutFile(FullFilePath.c_str(), ios::out | ios::binary | ios::ate);
	if (!OutFile)
	{
		cerr << "bmp file open error!" << endl;
		return;
	}
	OutFile.write((char *)pBitMapFileHeader, sizeof(BitMapFileHeader));
	//OutFile.write((char *)pBitMapFileHeader, 14);
	OutFile.write((char *)pBitMapInfoHeader, sizeof(BitMapInfoHeader));
	//OutFile.write((char *)pBitMapInfoHeader, 40);
	OutFile.write((char *)pRgbQuad, sizeof(RgbQuad) * 256);
	OutFile.write((char *)ImageDataBuffer, width * height);

	OutFile.close();
	delete pBitMapFileHeader;
	delete pBitMapInfoHeader;
	delete pRgbQuad;
}
void MakeRGBBmp(unsigned char *ImageDataBuffer, int width, int height, bool normalOrder, string FullFilePath)
{
	BitMapFileHeader *pBitMapFileHeader = new BitMapFileHeader;
	BitMapInfoHeader *pBitMapInfoHeader = new BitMapInfoHeader;
	//RgbQuad *pRgbQuad = new RgbQuad[256];

	pBitMapFileHeader->bfType = 0x4d42;
	pBitMapFileHeader->bfSize = 54 + width * height * 3;
	pBitMapFileHeader->bfReserved1 = 0;
	pBitMapFileHeader->bfReserved2 = 0;
	pBitMapFileHeader->bfOffBits = 54;


	pBitMapInfoHeader->biSize = 40;
	pBitMapInfoHeader->biWidth = width;
	if (normalOrder)
	{
		pBitMapInfoHeader->biHeight = 0 - height;
	}
	else
	{
		pBitMapInfoHeader->biHeight = height;
	}
	pBitMapInfoHeader->biPlanes = 1;
	pBitMapInfoHeader->biBitCount = 24;
	pBitMapInfoHeader->biCompression = 0;
	pBitMapInfoHeader->biSizeImage = width * height * 3;
	pBitMapInfoHeader->biXPelsPerMeter = 0;
	pBitMapInfoHeader->biYPelsPerMeter = 0;
	pBitMapInfoHeader->biClrUsed = 0;
	pBitMapInfoHeader->biClrImportant = 0;

	/*for (int i = 0; i < 256; i++)
	{
	pRgbQuad[i].rgbBlue = i;
	pRgbQuad[i].rgbGreen = i;
	pRgbQuad[i].rgbRed = i;
	pRgbQuad[i].rgbReserved = 0;
	}*/

	fstream OutFile(FullFilePath.c_str(), ios::out | ios::binary | ios::ate);
	if (!OutFile)
	{
		cerr << "bmp file open error!" << endl;
		return;
	}
	OutFile.write((char *)pBitMapFileHeader, sizeof(BitMapFileHeader));
	OutFile.write((char *)pBitMapInfoHeader, sizeof(BitMapInfoHeader));
	//OutFile.write((char *)pBitMapFileHeader, 14);
	//OutFile.write((char *)pBitMapInfoHeader, 40);
	//OutFile.write((char *)pRgbQuad, sizeof(RgbQuad) * 256);
	OutFile.write((char *)ImageDataBuffer, width * height * 3);

	OutFile.close();
	delete pBitMapFileHeader;
	delete pBitMapInfoHeader;
	//delete pRgbQuad;
}
