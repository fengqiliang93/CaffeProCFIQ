#pragma pack(1)
typedef struct//λͼ�ļ�ͷ��14���ֽ�
{
	unsigned short    bfType; //windows ��ȡΪ0x4d42�����ַ���"BM"����ͬϵͳ�¿���ȡֵ��̫һ����
	unsigned int    bfSize; //����bmp�ļ��Ĵ�С���������Ĳ�����ɡ�λͼ�ļ�ͷ��λͼ��Ϣͷ������ɫ�壨��ɫ������λͼ����
	unsigned short    bfReserved1;//�����֡���Ϊ0
	unsigned short    bfReserved2; //�����֡���Ϊ0
	unsigned int    bfOffBits; //���ļ�ͷ��ʵ�ʵ�λͼͼ�����ݵ�ƫ���ֽ�����
} BitMapFileHeader;

typedef struct //λͼ��Ϣͷ
{
	unsigned int  biSize; //���ṹ������Ҫ���ֽ���
	int   biWidth; //ͼ��Ŀ�ȣ�������Ϊ��λ
	int   biHeight; //ͼ��ĸ߶ȣ�������Ϊ��λ
	unsigned short   biPlanes; //ΪĿ���豸˵����ɫƽ��������ֵ���Ǳ���Ϊ1��
	unsigned short   biBitCount;//˵��������/���أ���ֵΪ1��4����8��16��24��32��
	unsigned int  biCompression; //˵��ͼ������ѹ�������͡�ȡֵ��Χ��
								  //0 BI_RGB ��ѹ������ã�
								  //1 BI_RLE8 8�����γ̱��루RLE����ֻ����8λλͼ
								  //2 BI_RLE4 4�����γ̱��루RLE����ֻ����4λλͼ
								  //3 BI_BITFIELDS ����������16/32λλͼ
								  //4 BI_JPG JPEG λͼ��JPEGͼ�񣨽����ڴ�ӡ����
								  //5 BI_PNG PNG  λͼ��PNGͼ�񣨽����ڴ�ӡ�ӣ�

	unsigned int  biSizeImage;   //˵��ͼ��Ĵ�С�����ֽ�Ϊ��λ������BI_RGB��ʽʱ��������Ϊ0.
	                              // ͼ������ = Width * Height * ��ʾÿ��������ɫռ�õ�byte��

	int   biXPelsPerMeter;       //˵���豸��ˮƽ�ֱ��ʣ�������/�ױ�ʾ���з���������ȱʡֵΪ0
	int   biYPelsPerMeter;       //˵���豸�Ĵ�ֱ�ֱ��ʣ�������/�ױ�ʾ���з���������ȱʡֵΪ0
	unsigned int   biClrUsed;    //˵��λͼʵ��ʹ�õ�����ɫ���е���ɫ�����������Ϊ0����˵��ʹ�����е�ɫ����
	unsigned int   biClrImportant; //˵����ͼ����ʾ����ҪӰ�����ɫ��������Ŀ�������0����ʾ����Ҫ��
} BitMapInfoHeader;

typedef struct //��ɫ�壬��ɫ��
{
	unsigned char rgbBlue; //����ɫ����ɫ����
	unsigned char rgbGreen; //����ɫ����ɫ����
	unsigned char rgbRed; //����ɫ�ĺ�ɫ����
	unsigned char rgbReserved; //����ֵ
} RgbQuad;

#pragma  pack()
//ClImage* clLoadImage(char* path);
//bool clSaveImage(char* path, ClImage* bmpImg);

