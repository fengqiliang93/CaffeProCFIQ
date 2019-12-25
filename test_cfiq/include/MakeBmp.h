#include"bmp.h"
#include<iostream>
#include<fstream>
using namespace std;
void MakeGrayBmp(unsigned char *ImageDataBuffer, int width, int height, bool normalOrader, string FullFilePath);
void MakeRGBBmp(unsigned char *ImageDataBuffer, int width, int height, bool normalOrder, string FullFilePath);
