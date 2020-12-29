// /*
//  * main.c
//  */
 #include <stdio.h>
 #include <math.h>
 #include "custombmplib.h"

 const char iFileName[] = "lena512.bmp";
 const char oFileName[] = "result10.bmp";
 const int output_size[2] = {768, 768};	//output HEIGHT & WIDTH

 BYTE* imresize(BYTE* psrc, int iWidth, int iHeight, int oWidth, int oHeight, WORD bitCount);

 int main(void) {
 	BITMAPFILEHEADER ibmfh, obmfh;	//inBitmapFileHead, outBitmapFileHead
     BITMAPINFOHEADER ibmih, obmih;	//inBitmapInfoHead, outBitmapInfoHead
     FILE* pifile = fopen(iFileName, "rb");	//pointer input file
     FILE* pofile = fopen(oFileName, "wb");	//pointer output file

 	//judge file format
 	WORD fileType;
 	fread(&fileType, 1, sizeof(WORD), pifile);
     if (fileType != 0x4d42)
     {
         printf("This file is not a .bmp file!");
         return -1;
     }

 	//read input file head
 	rewind(pifile);
 	fread(&ibmfh, sizeof(BITMAPFILEHEADER), 1, pifile);
 	fread(&ibmih, sizeof(BITMAPINFOHEADER), 1, pifile);

 	//write output file head
 	obmfh = ibmfh;
 	obmih = ibmih;
 	obmih.biHeight = output_size[0];
 	obmih.biWidth = output_size[1];
 	int oWidthByte = WIDTHBYTES(obmih.biWidth*obmih.biBitCount);	//���ͼ��ÿ������Ӧռ�ֽ�
 	obmih.biSizeImage = oWidthByte * obmih.biHeight;		//�������ͼ��������ռ�ֽ�
 	obmfh.bfSize = obmfh.bfOffBits + obmih.biSizeImage;		//���ͼ���ļ����ֽڣ������ļ�ͷ�����ݣ�
 	fwrite(&obmfh, 1, sizeof(BITMAPFILEHEADER), pofile);
 	fwrite(&obmih, 1, sizeof(BITMAPINFOHEADER), pofile);
	
 	//save palette
 	size_t Offbit_File_Info = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
 	BYTE *pPale = (BYTE *)malloc(ibmfh.bfOffBits-Offbit_File_Info);	//pointer palette
 	fread(pPale, 1, ibmfh.bfOffBits-Offbit_File_Info, pifile);
 	fwrite(pPale, 1, ibmfh.bfOffBits-Offbit_File_Info, pofile);

 	//read color data
 	unsigned int iWidthByte = WIDTHBYTES(ibmih.biWidth * ibmih.biBitCount);	//����ͼ��ÿ������Ӧռ�ֽ�
 	BYTE *piColorData = (BYTE *)malloc(ibmih.biSizeImage);	//����洢����ͼ�����ݵĿռ�
 	fread(piColorData, 1, ibmih.biSizeImage, pifile);

 	//bilinear interpolation resize
 	BYTE *poColorData;	//����洢���ͼ�����ݵĿռ�
 	poColorData = imresize(piColorData, ibmih.biWidth, ibmih.biHeight, obmih.biWidth, obmih.biHeight, ibmih.biBitCount);

 	fwrite(poColorData, 1, obmih.biSizeImage, pofile);   //��������ͼ��������д���ļ�
 	printf("finish.\n");
 	fclose(pifile);
     fclose(pofile);
 	return 0;
 }

 BYTE* imresize(BYTE *piColorData, int iWidth, int iHeight, int oWidth, int oHeight, WORD bitCount){
 	DWORD oSizeImage = WIDTHBYTES(oWidth*bitCount) * oHeight;
 	BYTE *poColorData = (BYTE *)malloc(oSizeImage);	//����洢���ͼ�����ݵĿռ�
 	memset(poColorData, 0, oSizeImage);

 	int y, x;						//y:output img y, x: output img x
 	float ori_y, ori_x;				//origin_y, origin_x (Ŀ��ͼ�������Ӧԭͼ����)
 	float ori_a, ori_b, ori_c, ori_d;	//������Χ�����µ�(ori_a, ori_b)��������Χ�����ϵ�(ori_c, ori_d)
 	int position1, position2, position3, position4;	//ԭͼ����ĸ�����λ��,���¡����¡����ϡ�����
 	float gray_level1, gray_level2;	//˫�����㷨��Ӧ�������м�ֵ
 	float ratiow = (float)iWidth / oWidth;
 	float ratioh = (float)iHeight / oHeight;
 	for (y = 0; y < oHeight; y++){
 		printf("y=%d\n",y);
 		for (x = 0; x < oWidth; x++){
 			ori_y = y * ratioh;
 			ori_x = x * ratiow;
 			ori_a = floor(ori_x);
 			ori_b = floor(ori_y);
 			ori_c = ceil(ori_x);
 			ori_d = ceil(ori_y);

 			position1 = ori_a + ori_b * iWidth;
 			position2 = ori_c + ori_b * iWidth;
 			position3 = ori_a + ori_d * iWidth;
 			position4 = ori_c + ori_d * iWidth;

 			gray_level1 = piColorData[position1] + (piColorData[position2] - piColorData[position1])*(ori_x - ori_a);
 			gray_level2 = piColorData[position3] + (piColorData[position4] - piColorData[position3])*(ori_x - ori_a);

 			poColorData[x+y*oWidth] = gray_level1 + (gray_level2 - gray_level1)*(ori_y-ori_b);
 		}
 	}
 	return poColorData;
 }
//#include <stdio.h>
//#include <math.h>
//extern int division(int a, int b);
//extern int ceil(int);
//extern int floor(int);
//float show_fix_point(int);
//int main(){
//	int dividend[] = {0x000B8000,0x00048000,0x00010000,0x00010000,0x00641000,0x00010000,0x02000000};
//	int divisor[] = {0x00038000,0x00018000,0x00020000,0x00031000,0x00021000,0x00030000,0x03000000};
//	int i = 0;
//	for (;i<7;i++){
//		int a = dividend[i];
//		int b = divisor[i];
//		int res = ceil(a);
//		int res2 = floor(b);
//		float s_a = show_fix_point(a);
//		float s_b = show_fix_point(b);
//		float s_res = show_fix_point(res);
//		float s_res_2 = show_fix_point(res2);
//		printf("%f:%f,%f:%f\n",s_a,s_res,s_b,s_res_2);
//	}
//}
//float show_fix_point(int a){
//	int integer = a>>16;
//	int i =16;
//	float result = 0;
//	for (; i>0; i--){
//		if (a % 2 ==1)
//			result += pow(2,-i);
//		a = a >> 1;
//	}
//	float res = (float)integer + result;
//	//printf("%f",res);
//	return res;
//}
